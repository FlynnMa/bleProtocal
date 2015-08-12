/*!
 * @section LICENSE
 * $license$
 *
 * @file         $filename$
 * @date         $date$
 * @commit       $id$
 *
 * @brief
 *
 * @detail
 *
 */

#ifndef __SENSOR_TRACE_H
#define __SENSOR_TRACE_H
#include <stdint.h>

extern uint8_t debug_log_level;

#define LOG_LEVEL_D 1
#define LOG_LEVEL_I 2
#define LOG_LEVEL_W 3
#define LOG_LEVEL_E 4
#define LOG_LEVEL_F 5
#define LOG_LEVEL_S 6

#ifndef CFG_LOG_LEVEL
#define CFG_LOG_LEVEL LOG_LEVEL_F
#endif

#ifndef __WITH_LOG_FUNCTION__
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#define LOGF(...)
#define LOGE_IF(cond, ...) ( (cond) ? ((void)0) : (void)0 )
#define LOG_RAW_DATA(...)

#else

#ifdef __LOG_TO_LOGCAT__
#include <android/log.h>

#define LOG_TAG    "ElectronicVehicle"

#define LOGD(...)  do\
{\
	if (debug_log_level <= LOG_LEVEL_D)\
		__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__);\
} while (0)

#define LOGI(...)  do\
{\
	if (debug_log_level <= LOG_LEVEL_I)\
		__android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__);\
} while (0)

#define LOGW(...)  do\
{\
	if (debug_log_level <= LOG_LEVEL_W)\
		__android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__);\
} while (0)

#define LOGE(...)  do\
{\
	if (debug_log_level <= LOG_LEVEL_E)\
		__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__);\
} while (0)

#define LOGF(...)  do\
{\
	if (debug_log_level <= LOG_LEVEL_F)\
		__android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__);\
} while (0)

#ifndef LOGE_IF
#define LOGE_IF(cond, ...)  do\
{\
	if ( (cond) && (debug_log_level <= LOG_LEVEL_E))\
		__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__);\
} while (0)
#endif

#define LOG_RAW_DATA(...)  do\
{\
	if (debug_log_level <= LOG_LEVEL_D)\
		__android_log_print(ANDROID_LOG_DEBUG,"bst_rd",__VA_ARGS__);\
} while (0)

#else

#define GET_TIME_TICK() (get_current_timestamp())

#define LOG_FORMAT(fmt,type) "[%10u]%s:%s(%d)" fmt "\n\n", \
			GET_TIME_TICK(),type, __FUNCTION__, __LINE__

#define LOGD(fmt, args...)  do\
{\
	if (debug_log_level <= LOG_LEVEL_D)\
		trace_log(LOG_FORMAT(fmt, "D"), ##args);\
} while (0)

#define LOGI(fmt, args...)  do\
{\
	if (debug_log_level <= LOG_LEVEL_I)\
		trace_log(LOG_FORMAT(fmt, "I"), ##args);\
} while (0)

#define LOGW(fmt, args...)  do\
{\
	if (debug_log_level <= LOG_LEVEL_W)\
		trace_log(LOG_FORMAT(fmt, "W"), ##args);\
} while (0)

#define LOGE(fmt, args...)  do\
{\
	if (debug_log_level <= LOG_LEVEL_E)\
		trace_log(LOG_FORMAT(fmt, "E"), ##args);\
} while (0)

#define LOGF(fmt, args...)  do\
{\
	if (debug_log_level <= LOG_LEVEL_F)\
		trace_log(LOG_FORMAT(fmt, "F"), ##args);\
} while (0)

#ifndef LOGE_IF
#define LOGE_IF(cond, fmt, args...)  do\
{\
	if ( (cond) && (debug_log_level <= LOG_LEVEL_E))\
		trace_log(LOG_FORMAT(fmt, "E"), ##args);\
} while (0)
#endif

#endif
#endif

#define PRINT_LINE  PDEBUG("%s:%d", __FUNCTION__, __LINE__)

#define PFAULT(fmt, args...) LOGF(fmt, ##args)
#define PERR(fmt, args...) LOGE(fmt, ##args)
#define PWARN(fmt, args...) LOGW(fmt, ##args)
#define PINFO(fmt, args...) LOGI(fmt, ##args)
#define PDEBUG(fmt, args...) LOGD(fmt, ##args)

/*!
 * @brief This function initialize debug log and raw data log file handles for
 * further use
 *
 * @param none
 *
 * @return zero success, non-zero failed
 */
int trace_init(void);

/*!
 * @brief This function de-initialize debug log and raw data log file handles
 *
 * @param none
 *
 * @return none
 */
void trace_deinit(void);

#ifdef __LOG_TO_FILE__
/*
 * @brief out put debug log information to trace file according with 'fmt'
 *
 * @param[i] fmt format string
 *
 * @return none
 */
void trace_log(const char *fmt, ...);
#endif

#ifdef __LOG_RAW_DATA__
/*!
 * @brief Out put sensor raw data to sensor raw data file
 *
 * @param[i] pdata   sensor raw data
 *
 * @return none
 */
void trace_raw_data(libraryinput_t *pdata, uint32_t ts);

/*!
 * @brief Out put sensor data from algo to data log file
 *
 * @param[i] pdata   sensor raw data
 *
 * @return none
 */
void trace_output_data(bst_fusion_data_t *pdata, uint32_t ts, uint32_t sensor_handle);
#endif

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __WITH_LOG_FUNCTION__
void trace_reload_debug_log_level();
#else
#define trace_reload_debug_log_level()
#endif
#ifdef __cplusplus
};
#endif

#endif
