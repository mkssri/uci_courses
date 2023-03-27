#include "measure.h" //implements
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//to get the perfmon libraries, install libpfm4 in your system
#include <perfmon/pfmlib.h>
#include <perfmon/pfmlib_perf_event.h>

static int initd = 0;
static int measuring = 0;       // how many counters are we measuring
static int fd = -1;             // counters' file descriptor
static uint64_t tmp_time = 0;   // to compute time intervals
static struct read_sformat rsf; // contains the read values and names

//default counters to measure
static char *df_evts[] = {NAM_RET_INS, NAM_CORE_TO_L2M, NAM_CORE_TO_L2H,
                          NAM_L2CHG_X}; // by string
static int df_evts_l = 4;
static uint64_t df_revt[] = {}; // by raw counter number
static int df_revt_l = 0;

inline uint64_t nano_stopwatch(void){
    struct timespec ts;
    //timespec_get(&ts, TIME_UTC); //compile with -std=c11
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec*1000000000 + ts.tv_nsec;
}

static void msr_perf_open(uint64_t *id, struct perf_event_attr pea){
    measuring +=1;
    if(MAX_EVENTS < measuring){
        fprintf(stderr, "Error: trying to measure more than max=%d events.\n",
                MAX_EVENTS);
        exit(-1);
    }
    int fdesc;
    fdesc = perf_event_open(&pea, 0, -1, fd, 0);
    if (fdesc == -1) {
        fprintf(stderr, "Error: Perf couldn't open the counter 0x%lx\n", pea.config);
        exit(EXIT_FAILURE);
    }
    ioctl(fdesc, PERF_EVENT_IOC_ID, id); // collect the ID of this event
    if(fd == -1) //if this was the first call
        fd = fdesc;
}

/**
 ** Performance counters are read in groups that are sent to the PMU together
 ** So you can add up to @MAX_EVENTS events to a group before start reading.
 ** Functions add_name/code a counter to the measurement group by its name or
 ** by its raw code (normally specified in the processor's documentation).
 ** @fd:    File descriptor of the group leader. The first time this
 **         function is called, it has to be a pointer to the value -1,
 **         it is internally replaced for the file descriptor of the group
 **         leader.
 **         For next calls, use that given group leader file descriptor. It
 **         will not be replaced anymore.
 ** @id:    Globaly unique ID given to this counter. Used by measure_read() to
 **         read the counted values.
 ** @event: Event name, obtainable with the library libpfm4.
 ** @code:  Raw code of the event that goes straight to the PMU.
 **/
static void msr_add_name(uint64_t *id, const char *event){
    int ret;
    pfm_perf_encode_arg_t arg;
    struct perf_event_attr pea;
    memset(&arg, 0, sizeof(arg));
    memset(&pea, 0, sizeof(pea));
    arg.attr = &pea;
    arg.size = sizeof(pfm_perf_encode_arg_t);
    pea.size = sizeof(pea);
    pea.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
    ret = pfm_get_os_event_encoding(event, PFM_PLM3, PFM_OS_PERF_EVENT_EXT,
                                    &arg);
    if (ret != PFM_SUCCESS) {
        fprintf(stderr, "Error: Couldn't get OS encoding for %s. ret: %d\n",
                event, ret);
        exit(EXIT_FAILURE);
    }
    msr_perf_open(id, pea);
}

static void msr_add_code(uint64_t *id, uint64_t code){
    struct perf_event_attr pea;
    memset(&pea, 0, sizeof(pea));
    pea.type = PERF_TYPE_RAW;
    pea.size = sizeof(pea);
    pea.config = code;
    pea.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
    pea.disabled = 1;
    pea.exclude_user = 0;
    pea.exclude_kernel = 1;
    pea.exclude_hv = 1;
    msr_perf_open(id, pea);
}

static void msr_reset(){
    if(initd){
        if(0 < fd){
            ioctl(fd, PERF_EVENT_IOC_RESET, 0);
            close(fd);
            fd = -1;
        }
        measuring = 0;
        tmp_time = 0;
        int i;
        for(i=0; i<rsf.nr; i++)
            free(rsf.rec[i].name);
        memset(&rsf, 0, sizeof(rsf));
    }
}

struct read_sformat msr_get(){
    return rsf;
}

void msr_record(){
    tmp_time = nano_stopwatch();
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
}

void msr_pause(){
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    rsf.ntime += nano_stopwatch() - tmp_time;
    
    // read counters from file descriptor
    struct read_format{
        uint64_t nr;
        struct {
            uint64_t val;
            uint64_t id;
        }rec[MAX_EVENTS];
    } rf;
    read(fd, &rf, sizeof(rf));
    for(int i=0; i<rsf.nr; i++){
        for(int j=0; j<rf.nr; j++){
            if(rsf.rec[i].id == rf.rec[j].id){
                rsf.rec[i].val = rf.rec[j].val;
                break;
            }
        }
    }
}

void msr_init(char **sevt, int sl, uint64_t *devt, int dl){
    if(!initd){
        int ret;
        ret = pfm_initialize();
        if (ret != PFM_SUCCESS){
            fprintf(stderr, "Error: Cannot initialize library: ret=%d\n", ret);
            exit(EXIT_FAILURE);
        }
        initd = 1;
    }
    
    msr_reset();
    
    // if not spefified list, use default names
    if(!sevt){
        sevt = df_evts;
        sl = df_evts_l;
    }

    // add events by name
    int l, i = 0;
    for(; i<sl; i++){
        msr_add_name(&rsf.rec[i].id, sevt[i]);
        l = strlen(sevt[i]);
        rsf.rec[i].name = calloc(l,sizeof(char));
        if(!rsf.rec[i].name){
            printf("Error: Cannot allocate memory for event name.\n");
            exit(-1);
        }
        strncpy(rsf.rec[i].name, sevt[i], l);
    }

    // if not specified, use default raw
    if(!devt){
        devt = df_revt;
        dl = df_revt_l;
    }
    for(; i<dl; i++){
        msr_add_code(&rsf.rec[sl+i].id, devt[i]);
        rsf.rec[i].name = calloc(22,sizeof(char));//'r'+log10(ullong)+\0 = 22
        if(!rsf.rec[i].name){
            printf("Error: Cannot allocate memory for event name.\n");
            exit(-1);
        }
        rsf.rec[i].name[0] = 'r';
        snprintf(rsf.rec[i].name+1, 21, "%lu", devt[i]);
    }
    rsf.nr = sl + dl;
    rsf.ntime = 0;
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
}

void msr_print_results(struct read_sformat *to_print, int verb){
    int i;
    char *nam, *name;
    if(!to_print){
        to_print = &rsf;
    }
    switch(verb){
    case 0:
        if(to_print->divd){
            printf("%f", to_print->fntime);
            for(i=0; i<to_print->nr; i++)
                printf(",%f",to_print->rec[i].fval);
            printf("\n");
        }else{
            printf("%lu", to_print->ntime);
            for(i=0; i<to_print->nr; i++)
                printf(",%lu",to_print->rec[i].val);
            printf("\n");
        }
        break;
    case 1:
        if(to_print->divd){
            printf("%20.4f : %s\n", to_print->fntime, "time [ns]");
            for(i=0; i<to_print->nr; i++){
                name = to_print->rec[i].name;
                while((nam = strstr(name, "::")))
                    name = nam+2;
                printf("%20.4f : %s\n",to_print->rec[i].fval, name);
            }
        }else{
            printf("%12lu : %s\n", to_print->ntime, "time [ns]");
            for(i=0; i<to_print->nr; i++){
                name = to_print->rec[i].name;
                while((nam = strstr(name, "::")))
                    name = nam+2;
                printf("%12lu : %s\n",to_print->rec[i].val, name);
            }
        }
        break;
    default: // 2 and more
        if(to_print->divd){
            printf("%20.4f : %s\n", to_print->fntime, "time [ns]");
            for(i=0; i<to_print->nr; i++)
                printf("%20.4f : %s\n",to_print->rec[i].fval,
                       to_print->rec[i].name);
        }else{
            printf("%12lu : %s\n", to_print->ntime, "time [ns]");
            for(i=0; i<to_print->nr; i++)
                printf("%12lu : %s\n",to_print->rec[i].val,
                       to_print->rec[i].name);
        }
    }
}

void accumulate_rsf(struct read_sformat *sum, struct read_sformat *term){
    for(int i=0; i<sum->nr; i++){
        for(int j=0; j<term->nr; j++){
            if(sum->rec[i].id == term->rec[j].id){
                sum->rec[i].name = term->rec[j].name;
                sum->rec[i].val += term->rec[j].val;
                break;
            }
        }
    }
    sum->ntime += term->ntime;
}

void divide_rsf(struct read_sformat *num, int64_t den){
    for(int i=0; i<num->nr; i++)
        num->rec[i].fval = (double) num->rec[i].val / den;
    num->fntime = (double) num->ntime / den;
    num->divd = 1;
}

void check_event(int idx){
    int ret;
    pfm_event_info_t info;
    memset(&info, 0, sizeof(info));
    info.size = sizeof(info);

    
    /**
     ** ######################################################################
     ** Call the get_event_info routine to fill info, and read its content
     **/
    ret = pfm_get_event_info(idx, PFM_OS_NONE, &info);
    if (ret != PFM_SUCCESS){
        printf("Error: Couldn't find event's info: ret=%d\n", ret);
        exit(1);
    }
    char *pr;
    pr = info.is_precise ? "TRUE" : "FALSE";
    printf("Event       : %s\n"
           "Description : %s\n"
           "Equivalent  : %s\n"
           "Code        : %lxh\n"
           "IDX         : %d\n"
           "Attributes  : %d\n"
           "Size        : %lu\n"
           "Precise?    : %s\n\n",
           info.name,
           info.desc,
           info.equiv,
           info.code,
           info.idx,
           info.nattrs,
           info.size,
           pr);
    
    /**
     ** ######################################################################
     ** Retreiving event's attributes
     **/

    printf("  # |       Attrib Name |   Equiv |     Ctrl | Size | D | P |"
           "   Code |     Str |   (type) Val | Desc\n"
           "----+-------------------+---------+----------+------+---+---+"
           "--------+---------+--------------+-----------------------\n");
    pfm_event_attr_info_t ainfo;
    char *type, *dfl, v[27], *aval=v, *ctrl;
    for(int attr=0; attr<info.nattrs; attr++){
        memset(&ainfo, 0, sizeof(ainfo));
        ainfo.size = sizeof(ainfo);
            
        ret = pfm_get_event_attr_info(idx, attr, PFM_OS_NONE, &ainfo);
        if (ret != PFM_SUCCESS){
            printf("Error: Couldn't find event attribute's info: ret=%d\n",
                   ret);
            exit(1);
        }
        
        dfl = ainfo.is_dfl ? "1" : "0";
        pr = ainfo.is_precise ? "1" : "0";
        switch(ainfo.type){
        case PFM_ATTR_UMASK:
            type = "(uMask)";
            snprintf(aval, 26, "%lu", ainfo.dfl_val64);
            break;
        case PFM_ATTR_MOD_BOOL:
            type = "(mod bool)";
            aval = ainfo.dfl_bool ? "1" : "0";
            break;
        case PFM_ATTR_MOD_INTEGER:
            type = "(mod int)";
            snprintf(aval, 26, "%d", ainfo.dfl_int);
            break;
        default:
            type = "invalid";
            aval = "invalid";
        }
        switch(ainfo.ctrl){
        case PFM_ATTR_CTRL_UNKNOWN:
            ctrl = "unknown";
            break;
        case PFM_ATTR_CTRL_PMU:
            ctrl = "PMU hw";
            break;
        case PFM_ATTR_CTRL_PERF_EVENT:
            ctrl = "Perf evt";
            break;
        default:
            ctrl = "invalid";
        }

        printf("%3d | %17s | %7s | %8s | %4lu | %1s | %1s | %5lxh | %7s | "
               "%10s%2s | %s\n",
               attr, ainfo.name, ainfo.equiv, ctrl, ainfo.size, dfl, pr,
               ainfo.code, ainfo.dfl_str, type, aval, ainfo.desc);
        printf("\n");
    }
}
