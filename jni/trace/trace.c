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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

//#include <cutils/properties.h>
#include <sys/system_properties.h>

//#include "bst_fusion.h"
//#include "TargetPlatform.h"
//#include "BsxLibraryDataTypes.h"
//#include "BsxFusionLibrary.h"
#include "trace.h"
//#include "version.h"

uint8_t debug_log_level = LOG_LEVEL_D;

#ifdef __LOG_TO_FILE__
static int m_fd_trace = -1;
#endif

#ifdef __LOG_RAW_DATA__
static int m_fd_raw_data = -1;
#endif

/*!
 * @brief write version number to target trace file
 *
 * @param[i] fd file descriptor
 *
 * @return none
 */
static void trace_write_version(int fd)
{
	char buf[128];

	sprintf(buf, "========\nBIC technolodgy\n");
//		get_version());
	write(fd, buf, strlen(buf));
//	sprintf(buf, "build date: %s\n", get_build_date());
	write(fd, buf, strlen(buf));
	sprintf(buf, "ev version:%d.%d.%d.%d\n========\n\n", 0,
				1,
				0,
				0);
	write(fd, buf, strlen(buf));
}

int trace_open_file(char *p_path)
{
	int fd;

	fd = open(p_path, O_CREAT | O_TRUNC | O_WRONLY | O_SYNC,
			S_IRUSR | S_IWUSR |
			S_IRGRP | S_IWGRP |
			S_IROTH | S_IWOTH);
	if (fd != -1)
	{
		trace_write_version(fd);
	}
	return fd;
}

/*!
 * @brief This function initialize debug log and raw data log file handles for
 * further use
 *
 * @param none
 *
 * @return zero success, non-zero failed
 */
int trace_init(void)
{

	return 0;
}

/*!
 * @brief This function de-initialize debug log and raw data log file handles
 *
 * @param none
 *
 * @return none
 */
void trace_deinit(void)
{
#ifdef __LOG_TO_FILE__
	if (m_fd_trace != -1)
	{
		close(m_fd_trace);
		m_fd_trace = -1;
	}
#endif
#ifdef __LOG_RAW_DATA__
	if (m_fd_raw_data != -1)
	{
		close(m_fd_raw_data);
		m_fd_raw_data = -1;
	}
#endif
}

#ifdef __LOG_TO_FILE__
/*
 * @brief Out put debug information to trace file according with 'fmt'
 *
 * @param[i] fmt format string
 *
 * @return none
 */
void trace_log(const char *fmt, ...)
{
	int ret;
	va_list ap;
	char buf[512];

	if (m_fd_trace < 0)
	{
		return;
	}
	va_start(ap, fmt);
	ret = vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	if (ret > 0) {
		write(m_fd_trace, buf, ret);
	}
}
#endif


