#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef DEBUG

#define debug_printf(fmt, ...) \
    printf("DEBUG: %s:%d@%s: " fmt,\
            __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#else /* !DEBUG */

#define debug_printf(fmt, ...) do{} while(0)

#endif /* DEBUG */

#endif /* DEBUG_H */
