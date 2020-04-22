#ifndef __LOG_H_
#define __LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define USING_SYSLOG	0 // 1: use syslog; 0: do not use it (default 0)
#define REALTIME_TO_DISK 1 // 1: every log write to disk immediately; 0: every log jsut wirte to ram (high speed & default 0)

#define LOG_DEFAULT_PRIO	LOG_INFO // default print out level 
#define LOG_BUF_SIZE	1024

#define LOG_DEFAULT_DATE_FORMAT "%Y-%m-%d %H:%M:%S" 

#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>

#if USING_SYSLOG
#include <syslog.h>
#else
typedef enum {
	LOG_ERR,
	LOG_WARNING,
	LOG_NOTICE,
	LOG_INFO,
	LOG_DEBUG
} LOG_PRIO_T;
#endif


typedef struct {
	int32_t prio;
	char *message;
} LOG_PRIO_MESSAGE_T;

typedef struct {
	pthread_mutex_t		mutex;
	struct cstl_map 	*log_file_map;
} log_map_t;


extern int32_t g_log_prio;


#if USING_SYSLOG
#define print_to_syslog(prio, str) do {			\
		syslog(LOG_LOCAL0 | prio, "%s", str);	\
	} while (0)
#else
#define print_to_syslog(prio, str) do {			\
	} while (0)
#endif


#define log(prio, fmt, ...)	do {							\
		if (prio <= g_log_prio) {								\
			char tmp[LOG_BUF_SIZE] = {0};					\
			snprintf(tmp, sizeof(tmp), fmt, ##__VA_ARGS__);	\
			_log(prio, NULL, tmp);							\
		}													\
	} while (0)


#define log_to_file(prio, file, fmt, ...)	do {			\
		if (prio <= g_log_prio) {								\
			char tmp[LOG_BUF_SIZE] = {0};					\
			snprintf(tmp, sizeof(tmp), fmt, ##__VA_ARGS__); \
			_log(prio, file, tmp);							\
		}													\
	} while (0)



extern void _log(int32_t prio, const int8_t *file, const int8_t *str);

/* log_prio means less than or equal to print log level (default LOG_INFO)	
*	if log_prio = LOG_NOTICE, log_to_file and log only exec when the prio is LOG_NOTICE, LOG_WARNING or LOG_ERR
*/
extern int32_t log_init(int32_t log_prio); 

extern void log_uninit();

extern double get_log_version();


#ifdef __cplusplus
}
#endif


#endif
