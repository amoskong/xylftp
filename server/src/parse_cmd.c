/*
 * Copyright (c) 2007,西安邮电学院Linux兴趣小组
 * All rights reserved.
 *
 * 文件名称：parse_cmd.c
 * 摘    要：从一行数据中解析出命令和参数 并调用相应的命令处理模块 
 * 对读取的字符串做了初步分析 提高了容错能力 对参数的接收做了限制，一次只接收一个参数。
 * 重写了接收参数模块,去掉了无用参数buf_size,_line_cmd()的返回值改成了结构体指针。
 * 当前版本：4.0
 * 作    者：贾孟树
 * 完成日期：2007年6月13日
 * 取代版本：3.0
 * 完成日期：2007年6月8日
 * 修改者：王聪
 * 修改日期：2007年6月15日
 */


#include "xylftp.h"
#include "do_cmd.h"
#include "debug.h"

#define MAX_CMD 5
#define MAX_ARG 4096

extern struct user_env user_env;
extern struct run_env run_env;
struct parse_cmd{
	char cmd[MAX_CMD];
	char arg[MAX_ARG];
};


const char *commands[] = {"USER","PASS","SYST","QUIT","RETR","STOR","RNFR","RNTO","ABOR","DELE",
		"RMD","MKD","PWD","CWD","CDUP","PORT","NOOP","PASV","TYPE","MODE",
		"STAT","STRU","LIST"};			/*服务器支持的所有命令*/

static int _line_cmd(char *line_buf, struct parse_cmd *user_cmd)
{
	int	i = 0;
	int	j = 0;
	int	k = 0;

	while (!isalpha(line_buf[i])) {
		i++;							
	} 		/*<--略过不是字母的字符*/		 		
	while (line_buf[i] != ' '
			&& line_buf[i] != '\r'
			&& line_buf[i] != '\n') {
		if (k < MAX_CMD-1) {
			user_cmd->cmd[k++] = line_buf[i];
			i++;
		}
		else {
			return -1;
		}
	}		/*<--接收以字母开头的字符串（命令）*/
	while (line_buf[i] == ' ') {		
		i++;	
	} 		/*<--清除命令与参数之间的空格*/
	while (line_buf[i] != '\0'
			&& line_buf[i] != '\r'
			&& line_buf[i] != '\n') {
		user_cmd->arg[j++] = line_buf[i];
		i++;
	}		/*<--只提取第一个参数*/
	
	for (;k >= 0;k--) {		/*检查命令中字母的大小写，若是小写字母，则转化为大写字母*/
		user_cmd->cmd[k] = toupper(user_cmd->cmd[k]);
	}
#ifdef DEBUG					 
	printf("\n");
	printf("CMD:");
	printf("%s",user_cmd->cmd);
	printf("\n");
	printf("ARG:");
	printf("%s",user_cmd->arg);
	printf("\n");
#endif
	return 0;
}

static int _cmd_num(struct parse_cmd cmd)
{
	int i;
	for (i = 0; i < (int)(sizeof(commands)/sizeof(commands[0])); i++) {
		if (strcmp(cmd.cmd, commands[i]) == 0) {
			return i+1;
		}		
	}
	return 0;
}


int parse_cmd(char *p_buf)
{	
	char rnfr_arg[MAX_ARG];
	struct parse_cmd user_cmd;
	const char login_error[] = "530 Please login with USER and PASS.\r\n";
	int i = 0;

	memset(&user_cmd, 0, sizeof(struct parse_cmd));
	if (_line_cmd(p_buf, &user_cmd) == -1) {
		failed(user_cmd.cmd);
		return 2;
	}
	if (( i = _cmd_num(user_cmd)) <= 4 || user_env.login_in == TRUE) {
		switch (i) {
		case 1:
				debug_printf("****call user()\n");
				do_user(user_cmd.arg);
				break;
		case 2:
				debug_printf("call pass()\n");
				printf("username=%s\n", user_env.user_name);

				if (strlen(user_env.user_name) != 0) { 
					if (do_pass(user_cmd.arg) == 0) {
						if (chroot(run_env.ftp_dir) < 0) {
							write_log("chroot error", 0);
							do_quit();
							break;	
						}
						if (chdir("/") < 0) {
							write_log("chdir error", 0);	
							do_quit();
							break;
						}
					} else {
						break;
					}
				} else {
					const char mess[] = "220 No username input.\r\n";
					write(user_env.connect_fd, mess, strlen(mess));
				}

				break;
		case 3:
				debug_printf("call syst()\n");
				do_syst();
				break;
		case 4:
				debug_printf("call quit()\n");
				do_quit();
				break;	
		case 5:
				debug_printf("call retr()\n");
				do_retr(user_cmd.arg);
				break;
		case 6:
				debug_printf("call stor()\n");
				do_stor(user_cmd.arg);
				break;
		case 7:
				debug_printf("call rnfr()\n");
				do_rnfr();
				strcpy(rnfr_arg, user_cmd.arg);
				break;
		case 8:
				debug_printf("call rnto()\n");
				do_rnto(rnfr_arg, user_cmd.arg);
				memset(rnfr_arg, 0, MAX_ARG);
				break;
		case 9:
				debug_printf("call abor()\n");
				fflush(stdout);
				do_abor(user_cmd.arg);
				break;
		case 10:
				debug_printf("call dele()\n");
				do_dele(user_cmd.arg);
				break;
		case 11:
				debug_printf("call rmd()\n");
				do_rmd(user_cmd.arg);
				break;
		case 12:
				debug_printf("call mkd()\n");
				do_mkd(user_cmd.arg);
				break;
		case 13:
				debug_printf("call pwd()\n");
				do_pwd();
				break;
		case 14:
				debug_printf("call cwd()\n");
				do_cwd(user_cmd.arg);
				break;
		case 15:
				debug_printf("call cdup()\n");
				do_cdup();
				break;
		case 16:
				debug_printf("call port()\n");
				do_port(user_cmd.arg);
				break;	
		case 17:
				debug_printf("call noop()\n");
				do_noop();	
				break;
		case 18:
				debug_printf("call pasv()\n\n");
				do_pasv();
				break;
		case 19:
				debug_printf("call type()\n");
				do_type(user_cmd.arg);
				break;
		case 20:
				debug_printf("call mode()\n");
				do_mode(user_cmd.arg);
				break;
		case 21:
				debug_printf("call stat()\n");
				do_stat(user_cmd.arg);
				break;
		case 22:
				debug_printf("call stru()\n");
				do_stru(user_cmd.arg);
				break;
		case 23:
				debug_printf("call list()\n");
				do_list(user_cmd.arg);
				break;
		default:
				debug_printf("call failed()\n");
				failed(user_cmd.cmd);
				break;
		}		
	} else if (i <= 23 && i >= 1) {
		write(user_env.connect_fd, login_error, strlen(login_error));
	} else {
		failed(user_cmd.cmd);
	}
	return 1;
}
