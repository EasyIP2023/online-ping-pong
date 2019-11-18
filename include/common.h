
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <log.h>

#define FREE(ptr) if (ptr) { free(ptr); ptr = NULL; }

#endif
