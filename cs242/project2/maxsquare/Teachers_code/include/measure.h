/**
 ** Simplified interface to linux perf.
 ** This simple API uses the library libpfm4 which should be installed in
 ** your system. See http://perfmon2.sourceforge.net/docs_v4.html
 ** It specifically uses pfm_get_os_event_encoding(), perf_event_open() and
 ** ioctl() to setup and measure performance counters specified by their 
 ** string representation or, their raw code (found in the processor's manual).
 **/

#ifndef PERF_H_
#define PERF_H_

#include <stddef.h>
#include <inttypes.h>

/**
 ** PROCESSOR SPECIFIC
 ** Raw events as presented in  "Processor Programming Reference (PPR) for
 ** AMD Family 17h Model 71h, Revision B0 Processors (PUB)" with their 
 ** codification given by libpfm4.
 **
 ** Raw codes:  0x CC BB AA
 **                ││ ││ ││
 **                ││ ││ └┴── Event mask.
 **                ││ └┴──── Micro mask code.
 **                └┴────── Modifiers and reserved bits.
 **/

// L1 data miss results in L2 miss.
#define NAM_CORE_TO_L2M "amd64_fam17h_zen2::CORE_TO_L2_CACHEABLE_REQUEST_ACCESS_STATUS:LS_RD_BLK_C:u"
#define RAW_CORE_TO_L2M 0x510864

// L1 data miss results in L2 hit.
#define NAM_CORE_TO_L2H "amd64_fam17h_zen2::CORE_TO_L2_CACHEABLE_REQUEST_ACCESS_STATUS:LS_RD_BLK_L_HIT_X:LS_RD_BLK_L_HIT_S:LS_RD_BLK_X:u"
#define RAW_CORE_TO_L2H 0x517064

// requests to l2, to change a line to writable.
#define NAM_L2CHG_X "amd64_fam17h_zen2::REQUESTS_TO_L2_GROUP1:CHANGE_TO_X:u"
#define RAW_L2CHG_X 0x510860

// retired instrucctions
#define NAM_RET_INS "amd64_fam17h_zen2::RETIRED_INSTRUCTIONS:u"
#define RAW_RET_INS 0x5100c0

// L3 access and misses (no names). aparently not supported by libpfm4
#define RAW_L3A 0xFF0F00000040FF04
#define RAW_L3M 0xFF0F000000400104

/**
 ** Maximum number of events to measure at the same time. You could increase
 ** it, but there is a small number of physical counters, beyond that, they
 ** get multiplexed, and become imprecise.
 **/
#define MAX_EVENTS 6

/**
 ** Struct used to read out the (potentially many) events from the group.
 ** @nr   : Number of records.
 ** @rec  : Array of records. each record represents a performance counter.
 **     @val  : Value of the performance counter.
 **     @id   : Unique counter's id assigned at runtime
 **     @name : Fully qualified name of the counter (given by libpfm4)
 ** @ntime: Time in nanoseconds taken in the measured task.
 **/
struct read_sformat{
    uint64_t nr;
    struct {
        union{
            uint64_t val;
            double fval;
        };
        uint64_t id;
        char *name;
    }rec[MAX_EVENTS];
    union{
        uint64_t ntime;
        double fntime;
    };
    char divd;
};

/**
 ** Returns the current time in nanoseconds.
 **/
uint64_t nano_stopwatch(void);

/**
 ** Setup the performance counters to measure.
 ** @sevt : Array of performance counters' names (see libpfm4 manual).
 **         If NULL, the default list is used and @sl is ignored.
 ** @sl   : Lenght of @sevt.
 ** @devt : Array of performance counters' raw numbers (see your processor's manual).
 **         If null, the default list is used and @dl is ignored.
 ** @dl   : Lenght of @devt.
 **/
extern void msr_init(char **sevt, int sl, uint64_t *devt, int dl);

/**
 ** Start and stop measuring the performance counters and execution time.
 ** Order of usage:
 **   msr_setup()
 **   msr_record()
 **   (run the task you are measuring)
 **   msr_pause()
 **   (commands not measured)
 **   msr_record()
 **   (run another task)
 **   msr_pause()
 **/
extern void msr_record();
extern void msr_pause();

/**
 ** msr_get()
 ** Gets data into a read_sformat structure. Sometimes needed to do exotic
 ** calculations.
 ** msr_print_results()
 ** Most commonly you just want to print the internally stored results.
 ** If @to_print is not NULL, then print @to_print instead.
 ** verbosity = {0, 1, 2}
 **/
extern struct read_sformat msr_get();
extern void msr_print_results(struct read_sformat *to_print, int verb);

/**
 ** Operations on struct read_sformat
 ** accumulate : sum = sum + term.
 ** divide     : num = num / den. Useful to compute avearge.
 **/
extern void  accumulate_rsf(struct read_sformat *sum,
                            struct read_sformat *term);
extern void divide_rsf(struct read_sformat *num, int64_t den);

/**
 ** Check the available attributes of an event given its IDX.
 ** You can get the IDX with the example codes available in libpfm4.
 **/
extern void check_event(int idx);
#endif
