#include "util.h" //implements
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

void error(char *msg, int code){
    fprintf(stderr, "%s\n", msg);
    if(code != 0)
        exit(code);
}

void print_bin(char *pre, uint64_t in, char *post){
    int i, j, str_size = 64 + 3*3 + 4*3 + 1; //bits + spaces + \0
    char str[str_size];
    memset(str, 0, str_size);
    for(i=j=0; i<64; i++){
        if(1ULL<<i & in) str[j] = '1';
        else str[j] = '_';
        if((i+1)%4 == 0 && i+1 < 64){
            j++;
            if((i+1)%16 == 0){
                str[j] = ' '; j++;
                str[j] = ' '; j++;
                str[j] = ' ';
            }else str[j] = ' ';
        }
        j++;
    }
    printf("%s", pre);
    for(i=str_size-1; i>=0; i--) printf("%c",str[i]);
    printf("%s", post);
}

static void print_help(char *bin, char *title, struct arg *opt, int optl, int code){
    int i;
    char m1, m2;
    char def[64];
    printf("%s.\n\n", title);
    printf("USAGE:\n %s", bin);
    for(i=0; i<optl; i++){
        if(opt[i].m)
            m1 = '<', m2 = '>';
        else
            m1 = '[', m2 = ']';
        printf(" %c%s=%c0%c", m1, opt[i].n, *opt[i].n, m2);
    }
    printf("\n\nOPTIONS:\n");
    for(i=0; i<optl; i++){
        if(!opt[i].m)
            snprintf(def, 64, " (default: %lld)", opt[i].v);
        else
            def[0] = '\0';
        printf("%9s : %s.%s\n", opt[i].n, opt[i].d, def);
    }
    printf("\n");
    exit(code);
}

static size_t Giga=1024*1024*1024;
static size_t Mega=1024*1024;
static size_t Kilo=1024;
static size_t giga=1000*1000*1000;
static size_t mega=1000*1000;
static size_t kilo=1000;

void encode2(size_t val, char *c, size_t cn){
    if(Giga <= val)
        snprintf(c, cn, "%.1f%c", (double)val/Giga, 'G');
    else if(Mega <= val)
        snprintf(c, cn, "%.1f%c", (double)val/Mega, 'M');    
    else if(Kilo <= val)
        snprintf(c, cn, "%.1f%c", (double)val/Kilo, 'K');
    else
        snprintf(c, cn, "%.1f%c", (double)val, 'B');
}

void encode10(size_t val, char *c, size_t cn){
    if(giga <= val)
        snprintf(c, cn, "%.1f%c", (double)val/giga, 'g');
    else if(mega <= val)
        snprintf(c, cn, "%.1f%c", (double)val/mega, 'm');    
    else if(kilo <= val)
        snprintf(c, cn, "%.1f%c", (double)val/kilo, 'k');
    else
        snprintf(c, cn, "%.1f", (double)val);
}

static long long decode(char *f){
    long long val = strtol(f, &f, 10);
    int mult = 1;
    switch(*f){
    case 'B':
        mult = 1; break;
    case 'K':
        mult = Kilo; break;
    case 'M':
        mult = Mega; break;
    case 'G':
        mult = Giga; break;
    case 'k':
        mult = kilo; break;
    case 'm':
        mult = mega; break;
    case 'g':
        mult = giga; break;
    }
    return val*mult;
}

void getargs(char *title, char **argv, int argc, struct arg *par, int par_l){
    // count mandatory parameters
    int i, man = 0;
    for(i=0; i<par_l; i++)
        if(par[i].m)
            man += 1;
    
    // get binary name and advance array
    char *bin = argv[0];
    argv = argv+1;
    argc -= 1;
    
    // asking for help?
    if(1 <= argc && (!memcmp(argv[0],"-h\0",3) || !memcmp(argv[0],"--help\0",7)))
        print_help(bin, title, par, par_l, 0);

    if(argc < man || par_l < argc){
        printf("Error: Required arguments no specified.\n");
        print_help(bin, title, par, par_l, -1);
    }

    int j, arg_len, left_len, cmp;
    char *eq, set = 0;
    for(i=0; i<argc; i++){
        // finding a pointer to '=', and the lenght of the left side
        // of the argument (left_len)
        eq = strchr(argv[i], '=');
        arg_len = strlen(argv[i]);
        if(eq){
            left_len = eq - argv[i];
            if(left_len < 1 || arg_len < left_len + 2){
                printf("Error: The sign '=' needs arg_name and value. Ex: arg=val\n");
                print_help(bin, title, par, par_l, -2);
            }
        }else
            left_len = arg_len;

        // storing the value in the correct option structure
        for(j=0; j<par_l; j++){
            cmp = memcmp(argv[i], par[j].n, left_len);
            if(!cmp){
                par[j].v = eq ? decode(argv[i]+left_len+1) : 1;
                par[j].s = 1;
                set = 1;
                break;
            }
        }

        // if unknown parameter, warn user
        if(!set){
            printf("Error: Unknown option %s found.\n", argv[i]);
            print_help(bin, title, par, par_l, -3);
        }
        set = 0;
    }

    // if a required arg was not setted by the user, error.
    for(i=0; i<par_l; i++){
        if(par[i].m && !par[i].s){
            printf("Error: Required argument '%s' not specified\n", par[i].n);
            print_help(bin, title, par, par_l, -4);
        }
    }
}
