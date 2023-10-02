#define _CRT_SECURE_NO_WARNINGS

#include<cstdio>
#include<iostream>
#include<WinSock2.h>           //ͷ�ļ�
#pragma comment(lib,"WS2_32.lib")    //���ļ�
#define PRINTF(str) printf("[%s - %d]"#str"=%s\n",__func__,__LINE__,str);
#include<sys/types.h>
#include<sys/stat.h>



using namespace std;
//�׽���
// 

void error_die(const char* str)
{
	perror(str);
	exit(1);
}


int startup(unsigned short* port)
{    //1.windows ����ͨ�ų�ʼ��
	WSADATA data;         //���ݴ�����
	int  ret = WSAStartup(MAKEWORD(1, 1), &data);   //1.1�汾Э��
	if (ret)
	{
		error_die("WSAStartup");
	}
	//2. �����׽���
	int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == -1)
	{//��ӡ������ʾ������
		error_die("�׽���");
	}

	//���ö˿ڿɸ���
	int opt = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
	if (ret == -1)
	{
		error_die("setsockopt");
	}
	//���÷����������ַ
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(*port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);


	//���׽��֣�
	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		error_die("bind");
	}
	//��̬����
	int nameLen = sizeof(server_addr);

	if (*port == 0)
	{
		if (getsockname(server_socket, (struct sockaddr*)&server_addr, &nameLen) < 0)
		{
			error_die("getsockname");
		}

		*port = server_addr.sin_port;
	}

	//������������ 
	if (listen(server_socket, 5) < 0)
	{
		error_die("listen");
	}

	return server_socket;
}


// \r\n

int get_line(int sock, char* buff, int size) //  ���ض�ȡ�����Լ���
{
	char c = 0;
	int i = 0;
	while (i < size && c != '\n')
	{
		int n = recv(sock, &c, 1, 0);
		if (n > 0)
		{
			if (c == '\r')   //�������
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
	//������Ӧ��ͷ��Ϣ
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
		printf("��Դ�������\n");
	}
	fclose(resouce);
}



//�����û�������̺߳���
DWORD WINAPI accept_request(LPVOID arg)
{
	char buff[1024];
	int client = (SOCKET)arg; //�ͻ����׽���
	int numchars = get_line(client, buff, sizeof(buff));
	PRINTF(buff);

	char method[255];   //GET POST
	int j = 0;
	int i = 0;
	//�Ȳ�Ϊ�հ��ַ���Ҳ��Խ��
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
	//��Դ·��
	//GET ·�� HTTP/1.1 \n
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