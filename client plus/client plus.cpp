#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<cstdio>
#include<iostream>
#include<WinSock2.h>
#pragma comment(lib,"WS2_32.lib")
#include <ctime>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

void error_die(const char* str)
{
    perror(str);
    exit(1);
}

SOCKET startTCP(unsigned short* port)
{
    WSADATA data;
    int ret = WSAStartup(MAKEWORD(2, 2), &data);
    if (ret)
    {
        error_die("WSAStartup");
    }

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == -1)
    {
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

SOCKET startUDP(unsigned short* port,string ip)
{
    WSADATA data;
    int ret = WSAStartup(MAKEWORD(2, 2), &data);
    if (ret)
    {
        error_die("WSAStartup");
    }

    SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_socket == -1)
    {
        error_die("套接字");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(*port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        error_die("connect");
    }

    return client_socket;
}

void sendData(SOCKET client_sock)
{
    char buffsend[4096];
    memset(buffsend, 0, sizeof(buffsend));

    while (1)
    {
        cout << "input:" << endl;
        cin >> buffsend;

        if (send(client_sock, buffsend, sizeof(buffsend), 0))
        {
            cout << "客户端输入：" << buffsend << endl;
            memset(buffsend, 0, sizeof(buffsend));
        }
        else
        {
            error_die("send");
        }
    }
}

void receiveData(SOCKET client_sock)
{
    char buffrecv[4096];
    memset(buffrecv, 0, sizeof(buffrecv));

    while (1)
    {
        if (recv(client_sock, buffrecv, sizeof(buffrecv), 0))
        {
            cout << "服务端返回：" << buffrecv << endl;
            memset(buffrecv, 0, sizeof(buffrecv));
        }
        else
        {
            error_die("recv");
        }
    }
}

struct DNSHeader {
    unsigned short ID;
    unsigned short flags;
    unsigned short questions;
    unsigned short answerRRs;
    unsigned short authorityRRs;
    unsigned short additionalRRs;
};

struct DNSQuestion {
    char* qname;
    unsigned short qtype;
    unsigned short qclass;
    int length;
    int label;
};

void DNS_header(DNSHeader* header) {
    srand((unsigned int)time(nullptr));
    header->ID = htons(rand());
    header->flags = 0x0100;
    header->questions = htons(0x0100);
    header->answerRRs = 0;
    header->authorityRRs = 0;
    header->additionalRRs = 0;
}

void dtoq(string domain, DNSQuestion* query) {
    int index = 0;
    int label = 0;
    int L = 0;
    int flag = 1;
    query->length = domain.length();
    query->qname = new char[domain.length() + 1];
    for (int i = 0; i < query->length; i++) {
        if (domain[i] == '.') {
            if (flag) {
                query->qname[index - label] = label;
                label = 0;
                index++;
                query->label++;
                flag = 0;
            }
            else {
                index++;
                query->qname[index - label - 1] = label;
                label = 0;
                query->label++;
            }
        }
        else {
            index++;
            query->qname[index] = domain[i];
            label++;
        }
    }
}

void DNS_query(string domain, DNSQuestion* query) {
    dtoq(domain, query);
    query->qtype = htons(0x0100);
    query->qclass = htons(0x0100);
}

string dtoh(long long num) {
    string str;
    long long Temp = num / 16;
    int left = num % 16;
    if (Temp > 0)
        str += dtoh(Temp);
    if (left < 10)
        str += (left + '0');
    else
        str += ('a' + left - 10);
    return str;
}

string print(string message) {
    if (message == "1" ) {
        return "00 01 ";
    }
    if (message == "0") return "00 00 ";
    if (message.length() < 4) {
        for (int i = 0; i < 4 - message.length(); i++) {
            message = '0' + message;
        }
    }
    return  message.substr(0, 2) + " " + message.substr(2, 2) + " ";
}

string dns_encode(string domain, DNSHeader* header, DNSQuestion* query) {
    domain.push_back('.');
    DNS_header(header);
    DNS_query(domain, query);
    string message="";
    message+=print(dtoh(stoi(to_string(header->ID))));
    message += print(dtoh(stoi(to_string(header->flags))));
    message += print(dtoh(stoi(to_string(header->questions))));
    message += print(dtoh(stoi(to_string(header->answerRRs))));
    message += print(dtoh(stoi(to_string(header->authorityRRs))));
    message += print(dtoh(stoi(to_string(header->additionalRRs))));

//   for (int i = 0; i < query->length; i++) {
//       message += print(dtoh(stoi(to_string(query->qname[i]))));
//    }
//    message += print(dtoh(stoi(to_string(query->qtype))));
//    message += print(dtoh(stoi(to_string(query->qclass))));
      return message;
}


int main()
{
    unsigned short port = 53;
    string ip="8.8,8.8", domain;
    int conn = 0;
    cout << "服务器ip：" << endl;
    cin >> ip;

    cout << "端口："<<endl;
    cin >> port;
    DNSHeader header;
    DNSQuestion query;
    
  
    // SOCKET client_sock = startUDP(&port,ip);

     cout << "dig: ";
     cin >> domain;
     string message=dns_encode(domain, &header, &query);
     cout << endl << message;
     return 0;
}
