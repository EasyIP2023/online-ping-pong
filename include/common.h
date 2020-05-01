#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <log.h>
#include <stdint.h>
#include <errno.h>

#define FREE(ptr) { free(ptr); ptr = NULL; }

#define ERR64 -1
#define CLIENT_TERMINATE -999
#define SERVER_TERMINATE -998

/*
 * http://efesx.com/2010/07/17/variadic-macro-to-count-number-of-arguments/
 * https://stackoverflow.com/questions/23235910/variadic-unused-function-macro
 */
#define UNUSED1(z) (void)(z)
#define UNUSED2(y,z) UNUSED1(y),UNUSED1(z)
#define UNUSED3(x,y,z) UNUSED1(x),UNUSED2(y,z)
#define UNUSED4(b,x,y,z) UNUSED2(b,x),UNUSED2(y,z)
#define UNUSED5(a,b,x,y,z) UNUSED2(a,b),UNUSED3(x,y,z)
#define UNUSED6(a,b,c,x,y,z) UNUSED3(a,b,c),UNUSED3(x,y,z)
#define UNUSED7(a,b,c,d,x,y,z) UNUSED4(a,b,c,d),UNUSED3(x,y,z)
#define UNUSED8(a,b,c,d,w,x,y,z) UNUSED4(a,b,c,d),UNUSED4(w,x,y,z)

#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8, N,...) N
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)

#define ALL_UNUSED_IMPL_(nargs) UNUSED ## nargs
#define ALL_UNUSED_IMPL(nargs) ALL_UNUSED_IMPL_(nargs)
#define ALL_UNUSED(...) ALL_UNUSED_IMPL(VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#endif
