/*
 *Copyright (c) 2007,西安邮电学院Linxu兴趣小组
 * All rights reserved.
 *
 * 文件名称：write_log.c
 * 摘    要：写日志
 * 当前版本：1.0
 * 作    者：董溥
 * 完成日期：2007年5月17日
 */

#include "xylftp.h"

extern struct user_env user_env;

int write_log(char *message, int level)
{
	const char *levels;
	int  option;
	option = LOG_PID;
	if (level == 1) {
		levels = "XYLftp[NOTICE]";
	}
	else if (level == 0) {
		levels = "XYLftp[ERROR]";
	}
	else {
		write_log("wrong level", 0);
		return -1;
	}
	#ifdef DEBUG
	option = LOG_PERROR;
	#endif	
	openlog(levels,option, LOG_INFO);
	syslog(LOG_INFO|LOG_LOCAL1,"From IP:%s Port:%d User:%s [%s]",
		user_env.client_ip, user_env.client_port,
		user_env.user_name, message);
	closelog();

	return 0;
}
