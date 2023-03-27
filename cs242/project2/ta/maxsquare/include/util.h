/**
 ** Utilities library
 **/

#ifndef UTIL_H_
#define UTIL_H_

#include <stddef.h>
#include <inttypes.h>

struct arg{
    long long v; //value
    char s;      //is set by user?
    char m;      //is mandatory?
    char *n;     //name
    char *d;     //description
};

/**
 ** @brief print error and exit if code != 0
 **/
extern void error(char *msg, int code);

/**
 ** Print a number @in in its binary form, surrounded by @pre, and @post
 **/
extern void print_bin(char *pre, uint64_t in, char *post);

/**
 ** Get arguments from @argv into @out. If error, print help.
 **
 ** @title : title of the program.
 ** @argv  : array with the input arguments (including binary name)
 ** @argc  : size of @argv
 ** @par   : array with the parameters correctly formated.
 ** @par_l : size of @par
 **/
extern void getargs(char *title, char **argv, int argc, struct arg *par, int par_l);

/**
 ** Given a (big) value @val, place in @c its representation as:
 ** encode2  -> 1.xxx {B,K,M,G}
 ** encode10 -> 1.xxx  {,k,m,g}
 ** @cn is the size of buffer @c.
 **/
void encode2(size_t val, char *c, size_t cn);
void encode10(size_t val, char *c, size_t cn);
#endif
