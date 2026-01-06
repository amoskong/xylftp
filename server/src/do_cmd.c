/*
 * Copyright (c) 2007,西安邮电学院Linux兴趣小组
 * All rights reserved.
 * 
 * 文件名称：命令处理
 * 摘    要：处理MKD RMD DELE PASV STOR命令
 * 
 * 当前版本：1.2
 *
 * 修 改 者：刘洋
 * 修改日期：2007年12月8日
 * 修改原因：LIST与STAT中的日期格式不符合要求，在很多图形界面客户端中出现问题，故更改之。
 *
 * 作    者：聂海海 王亚刚 郭拓 贾孟树
 * 完成日期：2007年5月30日
 * 修 改 者：刘洋 林峰 王聪 聂海海
 * 修改日期：2007年6月16日
 * 取代版本：1.1
 * 摘    要：原来增加的命令不很符合要求，重建此文件。全部根据要求，现将命令单独在test目录中通过的逐条加入。
 * 此次加入命令STAT。

 *14日加入USER。
 *6.15: 修正英文语法错误，排版错误。
 *6.16: 添加do_mkd，do_rmd, do_dele, do_retr
 *6.16:添加do_pass
 */

#define _GNU_SOURCE
#include <wordexp.h>
#include "xylftp.h"
#include "debug.h"

#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

extern struct user_env user_env;
extern struct run_env run_env;

static void _stat_fail_450(void);
static void _stat_error_421(void);
static void _stat_error1_501(void);
static void _stat_error2_501(void);
static void _stat_error3_501(void);
static void _stat_fail_550(void);
static void _stat_success_150(void);
static void _stat_success_226(void);
static void _stat_fail_501(void);
static void _path_tail(char *, char *);
static int _get_local_ip_address(int sock, char* ip_addr);
static int _stat_mkd(const char *path);    
static int _stat_rmd(const char *path);    
static int _stat_dele(const char *path);
static int _stat_retr(const char *);
static int _ignore_sigpipe(void);
static int _analysis_ipaddr(char *, char **, int *);
static int _chdir(const char*);
static int _response(const char*);
int do_quit(void);
int failed(const char *s);

int do_user(char username[])
{
	const char anonymous[] = "anonymous";
	const char inf_buf[] = "331 Please send you password.\r\n";
	const char no_anonymous[] = "555 Anonymous not allowed on this server.\r\n"; 
	const char noname[] = "500 USER: command requires a parameter.\r\n";	
	const char logged[] = "503 You have already logged in.\r\n";

	if (username[0] == '\0') {
		_response(noname);
		debug_printf("%s\n", noname);
		return -1;
	}	

	if (user_env.login_in == TRUE) {
		_response(logged);
		debug_printf("%s\n", logged);
		return -1;
	}

	if (strcmp(username, anonymous) == 0){
		if(run_env.anonymous_enable){
			strcpy(user_env.user_name, username);	
			_response(inf_buf);
			debug_printf("%s\n", inf_buf);
		} else {
			_response(no_anonymous);
			debug_printf("%s\n", no_anonymous);
		}			
	}
	/*anonymous client authentication*/
	else {
		strcpy(user_env.user_name, username);		
		_response(inf_buf);
		debug_printf("%s\n", inf_buf);
	}

	return 0;
}
/*implement of USER*/

static char get_hex(const char *buf)
{
	char tmp[3];
	strncpy(tmp, buf, 2);
	return (char) strtol(tmp, NULL, 16);
}

/*implement of PASS*/
int do_pass(char *pass)
{
	char mess[50];
	char password[16];
	char md[16];
	char name[16];
	const char log_error[] = "Login failed!\r\n";
	const char logged[] = "503 You have already logged in.\r\n";
	FILE *fp;
	size_t len = 0;
	ssize_t k;
	char *line = NULL, *tmp;
	int i, j, pass_len, id;

	if (user_env.login_in == TRUE) {
		_response(logged);
		return -1;
	}

	if (strcmp(user_env.user_name, "anonymous") == 0) {
		snprintf(mess, 50, "230 User anonymous logged in.\r\n");
		user_env.login_in = TRUE;
		user_env.enable_upload = FALSE;
		_response(mess);
		return 0;
	} 

	if ((fp = fopen(run_env.user_pass_file, "r")) == NULL) {
		write_log("open user_pass_file error",0);
		_response(log_error);
		return -1;
	}

	while ((k = getline(&line, &len, fp)) != -1) {
		fscanf(fp, "%d", &id);
		tmp = line;
		j = 0;
		for (i = 0; ; i++) {
			if (tmp[i] == ':') {
				i = i + 2;
				break;
			}
		}
		for( ; ; i++) {
			if(tmp[i] == ':') {
				name[j] = '\0';
				break;
			} else {
				name[j++] = tmp[i];
			}
		}

		if (strcmp(name, user_env.user_name) == 0)
			break;
	}

	if (k == -1) {
		snprintf(mess, 50, "530 Login incorrect.\r\n");
		_response(mess);
		free(line);
		return -1;
	}

	user_env.user_id = id;
	tmp = line;
	for (i = 0, j = 0; tmp[i] != '\n' && j < 6; i++) {
		if (tmp[i] == ':') {
			j++;
		}
		if (j == 4) {
			i = i + 2;
			if (tmp[i] == 'w') {
				user_env.enable_upload = TRUE;
			}			
		}
	}

	for (j = 0; j < 16; ) {
		password[j++] = get_hex(tmp+i);
		i += 2;
	}
	free(line);

	pass_len=strlen(pass);
	MD5((const unsigned char *)pass, pass_len, (unsigned char *)md);   

	fclose(fp);

	if (!memcmp(password, md, 16) ) { 
			snprintf(mess, 50, "230 User %s logged in.\r\n", user_env.user_name);
			user_env.login_in = TRUE;
			_response(mess);
			return 0;
	} else {
		snprintf(mess, 50, "530 Login incorrect.\r\n");
		_response(mess);
		return -1;
	}

}

/*implement of STAT*/

static char *_format_time(struct stat *stat_buf, char *str)
{
	const char month[][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	struct tm tmp; 

	gmtime_r(&(stat_buf->st_mtime), &tmp);

	if (S_ISDIR(stat_buf->st_mode)) {
		snprintf(str, strlen(str), "%s %2d  %4d", month[tmp.tm_mon], tmp.tm_mday, tmp.tm_year + 1900);
	}
	else {
		snprintf(str, strlen(str), "%s %2d %2d:%2d", month[tmp.tm_mon], tmp.tm_mday, tmp.tm_hour, tmp.tm_min);
	}

	return str;
}

static char *_get_line_info(struct stat *stat_buf, char *buf, int *width)
{
	char att[11] = "----------", tm[26];
	char time_str_buff[] = "Dec 12, 18:00";
	unsigned int t = S_IRUSR;
	int i;

	if (S_ISREG(stat_buf->st_mode)) {
		att[0] = '-';
	}
	else if (S_ISDIR(stat_buf->st_mode)) {
		att[0] = 'd';
	}
	else if (S_ISCHR(stat_buf->st_mode)) {
		att[0] = 'c';
	}
	else if (S_ISBLK(stat_buf->st_mode)) {
		att[0] = 'b';
	}
	else if (S_ISFIFO(stat_buf->st_mode)) {
		att[0] = 'p';
	}
	else if (S_ISLNK(stat_buf->st_mode)) {
		att[0] = 'l';
	}
	else if (S_ISSOCK(stat_buf->st_mode)) {
		att[0] = 's';
	}
	for (i = 1;i < 10;i++,t >>= 1) {
		if (stat_buf->st_mode & t) {
			switch (i % 3) {
			case 1: att[i] = 'r';
				break;
			case 2: att[i] = 'w';
				break;
			case 0: att[i] = 'x';
			}
		}
	}
	if (snprintf(tm, 25, "%s", _format_time(stat_buf, time_str_buff)) == -1) {
		write_log("Read system time error!", 0);
	}
	if (snprintf(buf, MAX_PATH, "%s% *d %*s %*s% *d %s", att, width[2], (size_t)stat_buf->st_nlink, width[0],
				run_env.visible_user_name,width[1], run_env.visible_group_name,width[3], 
				(size_t)stat_buf->st_size, tm) == -1){
		write_log("The path string is overflow!", 0);
	}
	return buf;	
}

static int _get_int_len(int n)
{
	int i;
	for (i = 1;n /= 10;i++) {
		;
	}
	return i;
}

static int _stat_no_arg(void)
{
	char msg[MAX_MSG_LEN]={0};
	if (snprintf(msg, MAX_MSG_LEN, "211-Status for user %s from %s:\r\n"
			"211-Stored %d files,%d KB\r\n211-Retrieved %d files,%d KB\r\n211 End of Status.\r\n",
			user_env.user_name, user_env.client_ip, user_env.upload_files,
			user_env.upload_kbytes, user_env.download_files, user_env.download_kbytes) == -1) {
		write_log("The message is overflow.",0);
	}
	return _response(msg);
}

static int _stat_with_arg(const char *cmd_arg)
{
	char buf[MAX_PATH], full_path[MAX_PATH], tmp[MAX_PATH];
	const char not_found[] = "450 Target path doesn't exist.\r\n";
	struct dirent *direntp;
	DIR *target_dir;
	struct stat stat_buf;
	int max_width[4] = {0}; /*max length of user name, group name,link number and the file length number*/
	int t;

	if (*cmd_arg == '/') {
		if (snprintf(buf, MAX_PATH, "%s", cmd_arg) == -1) {
			write_log("Path string overflows.", 0);
		}
	}
	else {
		if (snprintf(buf, MAX_PATH, "%s%s", user_env.current_path, cmd_arg) == -1) {
			write_log("Path string overflows.", 0);
		}
	}

	strcpy(full_path, buf); /*the length of buf won't greater than the full_path so it's safe.*/

	if (!(target_dir = opendir(buf))) {
		_response(not_found);
		return closedir(target_dir);
	}

	max_width[0] = (t = strlen(run_env.visible_user_name)) > max_width[0]?t:max_width[0];
	max_width[1] = (t = strlen(run_env.visible_group_name)) > max_width[1]?t:max_width[1];

	while ((direntp = readdir(target_dir)) != NULL) {
		if (snprintf(buf, MAX_PATH, "%s/%s", full_path, direntp->d_name) == -1) {
			write_log("Path string overflows.", 0);
		}

		if (stat(buf, &stat_buf) == -1) {
			write_log("Read local file status error", 0);
			closedir(target_dir);
			return -1;
		}

		max_width[2] = (t = _get_int_len(stat_buf.st_nlink)) > max_width[2]?t:max_width[2];
		max_width[3] = (t = _get_int_len(stat_buf.st_size)) > max_width[3]?t:max_width[3];
	}
	rewinddir(target_dir);

	if (snprintf(buf, MAX_PATH, "211-Status of %s\r\n", cmd_arg) == -1) {
		write_log("Path string overflows.",0);
	}

	_response(buf);

	while ((direntp = readdir(target_dir)) != NULL) {
		if (*(direntp->d_name) == '.') {
			continue;
		}
		if (snprintf(buf, MAX_PATH,"%s/%s", full_path, direntp->d_name) == -1) {
			write_log("Path string overflows.", 0);
		}

		if (stat(buf, &stat_buf) == -1) {
			write_log("Read local file status error", 0);
			closedir(target_dir);
			return -1;
		}
		if (snprintf(buf, MAX_PATH, "211-%s %s\r\n", _get_line_info(&stat_buf, tmp, max_width),
				direntp->d_name) == -1) {
			write_log("Path string overflows.", 0);
		}
		_response(buf);
	}

	if (snprintf(buf, MAX_PATH, "211 End of Status.\r\n") == -1) {
		write_log("Path string overflows.", 0);
	}
	_response(buf);
	return closedir(target_dir);
}

int do_stat(const char *cmd_arg)
{
	if (!strlen(cmd_arg)) {
		return _stat_no_arg();
	}

	return _stat_with_arg(cmd_arg);
}
/*end of the implement of STAT*/

int do_list(char *filename)
{
	int m_width[4]={3,10,10,10};
	int ret = 0;
	size_t i;
	char each_dir_inf[BUF_LEN] = {0};
	char buf[BUF_LEN] = {0};
	const char finished[] = "226 Transfer complete.\r\n";
	const char success[] = "150 Opening ASCII mode data connection for file list.\r\n";
	const char fail[] = "450 No such file or directory.\r\n";
	DIR *dir_inf_str;
	struct dirent *dirp;
	struct stat file_inf;
	char user_path[PATH_NAME_LEN] ={""};
	char inte_dir_inf[BUF_LEN]={0};
	char **w;
	wordexp_t wxp;

	_response(success);
	if(filename[0]!='/')	{
		if (strcmp(user_env.current_path, "/") != 0) {
			snprintf(buf, BUF_LEN, "%s/%s", user_env.current_path, filename);
		} else {
			snprintf(buf, BUF_LEN, "%s%s", user_env.current_path, filename);
		}
	}
	else {
		snprintf(buf, BUF_LEN, "%s", filename);
	}

	debug_printf("buf=%s\n", buf);

	wordexp(buf, &wxp, 0);
	w = wxp.we_wordv;
	for (i=0; i < wxp.we_wordc; i++){
		if (*w[i] == '.') {
			continue;
		}

		if (stat(w[i], &file_inf) == 0) {
			if (!S_ISDIR(file_inf.st_mode)
				|| (wxp.we_wordc!=1 || strcmp(w[0], buf) != 0)) {
				snprintf(inte_dir_inf, BUF_LEN, "%s %s\r\n",
					_get_line_info(&file_inf, each_dir_inf, m_width),
					w[i]);
				write(user_env.data_fd, inte_dir_inf,
					strlen(inte_dir_inf));
				continue;
			}
		} else {

			debug_printf("w[i]=%s\n", w[i]);

			ret = -1;
			break;
		}

		debug_printf("Get here with %s\n", w[i]);

		if((dir_inf_str = opendir(w[i])) != NULL){
			while((dirp = readdir(dir_inf_str)) != NULL){
				if (*(dirp->d_name) == '.') {
					continue;
				}

				memset(each_dir_inf, 0, BUF_LEN);
				memset(inte_dir_inf, 0, BUF_LEN);
				snprintf(user_path, PATH_NAME_LEN, "%s/%s", w[i], dirp->d_name);
				if(stat(user_path, &file_inf) != -1){
					snprintf(inte_dir_inf, BUF_LEN, "%s %s\r\n",
						_get_line_info(&file_inf, each_dir_inf, m_width),
						dirp->d_name);
					write(user_env.data_fd, inte_dir_inf,
						strlen(inte_dir_inf));
				}
			}
		} else {
			ret = -1;
			break;
		}
		closedir(dir_inf_str);
	}/*for*/

	if (ret == 0) {
		_response(finished);
	} else {
		_response(fail);
	}
	close(user_env.data_fd);
	wordfree(&wxp);
	return ret;
}

/* 命令MKD的处理 */
int do_mkd(const char *path)           /*处理命令MKD的入口*/
{
	char str[PATH_NAME_LEN] = {""};
	if (user_env.enable_upload == 1) {                  /*判断权限*/
		if (path[0] == '/') {                   /*参数是路径名?*/
                	path = strcat(str, path);
		} else {		/*参数是目录名?*/
			strcpy(str, user_env.current_path);                
			if(str[strlen(str)-1] != '/')
				strcat(str, "/");
				path = strcat(str, path);
		}
		if(_stat_mkd(path) == 0) {
			return 0;
		} else {
        		return -1;
		}
 	} else {
		_stat_error_421();
		return -1;
	}
}

static int _stat_mkd(const char *path)
{     
	char buf[50+PATH_NAME_LEN] = {};

	debug_printf("mkd: path=%s \r\n", path);
	if (mkdir(path, S_IRWXU) == 0) {              /*创建目录成功*/
		snprintf(buf, 50+PATH_NAME_LEN, "257 Directory successfully created:%s.\r\n", path);
		_response(buf);
		return 0;
	} else {
		if (errno == EEXIST) {
			_stat_error1_501();
        	} 
		else if (errno == ENAMETOOLONG) {
			_stat_error2_501();
		} else {
			_stat_fail_450();
		}
		return -1;
	}
}

/*
 *命令RMD的处理
 */
int do_rmd(const char *path)                /*处理命令RMD的入口*/
{
	char str[PATH_NAME_LEN]={""};
	if (user_env.enable_upload == 1) {	/*判断权限*/
		if (path[0] == '/') {	/*参数是路径名?*/
			path = strcat(str, path);
		}
		else {	/*参数是目录名?*/
			strcpy(str, user_env.current_path);
			if(str[strlen(str)-1] != '/') {
				strcat(str, "/");
			}
			path = strcat(str,path);
		}
		if (_stat_rmd(path) == 0) {
			return 0;
		} else {
			return -1;
		}
	} else {
		_stat_error_421();
		return -1;
	}
}

static int _stat_rmd(const char *path)
{
	const char buf[] = "250 RMD command successful.\r\n";

	debug_printf("rmdir at %s \r\n", path);
	if (rmdir(path) == 0) {	/*删除目录成功*/
		_response(buf);
		return 0;
	} else {
		if(errno == ENOENT) {
			_stat_error3_501();
		} else if (errno == ENAMETOOLONG) {
			_stat_error2_501();
		} else {
			_stat_fail_450();
		}
		return -1;
	}
}

/*
 *命令DELE的处理
 */
int do_dele(const char *path)          /*处理命令DELE的入口*/
{
	char str[PATH_NAME_LEN] = {""};
	if (user_env.enable_upload == 1) {    /*判断权限*/
		if (path[0] == '/') {         /*参数是路径名?*/
			path = strcat(str, path);
		} else {       /*参数是目录名?*/
			strcpy(str, user_env.current_path);
			if(str[strlen(str)-1] != '/') {
				strcat(str, "/");
			}
			path = strcat(str,path);
		}
		if (_stat_dele(path) == 0) {
			return 0;
		} else {
			return -1;
		}
	} else {
		_stat_fail_550();
		return -1;
	}
}

int _stat_dele(const char *path)
{
	const char msg[] = "250 File sucessfully deleted.\r\n";

	debug_printf("dele at %s \r\n",path);
	if (unlink(path) == 0) {	/*删除文件成功*/
		_response(msg);
		return 0;
	} else {
		if (errno == ENOENT) {
			_stat_error3_501();
		} else if (errno == ENAMETOOLONG) {
			_stat_error2_501();
		} else { 
			_stat_fail_450();
		}
		return -1;
	} 
}

/*
 *命令RETR的处理
 */
static int _stat_retr(const char *path)
{ 
	int fd;
	ssize_t i = 0;
	char buf[BUF_LEN]={""};      

	debug_printf("retr from %s\n",path);

	fd = open(path, O_RDONLY);
	if(fd != -1) {
		if((off_t)-1 == lseek(fd, user_env.restartat, SEEK_SET)){
			_stat_fail_501();
			r_close(fd);
			LOG_IT("lseek error.");
			return -errno;
		}
		_stat_success_150();
		while((i = read(fd, buf, BUF_LEN)) != 0) {
			if (i == -1) {
				r_close(user_env.data_fd);
				_stat_fail_501();
				return -1;
			} else {
				write(user_env.data_fd, buf, i);
				user_env.download_kbytes += i/1000;
			}
		}
		user_env.download_files++;
		r_close(fd);
		r_close(user_env.data_fd); 
		user_env.restartat = 0;
		_stat_success_226();
		return 0;
	} else {
		r_close(user_env.data_fd);
		_stat_fail_501();
		return -1;
	} 
}

int do_retr(const char *path)
{
	char str[PATH_NAME_LEN]={""};
	if (strlen(path) == 0) {
		close(user_env.data_fd); 
		return failed("RETR");
	}
	if (path[0] == '/') {                               /*参数是路径名?*/
		path = strcat(str, path);
	} else {                      /*参数是文件名?*/
		strcpy(str, user_env.current_path);
		if(str[strlen(str)-1] != '/') {
			strcat(str,"/");
		}
		path = strcat(str,path);
	}
	return _stat_retr(path);
}

/*命令执行失败*/
static void _stat_fail_450(void)
{
	const char msg[] = "450 File operation failed.\r\n";
	_response(msg);
}

/*出现错误*/
static void _stat_error_421(void)
{
	const char msg[] = "421 Service not available, closing control connection.\r\n";
	_response(msg);
	do_quit();
}

/*命令执行失败*/
static void _stat_fail_550(void)
{
	const char msg[] = "550 Requested action not taken. File unavailable.\r\n";
	_response(msg);
}

/*出现错误*/
static void _stat_error1_501(void)
{
	const char msg[] = "501 Wrong arguments, the filename exists.\r\n";
	_response(msg);
}

static void _stat_error2_501(void)
{
	const char msg[] =
		"501 Diretory or file name is too long.\r\n";
	_response(msg);
}

static void _stat_error3_501(void)
{
	const char msg[] =
		"501 Arguments wrong,the file or directory does not exists!\r\n";
	_response(msg);
}

static void _stat_success_150(void)
{ 
	const char msg[] = "150 File status okay; about to open data connection.\r\n";
	_response(msg);
}

static void _stat_success_226(void)
{ 
	const char msg[] = "226 Closing data connection."
             "Requested file-action succeed.\r\n";
	_response(msg);
}
 
static void _stat_fail_501(void)
{
	const char msg[] = "501 Syntax error in parameters or arguments.\r\n";
	_response(msg);
}

int do_mode(const char *arg)
{
	const char succ[] = "200 MODE S OK.\r\n";
	const char fail[] = "504 Command not implemented for that parameter.\r\n";
	if ((strlen(arg) == 1) && ((*arg == 's') || (*arg == 'S'))) {
		_response(succ);
		return 0;
	} else {
		_response(fail);
		return 1;
	}
}

static int _get_local_ip_address(int sock, char* ip_addr)
{
	char *ip = NULL;  
	int fd = sock, intrface;
	struct ifreq buf[MAX_INTERFACES];
	struct ifconf ifc;

	ifc.ifc_len = sizeof buf;
	ifc.ifc_buf = (caddr_t) buf;
	if (!ioctl(fd, SIOCGIFCONF, (char*)&ifc)) {
		intrface = ifc.ifc_len / sizeof(struct ifreq);
		while (intrface-- > 0) {
			if (!(ioctl(fd, SIOCGIFADDR, (char*)&buf[intrface]))) {
				ip = (inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));
				strcpy(ip_addr, ip);
				break;
			}
		} 
	} 
	return 0;
}

int do_pasv(void)
{
	char ip_addr[16] = {0};  /*I think 16 is just enough.*/
	char port_buf[64] = {0};

	const char pasv_fail[] = "425 Can't open data connection.\r\n";
	int sock, ret;
	int data_port; 
	int opt = SO_REUSEADDR;
	socklen_t i;
	struct sockaddr_in data, cliaddr;
	unsigned int port, port1, port2;
	struct timeval tv;
	char *tmp;

	tv.tv_sec = run_env.data_connection_timeout;
	tv.tv_usec = 0;
	bzero(&data, sizeof(data)); 
	data_port = 0;
	data.sin_family = AF_INET;    /*建立数据连接*/
	data.sin_port = htons(data_port);
	data.sin_addr.s_addr = htonl(INADDR_ANY);

	i = sizeof(cliaddr);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		LOG_IT("socket error in do_pasv().");
		return -errno;
	} 
	_get_local_ip_address(sock, ip_addr);
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
  	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(sock, (struct sockaddr *)&data, i) != 0) {
		LOG_IT("bind error in do_pasv().");
		return -errno;
	}

	getsockname(sock, (struct sockaddr *)&data, (socklen_t*) &i); 

	debug_printf("pasv: ip=%s\n",  ip_addr);
	debug_printf("pasv: port=%d\n", ntohs(data.sin_port));

	port = ntohs(data.sin_port);
	port1 = port / 256;
	port2 = port % 256;
	while ((tmp = strchr(ip_addr, '.')) != NULL) {
		*tmp = ',';
		tmp++;
	}

	if (listen(sock, 1) != 0){
		LOG_IT("listen error in do_pasv().");
		return -errno;
	}

	snprintf(port_buf, 64, "227  Entering Passive Mode (%s,%d,%d).\r\n",
		ip_addr, port1, port2);
	_response(port_buf);

	ret = accept(sock, (struct sockaddr *)&cliaddr, &i);
	if (ret!=-1) {
		user_env.data_fd = ret;
		//write(user_env.connect_fd, pasv_ready, strlen(pasv_ready));
		close(sock);		
		return 0;				
	} else {
		LOG_IT("accept error in do_pasv().");
		_response(pasv_fail);
		close(sock);		
		return -errno;
	}
} 

int do_syst(void)
{
	const char wel[] = "215 UNIX Type: L8\r\n";
	
	if (_response(wel) == -1) {
		LOG_IT("write error in do_syst()");
		return -1;
	}
	return 0;	
}

int do_noop(void)
{
	const char mess[] = "200 NOOP command successful.\r\n";
	if (_response(mess) == -1) {
		LOG_IT("write error in do_noop()");
		return -1;
	}
	return 0;
}

int do_type(char *arg)
{
	if (strcasecmp(arg, "I") == 0) {
		user_env.ascii_on = FALSE;
		_response("200 Type set to I.\r\n");
	} else if (strcasecmp(arg, "A") == 0) {
		user_env.ascii_on = TRUE;
		_response("200 Type set to A.\r\n");
	} else {
		_response("500 Type not understood.\r\n");	
		return -1;
	}

	return 0;
}

int do_stru(char *arg)
{
	const char fail[] = "501 'STRU' not understood.\r\n";
	const char succ[] = "200 Structure set to F.\r\n";
	const char unsupported[] = "504 Unsupported structure type.\r\n";
	const char unknown[] = "501 Unrecognized structure type.\r\n";

	if (strcmp(arg, "") == 0) {
		_response(fail);
		return -1;
	} else if (strcasecmp(arg, "F") == 0) {
		_response(succ);
	} else if (strcasecmp(arg, "P") == 0
				|| strcasecmp(arg, "R") == 0) {
		_response(unsupported);
	} else {
		_response(unknown);
	}

	return 0;
}

/*
 *Note! When this is called in parse_cmd, it means
 *we received an 'ABOR' when no data connection exists.
 *So we just reply a 226 and do nothing actually.
 */
int do_abor(char *arg)
{
	const char fail[] = "501 Can not understood.\r\n";
	const char succ[] = "226 Abort successful.\r\n";

	if (strcmp(arg, "") != 0) {
		_response(fail);
		return -1;
	} else {
		_response(succ);	
	}

	return 0;
}

int do_cwd(const char *dir)
{
	if (_chdir(dir) < 0) {
		return -1;
	} else {
		_response("250 Command ok.\r\n");
		return 0;
	}
}

int do_cdup(void)
{
	if (_chdir("..") < 0) {
		return -1;
	} else {
		_response("250 CDUP Command ok.\r\n");
		return 0;
	}
}

int do_pwd(void)
{
	char mess[PATH_NAME_LEN+6] = {0};
	snprintf(mess, 4096, "257 \"%s\"\r\n", user_env.current_path);
	_response(mess);
	return 0;
}

int do_rnfr(void)
{
	if (_response("350 Please send the RNTO command.\r\n") < 0) {
		return -1;
	} else {
		return 0;
	}
}

int do_rnto(const char *old_path, const char *new_path)
{
	const char *mess;

	debug_printf("%s,%s\n", old_path, new_path);

	if (user_env.enable_upload != 1) {
		_response("550 Permission denied!\r\n");
		return -1;
	}

	if (rename(old_path, new_path) < 0) {
		switch (errno) {
		case EISDIR:
			mess = "553 New path is a directory.\r\n";
			break;
		case EBUSY:
			mess = "502 The files are in use now.\r\n";
			break;
		default:
			mess = "501 Can't rename this file.\r\n";
		}
		_response(mess) ;
		return -1;
	}
	_response("250 Command succeed.\r\n") ;
	return 0;
}

int do_port(char *arg)
{
	struct sockaddr_in client;
	int error;
	int retval;
	int sock;
	int port;
	char *addr;

	if (_analysis_ipaddr(arg, &addr, &port) < 0) {
		return -1;
	}

	debug_printf("addr=%s, port=%d\n", addr, port);

	bzero((char *)&client, sizeof(client)) ;
	client.sin_port = htons(port);
	if (inet_pton(AF_INET, addr, &client.sin_addr) == 0) {
		_response("501 Incorrect IP address.\r\n");
		return -1;
	}
	if (strcmp(user_env.port_ip, addr) != 0){
		strcpy(user_env.port_ip, addr);
		user_env.port_connections = 0;
	}
	if (strcmp(user_env.port_ip, user_env.client_ip) != 0){
		if (user_env.port_connections >= run_env.max_port_connections){
			_response("421 Failed to create data connection.\r\n");
			return -1;
		} else {
			user_env.port_connections++;

			debug_printf("user_env.port_connections=%d\n", user_env.port_connections);

		}
	}
	client.sin_family = AF_INET;
	if ((_ignore_sigpipe() == -1) || ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)) {
		_response("421 Failed to create data connection.\r\n");
		return -1;
	}
	if (((retval = connect(sock, (struct sockaddr *)&client, sizeof(client))) == -1)
			&& ((errno ==  EINTR)||(errno == EALREADY))) {
		/*create data connection*/;
	}
	if (retval == -1) {
		error = errno;
		while ((close(sock) == -1) && (errno == EINTR))
			/*do nothing*/;
		errno = error;
		_response("421 Failed to create data connection.\r\n");
		return -1;
	}
	_response("200 Succeed to create data connection.\r\n");
	user_env.data_fd = sock;
	return 0;
}

static int _ignore_sigpipe(void)
{
	struct sigaction act;
	if (sigaction(SIGPIPE, (struct sigaction *)NULL, &act) == -1) {
		return -1;
	}
	if (act.sa_handler == SIG_DFL) {
		act.sa_handler = SIG_IGN;
		if (sigaction(SIGPIPE, &act, (struct sigaction *)NULL) == -1) {
			return -1;	
		}
	}
	return 0;
}

static int _analysis_ipaddr(char *str, char **re_addr, int *re_port)
{
	static char addr[16];
	int ip[4];
	int port[2];
	int m;
	int i = 0;
	sscanf(str, "%d,%d,%d,%d,%d,%d", &ip[0], &ip[1], &ip[2], &ip[3], &port[0], &port[1]);
	while (i < 4) {
		if ((ip[i] > 255) || (ip[i] < 0)) {
			return -1;
		}
		i++;
	} 
	m = port[0]*256 + port[1];
	if (m < 0) {
		return -1;	
	}
	snprintf(addr, 16, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	*re_port = m;
	*re_addr = addr;
	return 0;
}

#define RESPONSE(msg)	write(user_env.connect_fd, msg, strlen(msg))

static int _response(const char *buf)
{ 
	ssize_t byteswrite;

	while (((byteswrite = RESPONSE(buf)) == -1)
		&& (errno == EINTR))
		/*do nothing here*/;
	if (byteswrite < 0) { /*If cannot write to socket,close all connections and kill the process*/
		r_close(user_env.connect_fd);
		   /*？是否要关闭数据连接？*/
		write_log("remote host did not responsed.", 1);
	}
	return (int)byteswrite;
}

static int _chdir(const char *cdir)
{
	char *buffer = NULL;
	int size = 128;

	if (strlen(cdir) == 0) {
		strcpy(user_env.current_path,"/");
		if (chdir("/") < 0) {
			write_log("chdir failed", 0);
			return -1;
		}
		return 0;
	}

	if ((chdir(cdir)) < 0) {
		LOG_IT("chdir error in _chdir().");
		_response("501 Can't change directory.\r\n");
		return -1;	 	
	}
	/*检查是否合法,即是否超过根目录*/
	if ((buffer = malloc(size)) == NULL) {
		_response("550 Can't change directory.\r\n");
		LOG_IT("malloc failed");
		return -1;
	}
	while ((getcwd(buffer, size) == NULL) && (errno == ERANGE)){
		size *= 2;
		if((buffer = realloc(buffer, size)) == NULL){
			_response("550 Can't change directory.\r\n");
			LOG_IT("realloc failed");
			return -1;
		}
	}

	strcpy(user_env.current_path, buffer);
	free(buffer);
	return 0;
}

int do_quit(void)
{
	const char wel[] = "221 Goodbye.\r\n";

	while ((write(user_env.connect_fd, wel, strlen(wel))) == -1) {
		LOG_IT("write error in do_quit().");
	}
	close(user_env.connect_fd);
	free_sources();
	kill(getpid(), SIGTERM);
	return 0;
}

int do_stor(char *arg)
{
	char pathname[MAX_PATH]={""};
	const char stor_ok[] = "226 Transfer complete.\r\n";
	int sd, fd;
	ssize_t rsize;
	char buff[BUF_LEN];


	debug_printf("****STOR; arg = %s\n", arg);
	debug_printf("user_env.current_path=%s\n", user_env.current_path);

	if (!user_env.enable_upload) {
		_response("550 Permission denied.\r\n");
		close(user_env.data_fd);
		write_log("Attempt to write.", 1);
		return -1;
	}
	if (arg[0]=='/'){
#if 0
		strcpy(pathname, run_env.ftp_dir);
		if(pathname[strlen(pathname)-1] != '/') {
			strcat(pathname, "/");
		}
		if(arg[0]=='/') {
			arg++;
		}
#endif
		strncpy(pathname, arg, MAX_PATH);
	}
	else {
		strcpy(pathname, user_env.current_path);
		if(pathname[strlen(pathname)-1] != '/')
			strcat(pathname, "/");
		strcat(pathname, arg);
	} 

	debug_printf("pathname=%s.\n", pathname);

	fd = open(pathname, NULL, O_RDWR|O_CREAT);
	if (fd < 0) {
		_response("550 Permission denied.\r\n");
		close(user_env.data_fd);
		LOG_IT("Open error in do_stor().");
		return -errno;
	}
	if ((off_t)-1 == lseek(fd, user_env.restartat, SEEK_SET)){
		r_close(fd);
		LOG_IT("lseek error in do_stor().");
		return -errno;
	}
	_stat_success_150();

	debug_printf("%s create OK! \n", pathname);

	sd = user_env.data_fd;	
	for (;;) {
		rsize = read(sd, buff, BUF_LEN);
		if(rsize == 0) {
			break;
		}
		if (rsize < 0) {
			LOG_IT("Read socket error in do_stor().");
			break;
		}
		write(fd, buff, rsize);
		user_env.upload_kbytes += rsize/1024;
	}
	r_close(fd);
	r_close(sd);
	user_env.restartat = 0;
	_response(stor_ok);
	user_env.upload_files++;

	debug_printf("%d files, %d KB.\n", user_env.upload_files, user_env.upload_kbytes);

	return 0;
}

int do_rest(const char *arg)
{
	char *endptr;
	const char failed_msg[] = "501 REST needs a numeric parameter\r\n";
	const char succ_msg[] = "350 Restarting successfully."
			" Send STORE or RETRIEVE to initiate transfer\r\n";
	const char restrict_msg[] = "501 REST: Resuming transfers not"
			" allowed in ASCII mode\r\n";

	user_env.restartat = (off_t) strtoull(arg, &endptr, 10);
	if (*endptr != 0 || user_env.restartat < (off_t) 0) {
		user_env.restartat = 0;
		_response(failed_msg);
		return -1;
	} else {
		if (user_env.ascii_on && user_env.restartat != 0) {
			_response(restrict_msg);
			return 0;
		} else {
			_response(succ_msg);
			return 0;
		}
	}
}

int do_size(const char *name)
{
	const char fail_msg[] = "550 Could not get file size.\r\n";
	const char fail_msg2[] = "550 I can only retrieve regular files.\r\n";
	char buf[MAX_MSG_LEN] = {0,};
	struct stat st;

	if (!*name) {
		_response(fail_msg);
		return -1;
	} else if (stat(name, &st)) {
		_response(fail_msg);
		write_log("stat failed.", 0);
		return -errno;
	} else if (!S_ISREG(st.st_mode)) {
		_response(fail_msg2);
		return -1;
	} else {
		snprintf(buf, MAX_MSG_LEN, "%d %llu\r\n", 213,
			(unsigned long long)st.st_size);
		_response(buf);
		return 0;
	}
}

int do_site(const char *args)
{
	const char ok_msg[] = "200 Command okay.\r\n";
	const char bad_msg[] = "550 Bad file.\r\n";
	const char fail_msg[] = "550 Can't chmod.\r\n";
	const char no_msg[] = "502 Command not implemented.\r\n";

	if (!strncasecmp("CHMOD ", args, 6)) {
		mode_t mode;
		const char *filename;
		char buf[256] = {0,};

		if (!user_env.enable_upload) {
			_response("550 Permission denied.\r\n");
			LOG_IT("Attempt to write.");
			return -3;
		}
		args += 5;
		mode = strtol(args, (char**)&args, 8);
		while (*args && isspace(*args))
			args++;
		filename = args;

		debug_printf("mode=%d, filename=%s\n", mode, args);

		if (!getcwd(buf, 128) ||
		   strlen(buf) + strlen(filename) + 1 > 256) {
			_response(bad_msg);
			return -1;
		}

		strcat(buf, "/");
		strcat(buf, filename);
		if (-1 == chmod(filename, mode)) {
			_response(fail_msg);
			LOG_IT("chmod failed");
			return -errno;
		}
		_response(ok_msg);
		return 0;
	} else {
		_response(no_msg);
		return -2;
	}
}

int do_help(const char *args)
{
	const char help[] = "214-The following commands are implemented.\r\n"
			"214-USER    QUIT    PASS    SYST    HELP    PORT    PASV    LIST\r\n"
			"214-NLST    RETR    STOR    TYPE    MKD     RMD     DELE    PWD\r\n"
			"214-CWD     SITE    CDUP    RNFR    RNTO    NOOP    NLST\r\n"
			"214 End of list.\r\n";
	const char helpsite[] = "214-The following SITE commands are implemented.\r\n"
			"214-CHMOD   HELP\r\n"
			"214 End of list.\r\n";
	const char nohelp[] = "214 There is no help for that command.\r\n";

	if (!strlen(args)) {
		_response(help);
	} else if (!strcasecmp(args, "SITE")) {
		_response(helpsite);
	} else {
		_response(nohelp);
	}
	return 0;
}

/*reply to wrong or unsupported commands*/
int failed(const char *s)
{
	char msg500[MAX_PATH] = {0};
	int m_len;

	if ((m_len = snprintf(msg500, MAX_PATH, 
			"500 \'%s\' command is not supported.\r\n", s)) >= MAX_PATH || m_len == -1) {
		write_log("Too long command got.", 0);
		return -1;
	}
	_response(msg500);
	return 0;
}
/*implement of NLST*/
int do_nlst(const char *path)
{
	int i = 0;
	const char finish[] = "226 Transfer complete.\r\n";
	const char success[] = "150 Opening ASCII mode data connection for file list.\r\n";
	const char fail[] = "450 No such file or directory.\r\n";
	struct stat stat_buf;
	DIR *target_dir;
	struct dirent *direntp;
	char buf[BUF_LEN] = {0};
	char **words;
	wordexp_t wxp;

	if (path[0]!='/') {
		if ((user_env.current_path[strlen(user_env.current_path) - 1]) == '/') {
			snprintf(buf, BUF_LEN, "%s%s", user_env.current_path, path);
		} else {
			snprintf(buf, BUF_LEN, "%s/%s", user_env.current_path, path);
		}
	}
	else {
		snprintf(buf, BUF_LEN, "%s", path);
	}

	debug_printf("buf=\'%s\'\n", buf);

	wordexp(buf, &wxp, 0);
	words = wxp.we_wordv;

	if (wxp.we_wordc < 1) {
		_response(fail);
		goto end;	
	}

	_response(success);

	if (wxp.we_wordc == 1) {
		if (stat(words[0], &stat_buf) == -1) {
			_response(fail);
			goto end;
		}
		else {
			if (S_ISDIR(stat_buf.st_mode)) {
				_path_tail(buf, words[0]);
				if((target_dir = opendir(buf)) == NULL) {
					goto end;
				}
				while ((direntp = readdir(target_dir)) != NULL && direntp->d_name[0] != '.') {
					write(user_env.data_fd, direntp->d_name, strlen(direntp->d_name));
					write(user_env.data_fd, "\n", 1);
				}
				goto suc;
			}
		}
	}

	for (i = 0; i < (int)wxp.we_wordc; i++) {
		if (words[i][0] != '.') {
			write(user_env.data_fd, words[i], strlen(words[i]));
			write(user_env.data_fd, "\n", 1);
		}
	}
	
suc:	_response(finish);

end:	close(user_env.data_fd);
	wordfree(&wxp);
	return 0;
}

static void _path_tail(char *buf, char *name)
{
	int flag = 1;
	int i = 0;

	if ((strlen(buf) + strlen(name)) >= BUF_LEN) {
		flag = 0;
	}

	for (i = strlen(buf); i >= 0; i--) {
		if (buf[i] == '/') {
			buf[i] = '\0';
			break;
		}
	}

	if (flag) {
		strcat(buf, name);   /*It's safe*/
	}
}
/*end of implement NLST*/
