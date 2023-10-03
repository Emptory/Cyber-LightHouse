#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<cstdio>
#include<iostream>
#include<WinSock2.h>           //ͷ�ļ�
#pragma comment(lib,"WS2_32.lib")    //���ļ�
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
	
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_socket == -1)
	{
		error_die("�׽���");
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

int startUDP(unsigned short* port)
{    
	WSADATA data;
	int  ret = WSAStartup(MAKEWORD(2, 2), &data); 
	if (ret)
	{
		error_die("WSAStartup");
	}
	
	SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (client_socket == -1)
	{
		error_die("�׽���");
	}

	return client_socket;
}

int main()
{
	unsigned short port = 8000;
	int conn = 0;
	cout << "���Ͷ˿ڣ�";
	cin >> port;
	cout << "���ӷ�ʽ��1.TCP  2.UDP"<<endl;
	cin >> conn;
	if (conn == 1)
	{
		while (1)
		{
			SOCKET client_sock = startTCP(&port);
			char buffsend[4096];
			char buffrecv[4096];
			memset(buffsend, 0, sizeof(buffsend));
			memset(buffrecv, 0, sizeof(buffrecv));
			while (1)
			{
				cout << "input:" << endl;
				cin >> buffsend;
				if (send(client_sock, buffsend, sizeof(buffsend), 0))
				{
					cout << "�ͻ������룺" << buffsend << endl;
					memset(buffsend, 0, sizeof(buffsend));
				}
				else
				{
					error_die("send");
				}
				if (recv(client_sock, buffrecv, sizeof(buffrecv), 0))
				{
					cout << "����˷��أ�" << buffrecv << endl;
					memset(buffrecv, 0, sizeof(buffrecv));
				}
				else
				{
					error_die("recv");
				}
			}
		}
		
	}
	else {
		if (conn ==2)
		{
			while (1)
			{

				SOCKET client_sock = startUDP(&port);
				char buffsend[255];
				char buffrecv[255];
				memset(buffsend, 0, sizeof(buffsend));
				memset(buffrecv, 0, sizeof(buffrecv));
				struct sockaddr_in server_addr;
				int len_server = sizeof(server_addr);
				memset(&server_addr, 0, sizeof(server_addr));
				server_addr.sin_family = AF_INET;
				server_addr.sin_port = htons(port);
				server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

				if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
				{
					error_die("connect");
				}
				while (1)
				{
					cout << "input:" << endl;
					cin >> buffsend;
					if (sendto(client_sock, buffsend, sizeof(buffsend), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)))
					{
						cout << "�ͻ������룺" << buffsend << endl;
						memset(buffsend, 0, sizeof(buffsend));
					}
					else
					{
						error_die("send");
					}
					if (recvfrom(client_sock, buffrecv, 255, 0, (struct sockaddr*)&server_addr, &len_server))
					{
						cout << "����˷��أ�" << buffrecv << endl;
						memset(buffrecv, 0, sizeof(buffrecv));
					}
					else
					{
						error_die("recv");
					}
				}
            
			}
		}else
        {
			cout << "��֧�ִ�������";
			error_die("connect method");
		}

	}
	
	return 0;
}