#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<cstdio>
#include<iostream>
#include<WinSock2.h>     
#pragma comment(lib,"WS2_32.lib")  
#define PRINTF(str) printf("[%s - %d]"#str"=%s\n",__func__,__LINE__,str);
#include<sys/types.h>
#include<sys/stat.h>

using namespace std;

void error_die(const char* str)
{
	perror(str);
	exit(1);
}


int startTCP(unsigned short* port)
{   
	WSADATA data; 
	int  ret = WSAStartup(MAKEWORD(2, 2), &data); 
	if (ret)
	{
		error_die("WSAStartup");
	}
	
	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == -1)
	{
		error_die("套接字");
	}

	
	int opt = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
	if (ret == -1)
	{
		error_die("setsockopt");
	}
	
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(*port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		error_die("bind");
	}

	int nameLen = sizeof(server_addr);

	if (*port == 0)
	{
		if (getsockname(server_socket, (sockaddr*)&server_addr, &nameLen) < 0)
		{
			error_die("getsockname");
		}

		*port = server_addr.sin_port;
	}

	if (listen(server_socket, 5) < 0)
	{
		error_die("listen");
	}

	return server_socket;
}


int startUDP(unsigned short* port)
{    
	WSADATA data;        
	int  ret = WSAStartup(MAKEWORD(2, 2), &data);  
	if (ret)
	{
		error_die("WSAStartup");
	}

	SOCKET server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (server_socket == -1)
	{
		error_die("套接字");
	}

	int opt = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
	if (ret == -1)
	{
		error_die("setsockopt");
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(*port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);


	//绑定套接字，
	if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		error_die("bind");
	}


	int nameLen = sizeof(server_addr);

	if (*port == 0)
	{
		if (getsockname(server_socket, (sockaddr*)&server_addr, &nameLen) < 0)
		{
			error_die("getsockname");
		}

		*port = server_addr.sin_port;
	}

	return server_socket;
}

// \r\n

int get_line(int sock, char* buff, int size) //  返回读取到的自己数
{
	char c = 0;
	int i = 0;
	while (i < size && c != '\n')
	{
		int n = recv(sock, &c, 1, 0);
		if (n > 0)
		{
			if (c == '\r')   //极端情况
			{
				recv(sock, &c, 1, MSG_PEEK);
				if (n > 0 && c == '\n')
				{
					recv(sock, &c, 1, 0);
				}
				else
				{
					c = '\n';
				}
			}
			buff[i++] = c;
		}
		else
		{
			c = '\n';
		}
	}

	buff[i] = 0;
	return i;
}




void unimplement(int client)
{

}

void not_found(int client)
{

}

void headers(int client)
{
	//发送相应包头信息
	char buff[1024];
	strcpy(buff, "HTTP/1.1 200 OK\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "Server:Cyber_Lighthouse/0.1\r\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "Content-type:text/html\n");
	send(client, buff, strlen(buff), 0);

	strcpy(buff, "\r\n");
	send(client, buff, strlen(buff), 0);
}


void cat(int client, FILE* resource)
{
	char buff[4096];
	while (1)
	{
		int ret = fread(buff, sizeof(char), sizeof(buff), resource);
		if (ret <= 0)
		{
			break;
		}
		send(client, buff, strlen(buff), 0);
	}
}

void server_file(int client, const char* fileName)
{
	char numchars = 1;
	char buff[1024];

	while (numchars > 0 && strcmp(buff, "\n"))
	{
		numchars = get_line(client, buff, sizeof(buff));
		PRINTF(buff);
	}
	FILE* resouce = fopen(fileName, "r");
	if (resouce == NULL)
	{
		not_found(client);
	}
	else
	{
		headers(client);
		cat(client, resouce);
		printf("资源发送完毕\n");
	}
	fclose(resouce);
}



//处理用户请求的线程函数
DWORD WINAPI accept_request(LPVOID arg)
{
	char buff[1024];
	SOCKET client = (SOCKET)arg; //客户端套接字
	int numchars = get_line(client, buff, sizeof(buff));
	PRINTF(buff);

	char method[255];   //GET POST
	int j = 0;
	int i = 0;
	//既不为空白字符，也不越界
	while (!isspace(buff[j]) && j < (sizeof(method) - 1))
	{
		method[i++] = buff[j++];
	}
	method[i] = 0; // '\0'
	PRINTF(method);
	if (strcmp(method, "GET") && strcmp(method, "POST"))
	{
		unimplement(client);
		return 0;
	}
	//资源路径
	//GET 路径 HTTP/1.1 \n
	char url[255];
	i = 0;
	while (isspace(buff[j]) && j < (sizeof(method)))
	{
		j++;
	}
	while (!isspace(buff[j]) && i < (sizeof(url) - 1) && (j < sizeof(buff)))
	{
		url[i++] = buff[j++];
	}
	url[i] = 0;
	PRINTF(url);



	char path[512] = "";
	sprintf(path, "CLHdocs%s", url);
	if (path[strlen(path) - 1] == '/') 	strcat(path, "index.html");
	PRINTF(path);
	struct stat status;

	if (stat(path, &status) == -1)
	{
		while (numchars > 0 && strcmp(buff, "\n")) { numchars = get_line(client, buff, sizeof(buff)); }
		not_found(client);
	}
	else
	{
		if ((status.st_mode & S_IFMT) == S_IFDIR)
		{
			strcat(path, "/index.html");
		}
		server_file(client, path);

	}

	closesocket(client);
	return 0;
}


DWORD WINAPI accept_TCP(LPVOID arg)
{
	char buff[4096];
	SOCKET client = (SOCKET)arg;
	int TCPread, TCPsend;
	while (1)
	{ 
		TCPread = recv(client, buff, 4096, 0);
		if (TCPread > 0)
		{
			cout << "received:" << buff << endl;
			TCPsend = send(client, buff, strlen(buff), 0);
			if (TCPsend > 0)
			{
				memset(&buff, 0, sizeof(buff));
			}
			else 
			{
				error_die("TCPsend");
			}
		}
		else
		{
			error_die("TCPread");
		}
	}
	closesocket(client);
}


DWORD WINAPI accept_UDP(LPVOID arg)
{
	char buff[255];
	memset(&buff, 0, sizeof(buff));
	SOCKET server= (SOCKET)arg;
	struct sockaddr_in client_addr;
	int len_client = sizeof(client_addr);
	int UDPread, UDPsend;
	while (1)
	{
		UDPread = recvfrom(server, buff, 255, 0,(struct sockaddr *)&client_addr,&len_client);
		if (UDPread > 0)
		{
			cout << "接受到一个连接：%s \r\n" << inet_ntoa(client_addr.sin_addr)<<endl << buff << endl;
			UDPsend = sendto(server, buff, strlen(buff), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));

			if (UDPsend > 0)
			{
				cout << "UDP包发回" << endl;
				memset(&buff, 0, sizeof(buff));
			}
			else
			{
				error_die("UDPsend");
			}
		}
		else
		{
			error_die("UDPread");
		}
	}
	closesocket(server);
}





int main()
{
	unsigned short port = 8000;
	cout << "监听端口：";
	cin >> port;
	cout << "正在监听：" << port<<endl;
	SOCKET server_sock = startTCP(&port);
	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);

	while (1)
	{
		//阻塞式等待用户通过浏览器发起访问
		SOCKET client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_len);
		if (client_sock == -1)
		{
			error_die("accept");
		}
		else
		{
			cout << "accepted" << endl;
		}

		int type = SOCK_STREAM;
		int typeLen = sizeof(type);
		getsockopt(client_sock, SOL_SOCKET, SO_TYPE, (char *)&type, &typeLen);
		if (type != SOCK_STREAM) {  // 非TCP连接
			cout << "Invalid connection type" << endl;
			error_die("非法连接");
		}
		//使用client_sock对用户进行访问
		//这样只能同时服务一个客户端
		//创建新的线程，多线程并发
		//进程和线程

		//windows
		DWORD threadID = 0;
		CreateThread(0, 0, accept_TCP, (void*)client_sock, 0, &threadID);
     	//CreateThread(0, 0, accept_UDP, (void*)server_sock, 0, &threadID);
	}
    

	WSACleanup();
	closesocket(server_sock);
	return 0;
}