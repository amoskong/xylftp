#define _GNU_SOURCE
#include "xylftp.h"

struct user_env user_env;
struct run_env run_env;

/*implement of PASS*/
int do_pass(char *pass)
{
	char mess[50];
	char password[16];
	char md[16];
	char name[16];
	const char log_error[] = "Login failed!\r\n";
	const char logged[] = "503 You have already logged in!\r\n";
	FILE *fp;
	size_t len = 0;
	ssize_t k;
	char *line = NULL, *tmp;
	int i, j, pass_len;

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

