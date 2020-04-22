#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#include "log.h"
#include "cstl_lib.h"

int32_t g_log_prio = LOG_DEFAULT_PRIO;

static int32_t log_inited = 0;
static log_map_t log_map;

static double log_version = 0.3;

static LOG_PRIO_MESSAGE_T log_prio_message[] = {
	LOG_ERR,		"ERROR",
	LOG_WARNING,	"WARNING",
	LOG_NOTICE,		"NOTICE",
	LOG_INFO,		"INFO",
	LOG_DEBUG,		"DEBUG",
	-1,				NULL,
};

static void _log_error(int32_t prio, const int8_t *fmt, ...)
{
	if (prio < g_log_prio) {
		int8_t str[LOG_BUF_SIZE] = {0};
		va_list arg_ptr;
		va_start(arg_ptr, fmt);
		vsprintf(str, fmt, arg_ptr);
		va_end(arg_ptr);

		print_to_syslog(prio, str);
		
		fprintf(stderr, "%s", str);
	}
}

static void get_prio_message(int32_t prio, int8_t **message)
{
	int32_t i = 0;
	if (!message)
		return ;
	while (log_prio_message[i].prio != -1) {
		if (log_prio_message[i].prio == prio) {
			*message = log_prio_message[i].message;
			return ;
		}
		i++;
	}
	*message = NULL;
}

static int32_t log_file_compare(void* left, void* right ) 
{
    return strcmp((const int8_t *)left, (const int8_t *) right);
}
static void log_file_close(void *fp)
{
	if (fp) {
		//printf("file = %x\n", *(FILE **)(((struct cstl_object *)fp)->data));
		fclose(*(FILE **)(((struct cstl_object *)fp)->data));
	}
}

int32_t log_init(int32_t log_prio)
{
	if (log_inited) {
		_log_error(LOG_INFO, "%s: log already init\n", __func__);
		return 0;
	}
	
	if (pthread_mutex_init(&log_map.mutex, NULL) != 0) 
		return -1;
	
	log_map.log_file_map = new_cstl_map(log_file_compare, NULL, log_file_close);
	if (!log_map.log_file_map) {
		_log_error(LOG_ERR, "%s: failed to init log", __func__);
		return -1;
	}

	g_log_prio = log_prio;
	log_inited = 1;
	
	return 0;
}

void log_uninit()
{
	if (log_inited) {
		delete_cstl_map(log_map.log_file_map);
		log_inited = 0;
	}
}

void _log(int32_t prio, const int8_t *file, const int8_t *str)
{
	int32_t ret = 0;
	
	print_to_syslog(prio, str);

	// get now time to buf
	time_t rawtime;
	struct tm *info;
	struct tm *now_time = (struct tm *)malloc(sizeof(struct tm));
	int8_t time_buf[22];
	time(&rawtime);
	if (!now_time) {
		info = localtime(&rawtime);
		strftime(time_buf, 30, LOG_DEFAULT_DATE_FORMAT, info);
	} else {
		now_time = localtime_r(&rawtime, now_time);
		strftime(time_buf, 30, LOG_DEFAULT_DATE_FORMAT, now_time);
		free(now_time);
	}

	int8_t *prio_buf = NULL;
	get_prio_message(prio, &prio_buf);

	// print log
	if (!file) // print to stdout
		fprintf(stdout, "[%s] %s %s", time_buf, prio_buf, str);
	else { // print to file
		if (!log_inited) { // init log module first
			ret = log_init(g_log_prio);
			if (ret != 0) {
				_log_error(LOG_ERR, "%s: failed to exec log_init()\n", __func__);
				return ;
			}
		}
		
		void *value;
		FILE *fp;
		int32_t need_open = 1;
		if (pthread_mutex_lock(&log_map.mutex) != 0) {
			_log_error(LOG_ERR, "%s: failed to lock the map mutex\n", __func__);
			return ;
		}		
		ret = find_cstl_map(log_map.log_file_map, file, &value);
		pthread_mutex_unlock(&log_map.mutex);
		if(ret != 0) { // file already open
			if (!access(file, F_OK)) {
				fp = *(FILE **)value;
				fprintf(fp, "[%s] %s %s", time_buf, prio_buf, str);

				need_open = 0;
				
			} else {
				if (pthread_mutex_lock(&log_map.mutex) != 0) {
					_log_error(LOG_ERR, "%s: failed to lock the map mutex\n", __func__);
					return ;
				}	
				remove_cstl_map(log_map.log_file_map, file);
				pthread_mutex_unlock(&log_map.mutex);
			}
			
			free(value);
		} 
		
		if (need_open == 1) { // open the file and store FILE's pointer
			fp = fopen(file, "a+");
			if (!fp) {
				_log_error(LOG_ERR, "%s: failed to open %s\n", __func__, file);
				return ;
			}
			
			if (pthread_mutex_lock(&log_map.mutex) != 0) {
				_log_error(LOG_ERR, "%s: failed to lock the map mutex\n", __func__);
				return ;
			}	
			insert_cstl_map(log_map.log_file_map, file, (strlen(file) + 1), &fp, sizeof(FILE *));
			pthread_mutex_unlock(&log_map.mutex);
			
			fprintf(fp, "[%s] %s %s", time_buf, prio_buf, str);
		}	

		fflush(fp);
#if REALTIME_TO_DISK
		fsync(fileno(fp));
#endif
	}
}

double get_log_version()
{
	return log_version;
}



