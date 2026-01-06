/*
 * Copyright (c) 2007,西安邮电学院Linux兴趣小组
 * All rights reserved.
 *
 * 文件名称：read_configure.c
 * 摘要：读取配置文件内容,测试使用。
 *
 * 当前版本：0.1
 * 作者：林峰
 * 完成日期：2007年5月15日
 *
 * 修改者：林峰
 * 修改日期：2007年6月1日
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "xylftp.h"

#define CONFIG_NUM 13	/*item number of the configue file*/
struct run_env run_env;

int _read_line(int fd,char *buf);
int _analyze_para(char *buf,int *array);
int _check_configure(int *array);
#ifdef DEBUG
void print(void);

void print(void)
{
	printf("run_env.anonymous_enable = %d\n",run_env.anonymous_enable);
	printf("run_env.ftp_port = %d\n",run_env.ftp_port);
	printf("run_env.local_umask = %d\n",run_env.local_umask);
	printf("run_env.log_file_enable = %d\n",run_env.log_file_enable);
	printf("run_env.log_file = %s\n",run_env.log_file);
	printf("run_env.idle_session_timeout = %d\n",run_env.idle_session_timeout);
	printf("run_env.data_connection_timeout = %d \n",run_env.data_connection_timeout);
	printf("run_env.ftpd_banner = %s\n",run_env.ftpd_banner);
	printf("run_env.max_clients = %d\n",run_env.max_clients);
	printf("run_env.max_connections = %d\n",run_env.max_connections);
	printf("run_env.passive_port = %d,%d\n",run_env.passive_port_min,run_env.passive_port_max);
	printf("run_env.ftp_dir = %s\n",run_env.ftp_dir);
	printf("run_env.user_pass_file = %s\n",run_env.user_pass_file);
}
#endif

int _analyze_para(char *buf,int *array)
{
	char *tmp;
	char *option = NULL,*value = NULL;			/*used to store the option strings and value strings*/
	char *saveptr1,*saveptr2;		/*used for the function strtok_r*/
	if ((tmp = strtok_r(buf,"=",&saveptr1)) == NULL) {
		fprintf(stderr,"error in configure file\n");
		return -1;
	}		/*get the option string*/
	if ((option = malloc(sizeof(char) * (strlen(tmp) + 1))) == NULL) {
		fprintf(stderr,"Not enough memory!\n");
		goto ret;
	}	
	strcpy(option,buf);			/*store option*/
	if((tmp = strtok_r(NULL,"=",&saveptr1)) == NULL) {
		fprintf(stderr,"error in configure file\n");
		return -1;
	}
	if ((value = malloc(sizeof(char) * ((strlen(tmp) + 1)))) == NULL) {
		fprintf(stderr,"Not enough memory!\n");
		goto ret;
	}
	strcpy(value,tmp);			/*store value*/

/*codes below are used to value the parameters of the struct run_env*/
	if (!strcmp(option,"Anonymous_enable")) {
		if (!strcmp(value,"YES")) {
			run_env.anonymous_enable = TRUE;	
		} else {
			run_env.anonymous_enable = FALSE;
		}
		array[0] = 1;
	} else if (!strcmp(option,"FTP_port")) {
		run_env.ftp_port = (unsigned int)(atoi(value));
		array[1] = 1;
	} else if (!strcmp(option,"Local_umask")) {
		run_env.local_umask = (unsigned int)(atoi(value));
		array[2] = 1;
	} else if (!strcmp(option,"Log_file_enable")) {
		if (!strcmp(value,"YES")) {
			run_env.log_file_enable = 1;
		} else {
			run_env.log_file_enable = 0;
		}
		array[3] = 1;
	} else if (!strcmp(option,"Log_file")) {
		if ((run_env.log_file = malloc((strlen(value) + 1) * sizeof (char))) == NULL) {
			fprintf(stderr,"Not enough memory!\n");
			goto ret;
		}
		strcpy(run_env.log_file,value);
		array[4] = 1;
	} else if (!strcmp(option,"Idle_session_timeout")) {
		run_env.idle_session_timeout = (unsigned int)(atoi(value));
		array[5] = 1;
	} else if (!strcmp(option,"Data_connection_timeout")) {
		run_env.data_connection_timeout = (unsigned int)(atoi(value));
		array[6] = 1;
	} else if (!strcmp(option,"Ftpd_banner")) {
		if ((run_env.ftpd_banner = malloc((strlen(value) + 1) * sizeof (char))) == NULL) {
			fprintf(stderr,"Not enough memory!\n");
			return -1;
		}
                strcpy(run_env.ftpd_banner,value);
		array[7] = 1;
	} else if (!strcmp(option,"Max_clients")) {
		run_env.max_clients = (unsigned int)(atoi(value));
		array[8] = 1;
	} else if (!strcmp(option,"Max_connections")) {
		run_env.max_connections = (unsigned int)(atoi(value));
		array[9] = 1;
	} else if (!strcmp(option,"Passive_port")) {
		if ((tmp = strtok_r(value,",",&saveptr2)) == NULL) {
			fprintf(stderr,"error in configure file\n");
			return -1;
		}
		run_env.passive_port_min = (unsigned int)(atoi(tmp));		/*get the min_port and store it*/
		if ((tmp = strtok_r(NULL,",",&saveptr2)) == NULL) {
			fprintf(stderr,"error in configure file\n");
			return -1;
		}								/*get the max_port and store it*/
		run_env.passive_port_max = (unsigned int)(atoi(tmp));
		array[10] = 1;
	} else if (!strcmp(option,"FTP_dir")) {
		if ((strlen(value) + 1) > PATH_NAME_LEN) {
			fprintf(stderr, "Path name is too long!\n");
			return -1;
		}
                strcpy(run_env.ftp_dir,value);
		array[11] = 1;
	} else if (!strcmp(option,"User_pass_file")) {
		if ((run_env.user_pass_file = malloc((strlen(value) + 1) * sizeof (char))) == NULL) {
			fprintf(stderr,"Not enough memory!\n");
			goto ret;
		}
                strcpy(run_env.user_pass_file,value);
		array[12] = 1;
	} else {
		fprintf(stderr,"Parameter can't be analyzed!\n");
		goto ret;
	}
/*codes above are used to value the parameters of the struct run_env*/

	return 0;
ret:
	free(run_env.user_pass_file);	/*free the memory*/
	free(run_env.log_file);		/*free the memory*/
	free(value);			/*free the memory*/
	free(option);			/*free the memory*/
	return -1;
}


int read_configure()
{
	int fd;			
	char buffer[_BUFFER_LENGTH];
	int i;
	int config_array[CONFIG_NUM];
	
	for (i = 0;i < CONFIG_NUM;i++) {
		config_array[i] = 0;
	}
		
	i = 0;
	if ((fd = open(CONFIG_FILE,O_RDONLY)) == -1) {		/*open the configure file error*/
		fprintf(stderr,"can't open the configure file,please use the tool xylftppass to solve this problem!\n");
		return -1;
	} else {
		i = _read_line(fd,buffer);			/*get an valuable line from the configure file*/
		#ifdef DEBUG
		while (i != EOF ) {
			
			if (i == 1) {
			printf("%s\n",buffer);
			fflush(stdout);
			_analyze_para(buffer,config_array);				/*analyze the line got from the file*/
			}
			i = _read_line(fd,buffer);
		}
		print();
		#endif
		close(fd);
	}

	if (_check_configure(config_array) == -1) {
		/*write_log("configure file error!",0);*/
		fprintf(stderr,"configure file error!");
		return -1;
	}
	return 0;
}

int _read_line(int fd,char *buf)
{
	char ch;
	int i = 0,j;
	
	if ((j = read(fd,&ch,1)) == 0) {		/*end of the file*/
		return EOF;
	}		
	

	
	while (ch != '\n' && j != 0 ){		/*if the character we got is not an end of a line or the end of the file*/
		if (ch == '#' ) {							/*if it is a comment line,skip it over*/
			while (ch != '\n' ) {
				if ((j = read(fd,&ch,1))  == 0) {
					break;	
				}
			}
			j = read(fd,&ch,1);
		} else if (ch == ' ') {					/*if it is a space,skip to the next character*/
			if (i != 0) {
				buf[i++] = ch;
			}
			j = read(fd,&ch,1);
		} else if (j != 0){					/*a valuable character,store it*/
			buf[i++] = ch;
			j = read(fd,&ch,1);
		}
	}

	if ( j == 0 ) {							/*end of the line,also the end of the file,return EOF*/
		buf[i] = '\0';
		return EOF;
	} else if (i != 0) {							/*end of an valuable line*/
		buf[i] = '\0';
		return 1;
	}
	return 0;
}

int _check_configure(int *array)
{
	int i = 0;
	if(!array[0]) {
		fprintf(stderr,"no Anonymous_enable item or error\n");
	} 
	if (!array[1]) {
		fprintf(stderr,"no FTP_port item or error\n");
	}
	if (!array[2]) {
		fprintf(stderr,"no Local_umask item or error\n");
	}
	if (!array[3]) {
		fprintf(stderr,"no Log_file_enable item or error\n");
	}
	if (!array[4]) {
		fprintf(stderr,"no Log_file item or error\n");
	}
	if (!array[5]) {
		fprintf(stderr,"no Idle_session_timeout item or error\n");
	}
	if (!array[6]) {
		fprintf(stderr,"no Data_connection_timeout item or error\n");
	}
	if (!array[7]) {
		fprintf(stderr,"no Ftpd_banner item or error\n");
	}
	if (!array[8]) {
		fprintf(stderr,"no Max_clients item or error\n");
	}
	if (!array[9]) {
		fprintf(stderr,"no Max_connections item or error\n");
	}
	if (!array[10]) {
		fprintf(stderr,"no Passive_port item or error\n");
	}
	if (!array[11]) {
		fprintf(stderr,"no FTP_dir item or error\n");
	}
	if (!array[12]) {
		fprintf(stderr,"no User_pass_file item or error\n");
	}

	for (i = 0;i < CONFIG_NUM;i++) {
		if (array[i] == 0) {
			return -1;
		}
	}

	return 0;
}

int main(void)
{
	read_configure();
	return 0;
}
