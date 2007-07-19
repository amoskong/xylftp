#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/time.h>
#include <pthread.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <openssl/md5.h>



typedef unsigned int bool;
#define TRUE 1
#define FALSE 0
#define MAXFD 65535
#define LISTENQ 128
#define MAX_ADDR 256
#define _BUFFER_LENGTH 150
#define CONFIG_FILE "/etc/xylftp/xylftp.conf"
#define PATH_NAME_LEN 1024
#define USER_NAME_LEN 32
#define MAX_CMD 5
#define MAX_ARG 4096
#define MAX_MSG_LEN 256


/*运行时的环境变量，从配置文件中读取*/
struct run_env{
	bool anonymous_enable;				/*是否允许匿名登录*/
	unsigned short ftp_port;			/*FTP使用的端口号*/
	unsigned int local_umask;			/*上传文件权限*/
	unsigned int log_file_enable;			/*是否启用日志*/
	char *log_file;					/*日志文件存储路径*/
	unsigned int idle_session_timeout;		/*控制链接的最大空闲时间，超时断开链接*/
	unsigned int data_connection_timeout;		/*数据链接的最大空闲时间，超时断开链接*/
	char *ftpd_banner;				/*登录欢迎信息*/
	unsigned int max_clients;			/*允许的最大客户数目*/
	unsigned int max_links;				/*允许的最大链接数目*/
	unsigned int passive_port_max;			/*被动模式下监听的端口范围*/
	unsigned int passive_port_min;
	char ftp_dir[PATH_NAME_LEN];			/*FTP根目录位置*/
	char *user_pass_file;				/*用户数据文件目录*/
};


/*用户登录服务器的环境变量*/
struct user_env{
	bool login_in;				/*是否已经登录*/
	char user_name[USER_NAME_LEN];		/*登录的用户名*/
	unsigned int user_id;			/*登录的用户ID*/
	unsigned int client_data_port;		/*客户端数据连接使用端口*/
	char *client_ip;			/*客户端ip*/
	unsigned short client_port;		/*客户所使用的端口号*/
	unsigned long login_time;		/*登录时间*/
	unsigned long last_operation_time;	/*上次操作时间*/
	char current_path[PATH_NAME_LEN];	/*当前路径*/
	unsigned int enable_upload;		/*是否允许上传*/
	bool passive_on;			/*是否为被动被模式*/
	bool ascii_on;				/*是否为ascii码模式*/
	int connect_fd;				/*控制连接*/
	int data_fd;				/*数据连接*/
	unsigned int upload_files;
	unsigned int upload_kbytes;
	unsigned int download_files;
	unsigned int download_kbytes;		/*以上四项为传输过程中的统计数据*/
};





struct user_env user_env;
struct run_env run_env;

/*implement of PASS*/
int do_pass(char *pass)
{
	char mess[50];
	char password[16];
	char md[16];
	char name[16];
	const char *log_error = "Login failed!\r\n";
	const char *logged = "503 You have already logged in!\r\n";
	FILE *fp;
	size_t len = 0;
	ssize_t k;
	char *line = NULL,*tmp;
	int i,j,pass_len;

	if (user_env.login_in == TRUE) {
		write(user_env.connect_fd, logged, strlen(logged));
		return -1;
	}

	if (strcmp(user_env.user_name,"anonymous") == 0) {
		snprintf(mess,50,"230 User anonymous logged in!\r\n");
		user_env.login_in = TRUE;
		write(user_env.connect_fd,mess,strlen(mess));
		return 0;
	} 

	if ((fp = fopen(run_env.user_pass_file,"r")) == NULL) {
		/*write_log("open user_pass_file error",0);*/
		write(user_env.connect_fd,log_error,strlen(log_error));
		return -1;
	}

	while ((k = getline(&line,&len,fp)) != -1) {
		tmp = line;
		j = 0;
		for (i = 0; ;i++) {
			if (tmp[i] == ':') {
				i = i + 2;
				break;
			}
		}
		for( ; ;i++) {
			if(tmp[i] == ':') {
				name[j] = '\0';
				break;
			} else {
				name[j++] = tmp[i];
			}
		}

		if (strcmp(name,user_env.user_name) == 0) break;
		
	}

	if (k == -1) {
		snprintf(mess,50,"530 Login incorrect!\r\n");
		write(user_env.connect_fd,mess,strlen(mess));
		return -1;
	}
	
	fscanf(fp,"%d",&user_env.user_id);
	tmp = line;
	for (i = 0,j = 0;tmp[i] != '\n' && j < 6;i++) {
		
		if (tmp[i] == ':') {
			j++;
		}
	}	
	
	for (j = 0;j < 16;) {
		password[j++] = tmp[i++];
	}

	pass_len=strlen(pass);	

	MD5((const unsigned char *)pass, pass_len, (unsigned char *)md);   

	if (!memcmp(password, md, 16) ) { 
			snprintf(mess,50,"230 User %s logged in.\r\n",user_env.user_name);
			user_env.login_in = TRUE;
			write(user_env.connect_fd,mess,strlen(mess));
			return 0;
	} else {
		snprintf(mess,50,"530 Login incorrect!\r\n");
		write(user_env.connect_fd,mess,strlen(mess));
		return -1;			
	}		
	
}

/*implement of PASS*/

int main(void)
{
	char a[30];
	user_env.connect_fd = 0;
	scanf("%s",a);
	strcpy(user_env.user_name,a);
	run_env.user_pass_file = "/etc/xylftp/xylftp.pass";
	scanf("%s",a);
	do_pass(a);
	return 0;
}

