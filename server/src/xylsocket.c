/* All rights reserved.
 *
 * 文件名称：xylsocket.c
 * 摘    要：对socket函数进行封装
 * 当前版本：1.0
 * 作    者：董溥
 * 完成日期：2007年5月13日
 *
 *修改人员：林峰
 *修改日期：2007年6月6日
 *修改：王聪
 *修改日期：2007.8.4
 */

#include "xylftp.h"
#include "debug.h"

extern struct user_env user_env;
extern struct run_env run_env;

extern void telnet(void);

int r_close(int fd)
{
	int state;
	while ((state = close(fd)) == -1 && errno == EINTR) {
		continue;
	}
	return state;
}
void listen_connect(void)
{
	int socket_fd;
	int connect_fd;
	pid_t child_pid;
	#ifdef 	DEBUG
	run_env.ftp_port = 1986;
	#endif
	if ((socket_fd = xyl_listen(run_env.ftp_port)) == -1) {
		exit(1);
	}
	while (1) {
		if ((connect_fd = xyl_accept(socket_fd)) == -1) {
			if (r_close(socket_fd) == -1) {
				LOG_IT("close error.");
			}
			exit(1);
		}	
		user_env.connect_fd = connect_fd;	

		debug_printf("run to fork.\n");

		signal(SIGCLD,SIG_IGN);		/*to avoid zombie process*/

		if ((child_pid = fork()) == -1) {
			LOG_IT("fork error.");
		}
		else if (child_pid) {			/*parent*/
			if (r_close(user_env.connect_fd) == -1) {
				LOG_IT("close error.");
			}
			continue;
		}
		else if (child_pid == 0) {
			if (r_close(socket_fd) == -1) {
				LOG_IT("close error.");
			}
			
			telnet();

			if (r_close(user_env.connect_fd) == -1) {
				LOG_IT("close connect_fd error.");
			}
			exit(0);
		}
	}
}
int xyl_listen(short port) 
{
	int socket_fd;
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));	/*initial socket address to zero*/
	server_addr.sin_family = AF_INET;			/*use IPv4*/
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	/*use localhost IP*/
	server_addr.sin_port = htons(port);			/*convert from host byte order to network byte order*/

	if ((socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
		LOG_IT("socket error.");
		if (errno == ENFILE){
			LOG_IT("The  system  limit  on  the  total number of open files has been reached.");
		}
		else if (errno == EPROTONOSUPPORT) {
			LOG_IT("The  protocol  type  or  the specified protocol is not supported within this domain.");
		}
		else if (errno == EACCES) {
			LOG_IT("Permission  to create a socket of the specified type and/or protocol is denied.");
		return -1;
		}
	}

	if ((bind(socket_fd,(struct sockaddr *) &server_addr,sizeof(server_addr)) == -1)) {
		LOG_IT("bind error.");
		if (errno == EADDRINUSE) {
			LOG_IT("The given address is already in use.");
		}
		else if (errno == EBADF) {
			LOG_IT("sock_fd is not a valid descriptor.");
		}
		else if (errno == EACCES) {
			LOG_IT("The address is protected.");
		}
		if (r_close(socket_fd) == -1) {
			LOG_IT("close socket_fd error.");
		}
		return -1;
	}

	if (listen(socket_fd,LISTENQ) == -1) {
		LOG_IT("listen error.");
		if (errno == EADDRINUSE) {
			LOG_IT("Another socket is already listening on the same port.");
			return -1;
		}
	}

	return socket_fd;
}


int xyl_accept(int socket_fd)
{
	int connect_fd;
	socklen_t len;
	struct sockaddr_in client_addr;
	while (1) {
		len = sizeof(client_addr);
		if ((connect_fd = accept(socket_fd, (struct sockaddr *)&client_addr,&len)) == -1
				&& errno == EINTR) {
			continue;
		}
		if (connect_fd == -1) {
			LOG_IT("accept error.");
			if (errno == ECONNABORTED) {
				LOG_IT("A connection has been aborted.");
			}
			else if (errno == EPERM) {
				LOG_IT("Firewall rules forbid connection.");
			}
			else if (errno == ENFILE) {
				LOG_IT("The system limit on the total number of open files has been reached.");
			}
			else if (errno == EINTR) {
				LOG_IT("The system call was interrupted by a signal that was" 
					"caught before a valid connection arrived.");
			}
		}
		break;
	}
	strcpy(user_env.client_ip, inet_ntoa(client_addr.sin_addr));
	return connect_fd;
}


int xyl_connect(char *hostname,short port)
{
	int socket_fd;
	int ret;
	struct sockaddr_in server_addr;
	socklen_t len;

	server_addr.sin_port = htons(port);
	server_addr.sin_family = AF_INET;
	if ((inet_aton(hostname, &server_addr.sin_addr)) == 0) {
		LOG_IT("invalid address.");
		return -1;
	}
	/*bzero(&(server_addr.sin_zero),8);*/

	if ((socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
		LOG_IT("socket error.");	
		if (errno == ENFILE){
			LOG_IT("The  system  limit  on  the  total number of open files has been reached.");
		}
		else if (errno == EPROTONOSUPPORT) {
			LOG_IT("The  protocol  type  or  the specified protocol is not supported within this domain.");
		}
		else if (errno == EACCES) {
			LOG_IT("Permission  to create a socket of the specified type and/or protocol is denied.");
		}
		return -1;

	}	

	while (1) {
		len = sizeof(server_addr);
		if (((ret = connect(socket_fd, (struct sockaddr *)&server_addr, len)) == -1
				&& errno == EINTR) || errno == EALREADY) {
			continue;
		}
		if (ret == -1) {
			LOG_IT("connect error.");
			if (errno == ENETUNREACH) {
				LOG_IT("Network is unreachable.");
			}	
			else if (errno == ETIMEDOUT) {
				LOG_IT("Timeout while attempting connection.");
			}
			else if (errno == EISCONN) {
				LOG_IT("The socket is already connected.");
			}
			else if (errno == ECONNREFUSED) {
				LOG_IT("No one listening on the remote address.");
			}	
			if (r_close(socket_fd) == -1) {
				LOG_IT("close socket_fd error.");
			}
			return -1;
		}
		break;
	}
	return socket_fd;
}



