/*
 *Copyright (c) 2007,西安邮电学院Linux兴趣小组
 * All rights reserved.
 *
 * 文件名称：main.c
 * 摘    要：主函数，以及守护进程建立，建立监听
 * 当前版本：1.0
 * 作    者：董溥
 * 完成日期：2007年5月12日
 *
 *修改人员：林峰
 *最后修改：2007年6月6日
 *
 *修改人员：王聪
 *最后修改：2007年6月13日
 */


#include "xylftp.h"

struct run_env run_env;
struct user_env user_env;

extern int r_close(int);
static void _init_env(void);
static void _daemon_init(void);


int main(void)
{
	if (read_configure() == -1) {
		exit (1);
	}
	#ifndef DEBUG
	_daemon_init();
	#endif
	_init_env();
	listen_connect();
	return 0;	
}


static void _init_env(void)
{
	user_env.login_in = FALSE;
	strcpy(user_env.user_name, "");
	user_env.client_data_port = 0;
	user_env.client_port = 0;
	user_env.login_time = 0;
	user_env.last_operation_time = 0;
	strcpy(user_env.current_path, "/");
#ifdef DEBUG
	user_env.enable_upload = TRUE;
#else
	user_env.enable_upload = FALSE;
#endif
	user_env.passive_on = FALSE;
	user_env.ascii_on = FALSE;
}

static void _daemon_init(void) 
{
	int i;
	pid_t pid;
	int tmp;
	if ((pid = fork()) == -1) {
		perror("fork error");
		write_log("fork error", 0);
		exit(errno);
	}
	else if (pid) {
        	exit(0);
	}
	if (setsid() == -1) {
		perror("setsid error");
		write_log("setsid error", 0);
		exit(errno);
	}
	signal(SIGHUP,SIG_IGN); /*ignore signals*/

	if ((pid = fork()) == -1) {
		perror("fork error");
		write_log("fork error", 0);
		exit(errno);
	}
	else if (pid) {
		exit(0);
	}
	if (chroot("/") == -1) {
		perror("chroot error");
		write_log("chroot error", 0);
		exit(errno);
	}
	if ((tmp = chdir(run_env.ftp_dir)) == -1) {
		perror("chdir error");
		write_log("chdir error", 0);
		exit(errno);
	}
	umask(0);
	for (i = 0;i < MAXFD;i++) {
		r_close(i);
	}
}


void free_sources(void)
{
	free(run_env.user_pass_file);
	free(run_env.ftpd_banner);
	free(run_env.log_file);
}
