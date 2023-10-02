#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<cstdio>
#include<iostream>
#include<WinSock2.h>           //头文件
#pragma comment(lib,"WS2_32.lib")    //库文件
#define PRINTF(str) printf("[%s - %d]"#str"=%s\n",__func__,__LINE__,str);
#include<sys/types.h>
#include<sys/stat.h>



using namespace std;

void error_die(const char* str)
{
	perror(str);
	exit(1);
}


int startup(unsigned short* port)
{    //1.windows 网络通信初始化
	WSADATA data;         //数据存在这
	int  ret = WSAStartup(MAKEWORD(1, 1), &data);   //2.2版本协议
	if (ret)
	{
		error_die("WSAStartup");
	}
	//2. 创建套接字
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_socket == -1)
	{//打印错误提示，结束
		error_die("套接字");
	}


	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(*port);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		error_die("connect");
	}
	
	return client_socket;
}

int main()
{
	unsigned short port = 8000;
	cout << "发送端口：";
	cin >> port;
	SOCKET client_sock = startup(&port);
	char buffsend[4096];
	char buffrecv[4096];
	memset(buffsend, 0, sizeof(buffsend));
	memset(buffrecv, 0, sizeof(buffrecv));
	while (1)
	{
		cout << "input:" << endl;
		cin >> buffsend;
		if (send(client_sock, buffsend, sizeof(buffsend), 0) ) 
		{
			cout << "客户端输入：" << buffsend << endl;

		}
		else
		{
			error_die("send");
		}
		if (recv(client_sock, buffrecv, sizeof(buffrecv), 0))
		{
			cout << "服务端返回：" << buffrecv << endl;

		}
		else
		{
			error_die("recv");
		}
	}
	closesocket(client_sock);
	return 0;
}