/*文件名:do_stat.c
 * 作    者：刘洋
 * 日    期：2007年6月3日
 * 摘    要：实现STAT命令，并且附带单独测试do_stat函数的代码
 * 修 改 者：刘洋
 * 修改日期：2007年6月11日
 * 修改内容：改变xylftp.h的包含路径,检查每个snprintf()的返回值。使目录列表能够基本对齐。因为调用接口同意使用user_cmd
 *           而不是user_cmd.arg，故改之。user_cmd原类型为struct p_cmd 改为struct parse_cmd
 */

#include "xylftp.h"

#ifndef TEST
#define TEST
#endif

#define MAX_CMD 5
#define MAX_ARG 4096
#define MAX_PATH 4096

#ifdef TEST
struct user_env user_env;
struct run_env run_env;
#else
extern struct user_env user_env;
extern struct run_env run_env;
#endif

static char* _get_line_info(struct stat *stat_buf,char *buf,int *width)
{
	char att[11] = "----------\0",tm[26];
	struct passwd *pwd = getpwuid(stat_buf->st_uid);
	struct group *grp = getgrgid(stat_buf->st_gid);
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
	if (snprintf(tm,25,"%s",ctime(&(stat_buf->st_ctime))) != 25) {
#ifdef TEST
		perror("Time error!");
#else
		write_log("Read system time error!",0);
#endif
	}
	if (snprintf(buf,MAX_PATH,"%s% *d %*s %*s% *d %s",att,width[2],(size_t)stat_buf->st_nlink,width[0],
				pwd->pw_name,width[1],grp->gr_name,width[3],(size_t)stat_buf->st_size,tm) == -1){
#ifdef TEST
		perror("Path length is overflow!");
#else
		write_log("The path string is overflow!",0);
#endif
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

static int _stat_no_arg()
{
	char msg[MAX_MSG_LEN];
	if (snprintf(msg,MAX_MSG_LEN,"211-Status for user %s from %s:\r\n"
			"211-Stored %d files,%d KB\r\n211-Retrieved %d files,%d KB\r\n211 End of Status.\r\n",
			user_env.user_name,user_env.client_ip,user_env.upload_files,
			user_env.upload_kbytes,user_env.download_files,user_env.download_kbytes) == -1) {
#ifdef TEST
		perror("the message is overflow.");
#else
		write_log("The message is overflow.",0);
#endif
	}
	return write(user_env.connect_fd,msg,strlen(msg));
}

static int _stat_with_arg(const char *cmd_arg)
{
	char buf[MAX_PATH],full_path[MAX_PATH],tmp[MAX_PATH];
	char *not_found = "450 Target path DON'T exist!\r\n";
	struct dirent *direntp;
	DIR *target_dir;
	struct stat stat_buf;
	struct passwd *pwd;
	struct group *grp;
	int max_width[4]; /*max length of user name, group name,link number and the file length number*/

	if (*cmd_arg == '/') {
		if (snprintf(buf,MAX_PATH,"%s%s",run_env.ftp_dir,cmd_arg) == -1) {
#ifdef TEST
			perror("Path string is overflow.");
#else
			write_log("Path string is overflow.",0);
#endif
		}
	}
	else {
		if (snprintf(buf,MAX_PATH,"%s%s%s",run_env.ftp_dir,user_env.current_path,cmd_arg) == -1) {
#ifdef TEST
			perror("Path string is overflow.");
#else
			write_log("Path string is overflow.",0);
#endif
		}
	}

	strcpy(full_path,buf); /*the length of buf won't greater than the full_path so it's safe.*/

	if (!(target_dir = opendir(buf))) {
		write(user_env.connect_fd,not_found,strlen(not_found));
		return closedir(target_dir);
	}

	while ((direntp = readdir(target_dir)) != NULL) {
		int t;
		
		if (snprintf(buf,MAX_PATH,"%s/%s",full_path,direntp->d_name) == -1) {
#ifdef TEST
			perror("Path string is overflow.");
#else
			write_log("Path string is overflow.",0);
#endif
		}

		if (stat(buf,&stat_buf) == -1) {
#ifdef TEST
			perror("Read local file status error");
#else
			write_log("Read local file status error",0);
#endif
			closedir(target_dir);
			return -1;
		}

		pwd  = getpwuid(stat_buf.st_uid);
		grp = getgrgid(stat_buf.st_gid);

		max_width[0] = (t = strlen(pwd->pw_name)) > max_width[0]?t:max_width[0];
		max_width[1] = (t = strlen(grp->gr_name)) > max_width[1]?t:max_width[1];
		max_width[2] = (t = _get_int_len(stat_buf.st_nlink)) > max_width[2]?t:max_width[2];
		max_width[3] = (t = _get_int_len(stat_buf.st_size)) > max_width[3]?t:max_width[3];
	}
	rewinddir(target_dir);

	if (snprintf(buf,MAX_PATH,"211-Status of %s%s\r\n",user_env.current_path,cmd_arg) == -1) {
#ifdef TEST
		perror("Path string is overflow.");
#else
		write_log("Path string is overflow.",0);
#endif
	}
		
	write(user_env.connect_fd,buf,strlen(buf));

	while ((direntp = readdir(target_dir)) != NULL) {
		if (*(direntp->d_name) == '.') {
			continue;
		}
		if (snprintf(buf,MAX_PATH,"%s/%s",full_path,direntp->d_name) == -1) {
#ifdef TEST
			perror("Path string is overflow.");
#else
			write_log("Path string is overflow.",0);
#endif
		}

		if (stat(buf,&stat_buf) == -1) {
#ifdef TEST
			perror("Read local file status error");
#else
			write_log("Read local file status error",0);
#endif
			closedir(target_dir);
			return -1;
		}
		if (snprintf(buf,MAX_PATH,"211-%s %s\r\n",_get_line_info(&stat_buf,tmp,max_width),direntp->d_name) == -1) {
#ifdef TEST
			perror("Path string is overflow.");
#else
			write_log("Path string is overflow.",0);
#endif
		}
		write(user_env.connect_fd,buf,strlen(buf));
	}

	if (snprintf(buf,MAX_PATH,"211 End of Status.\r\n") == -1) {
#ifdef TEST
		perror("Path string is overflow.");
#else
		write_log("Path string is overflow.",0);
#endif
	}
	write(user_env.connect_fd,buf,strlen(buf));
	return closedir(target_dir);
}

int do_stat(const char *cmd_arg)
{
	if (!strlen(cmd_arg)) {
		return _stat_no_arg();
	}

	return _stat_with_arg(cmd_arg);
}

#ifdef TEST

int main(void)
{
	strcpy(user_env.client_ip, "192.168.0.1");
	strcpy(user_env.user_name,"Anonymous");
	user_env.upload_files = 1;
	user_env.upload_kbytes = 10;
	user_env.download_files = 0;
	user_env.download_kbytes = 0;
	user_env.connect_fd = 0;
	strcpy(user_env.current_path,"/");
	strcpy(run_env.ftp_dir,"/");
	return do_stat("tmp");
}  
#endif /*end the TEST. the main() for test the functions only should be compiled when it tested*/
/*This main function just define the required value for tests*/
