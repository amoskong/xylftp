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
 */


#include "xylftp.h"
#include "do_cmd.h"

#define MAX_CMD 5
#define MAX_ARG 4096
struct user_env user_env;
struct run_env run_env;

struct parse_cmd{
	char	cmd[MAX_CMD];
	char	arg[MAX_ARG];
} usr_cmd;

char *commands[] = {"USER","PASS","CWD","CDUP","RETR","STOR","RNFR","RNTO","ABOR","DELE",
		"RMD","MKD","PWD","SYST","QUIT","PORT","NOOP","PASV","TYPE","MODE",
		"STAT","STRU","LIST"};			/*服务器支持的所有命令*/

struct parse_cmd* _line_cmd(char *line_buf)
{
	int	i = 0;
	int	j = 0;
	int	k = 0;

	while ((*(line_buf+i) < 65)||((*(line_buf+i) > 90)&&(*(line_buf+i) < 97))||(*(line_buf+i) > 122 )) {
		i++;							
	} 		/*<--略过不是字母的字符*/		 		
	while ((((char)*(line_buf+i)) != ' ')&&((char)(*(line_buf+i)) != '\r')&&((char)(*(line_buf+i)) != '\n')) {
		if (k < (MAX_CMD-1)) {
			usr_cmd.cmd[k++] = (char)(*(line_buf+i));
			i++;
		} else break;
	}		/*<--接收以字母开头的字符串（命令）*/
	while ((char)(*(line_buf+i)) == ' ') {		
		i++;	
	} 		/*<--清除命令与参数之间的空格*/
	while (((char)(*(line_buf+i)) != '\0')&&(((char)*(line_buf+i)) != '\r')&&(((char)*(line_buf+i)) != '\n')&&(((char)*(line_buf+i)) != ' ')) {
		usr_cmd.arg[j++] = (char)(*(line_buf+i));
		i++;
	}		/*<--只提取第一个参数*/
	
	for (;k >= 0;k--) {		/*检查命令中字母的大小写，若是小写字母，则转化为大写字母*/
		if ((usr_cmd.cmd[k] > 96)&&(usr_cmd.cmd[k]) < 123) {
			usr_cmd.cmd[k] =(char)((int)usr_cmd.cmd[k] - 32);
		} else continue;
	}
	#ifdef DEBUG					 
	printf("\n");
	printf("CMD:");
	printf("%s",usr_cmd.cmd);
	printf("\n");
	printf("ARG:");
	printf("%s",usr_cmd.arg);
	printf("\n");
	#endif	
	return &usr_cmd;
}

int _cmd_num(struct parse_cmd *p_cmd)
{
	int i = 0;
	while (i < 23) {
		if (strcmp(p_cmd->cmd,*(commands+(i++))) == 0) {
			return i;
		}		
	}
	return 0;
}


int parse_cmd(char *p_buf)
{
	char	*rnfr_arg = "";
/*
	char	*user_name = "";
	int	banner_lenth = (int)strlen(run_env.ftpd_banner) + 8;
	char	banner_mess[banner_lenth];
*/	
	struct parse_cmd *user_cmd = _line_cmd(p_buf);

	switch (_cmd_num(user_cmd)) {
	case 1: {
			printf("****call user()\n");
			//do_user(user_cmd->arg);
			break;	
		}
			
	case 2: {
			printf("call pass()\n");
			printf("username=%s\n", user_env.user_name);
#if 0
			if (strlen(user_env.user_name) != 0) { 
				if (do_pass(user_cmd->arg) == 0) {
					write(user_env.connect_fd,strcat(strcat(strcat(banner_mess,"230 "), run_env.ftpd_banner),"\r\n"),banner_lenth);
				}
				user_name = "";	
			} else {
				char	*mess = "220 No username input.\r\n";
				write(user_env.connect_fd, mess, strlen(mess));
			}
#endif
			break;	
		}
	case 3: {
			printf("call cwd()");
			printf("\n");
			//do_cwd(user_cmd->arg);
			break;	
		}
	case 4: {
			printf("call cdup()");
			printf("\n");
			//do_cdup();
			break;	
		}
	case 5: {
			printf("call retr()");
			printf("\n");
			//do_retr(user_cmd->arg);
			break;	
		}
	case 6: {
			printf("call stor()\n");
			//do_stor(user_cmd->arg);
			break;	
		}
	case 7: {
			printf("call rnfr()");
			printf("\n");
			//do_rnfr();
			rnfr_arg = user_cmd->arg;
			break;	
		}
	case 8: {
			printf("call rnto()");
			printf("\n");
			if (strcmp(rnfr_arg, "")!=0 && strcmp(user_cmd->arg, "")!=0) {
				//do_rnto(rnfr_arg,user_cmd->arg);
				rnfr_arg = "";
			} else {
				char	*mess = "450 No username input.\r\n";
				printf("\n");
				while ((write(user_env.connect_fd,mess,strlen(mess))) == -1) {
					perror("wirte error");
				} 
			}
			break;	
		}
	case 9: {
			printf("call abor()\n");
			fflush(stdout);
			//do_abor();
			break;	
		}
	case 10: {
			printf("call dele()\n");
			//do_dele(user_cmd->arg);
			break;	
		}
	case 11: {
			printf("call rmd()\n");
			//do_rmd(user_cmd->arg);
			break;	
		}
	case 12: {
			printf("call mkd()\n");
			//do_mkd(user_cmd->arg);
			break;	
		}
	case 13: {
			printf("call pwd()\n");
			//do_pwd();
			break;	
		}
	case 14: {
			printf("call syst()\n");
			//do_syst();
			break;	
		}
	case 15: {
			printf("call quit()\n");
			//do_quit();
			break;	
		}
	case 16: {
			printf("call port()\n");
			//do_port(user_cmd->arg);
			break;	
		}
	case 17: {
			printf("call noop()\n");
			//do_noop();	
			break;	
		}
	case 18: {
			printf("call pasv()\n");
			printf("\n");
			//do_pasv();
			break;	
		}
	case 19: {
			printf("call type()\n");
			//do_type(user_cmd->arg);
			break;	
		}
	case 20: {
			printf("call mode()\n");
			//do_mode();
			break;	
		}
	case 21: {
			printf("call stat()\n");
			//do_stat(user_cmd->arg);
			break;	
		}
	case 22: {
			printf("call stru()\n");
			//do_stru();
			break;	
		}
	case 23: {
			printf("call list()\n");
			//do_list(user_cmd->arg);
			break;	
		}
	case 0: {
			printf("call failed()\n");
			//failed();
			break;	
		}
	}		
	return 1;
}

int main()
{	
	char *buff = "  !@  #$   usEr     a$@#@ /aa /   fdd";

	parse_cmd(buff);
	return 0;
}
