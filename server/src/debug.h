#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef DEBUG

	#define debug_printf(fmt, ...) \
		printf("DEBUG: %s:%d@%s: " fmt,\
            __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

	#define LOG_IT(msg) fprintf(stderr, "LOG: " msg "\n")

#else /* !DEBUG */

	#define debug_printf(fmt, ...) do{} while(0)

	#define LOG_IT(msg) write_log(msg, 0)

#endif /* DEBUG */

#endif /* DEBUG_H */
