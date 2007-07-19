/*
 * Copyright (c) 2001,西安邮电学院Linux兴趣小组
 * All rights reserved.
 * 
 * 文件名称：do_mode.h
 * 摘    要：实现MODE命令的测试文件。
 * 
 * 当前版本：1.0
 * 作    者：刘洋
 * 完成日期：2007年6月13日
 */

#include "../src/xylftp.h"

#ifndef TEST
#define TEST
#endif

#ifdef TEST
struct user_env user_env;
#else
extern struct user_env user_env;
#endif

int do_mode(const char *arg)
{
	char *succ = "200 MODE S OK.\r\n";
	char *fail = "504 Command not implemented for that parameter.\r\n";
	if ((strlen(arg) == 1) && ((*arg == 's') || (*arg == 'S'))) {
		write(user_env.connect_fd,succ,strlen(succ));
		return 0;
	}
	else {
		write(user_env.connect_fd,fail,strlen(fail));
		return 1;
	}
}

int main(int argc,char **argv)
{
	char *arg = "";
	if (argc > 1) {
		arg = argv[1];
	}
	user_env.connect_fd = 0;
	return do_mode(arg);
}
