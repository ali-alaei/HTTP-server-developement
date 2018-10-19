#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

char webpage[]=
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>Our Sample Page</title><link rel=\"icon\" href=\"testicon.ico\">\r\n"
"<style>body {background-color: #FFFF00}</style></head>\r\n"
"<body><center><h1>Hello World!</h1><br>\r\n"
"<img src=\"testpic.jpg\"></center></body></html>\r\n";

int main(int argc, char *argv[])
{
	struct sockaddr_in server_addr, client_addr;
	socklen_t sin_len = sizeof(client_addr);
	int fd_server, fd_client;
	char buf[2048];
	int fdimg;
	//int on = 1;

	fd_server = socket(AF_INET, SOCK_STREAM, 0);
	if(fd_server < 0)
	{
		perror("socket");
		exit(1);
	}

	//setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(8080);

	if(bind(fd_server, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind");
		close(fd_server);
		exit(1);
	}

	if(listen(fd_server,10) == -1)
	{
		perror("listen");
		close(fd_server);
		exit(1);
	}

	while(1)
	{		
		fd_client = accept(fd_server, (struct sockaddr *) &client_addr, &sin_len);
		if(fd_client == -1)
		{
			perror("Connection Failed! Can't Conneect to Client .... \n");
			continue;
		}
		printf("Accepted the Client Connection ..... \n");

		if(!fork())
		{			
			/* Child Process */
			close(fd_server);
			memset(buf, 0, 2047);
			read(fd_client, buf, 2047);

			printf("%s\n", buf);

			if(!strncmp(buf, "GET /testicon.ico", 16))
			{
				fdimg = open("testicon.ico", O_RDONLY);
				sendfile(fd_client, fdimg, NULL, 200000);
				close(fdimg);
			}
			else if(!strncmp(buf, "GET /testpic.jpg", 16))
			{
				fdimg = open("testpic.jpg", O_RDONLY);
				sendfile(fd_client, fdimg, NULL, 60000);
				close(fdimg);	
			}
			else
				write(fd_client, webpage, sizeof(webpage)-1);
			close(fd_client);
			printf("Closing ... \n");
			exit(0);
		}
		/* Parent Process */
		close(fd_client);
	}	
	return 0;	
}