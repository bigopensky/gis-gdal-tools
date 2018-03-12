/* Last modified Time-stamp: <Ye Wenbin 2007-12-30 16:29:58>
 * @(#)error.c
 */

#include "ifgdv/error.h"

// ---------------------------------------------------------------
void gistk_error_fatal(int code, const char *message, ...) {
    va_list arglist;
    va_start(arglist,message);
    vfprintf(stderr, message, arglist );
    va_end(arglist);
    exit(code);
}

