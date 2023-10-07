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
int flag = 0;
void error_die(const char* str)
{
    perror(str);
    exit(0);
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

    return client_socket;
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

string print(string message)
{

    if (message.length() == 1)
    {
        message = "000" + message;
    }

    if ((message.length() < 4))
    {
        for (int i = 0; i < 4 - message.length(); i++)
        {
            message = "0" + message;
        }
    }
    return  message.substr(0, 2) + " " + message.substr(2, 2) + " ";
}

string dns_encode(string domain, DNSHeader* header, DNSQuestion* query) {
    domain.push_back('.');
    DNS_header(header);
    DNS_query(domain, query);

    cout << endl;
    string message = "";
    message += print(dtoh(stoi(to_string(header->ID))));
    message += print(dtoh(stoi(to_string(header->flags))));
    message += print(dtoh(stoi(to_string(header->questions))));
    message += print(dtoh(stoi(to_string(header->answerRRs))));
    message += print(dtoh(stoi(to_string(header->authorityRRs))));
    message += print(dtoh(stoi(to_string(header->additionalRRs))));

    for (int i = 0; i < query->length; i++)
    {
        string t = (dtoh(stoi(to_string(query->qname[i]))));
        if (t.length() == 1)
            t = "0" + t;
        message += t;
        message += ' ';
    }
    message += "00 ";
    message += print(dtoh(stoi(to_string(query->qtype))));
    message += print(dtoh(stoi(to_string(query->qclass))));
      return message;
}
string htob(string message) {
    string binary;
    for (int j = 0; j < message.length(); j++)
    {
        switch (message[j])
        {
        case ('0'): binary.append("0000"); break;
        case ('1'): binary.append("0001"); break;
        case ('2'): binary.append("0010"); break;
        case ('3'): binary.append("0011"); break;
        case ('4'): binary.append("0100"); break;
        case ('5'): binary.append("0101"); break;
        case ('6'): binary.append("0110"); break;
        case ('7'): binary.append("0111"); break;
        case ('8'): binary.append("1000"); break;
        case ('9'): binary.append("1001"); break;
        case ('a'): binary.append("1010"); break;
        case ('b'): binary.append("1011"); break;
        case ('c'): binary.append("1100"); break;
        case ('d'): binary.append("1101"); break;
        case ('e'): binary.append("1110"); break;
        case ('f'): binary.append("1111"); break;
        }
    }
    return binary;
}

int main()
{
    unsigned short port = 53;
    string ip="8.8,8.8", domain;
    int conn = 0;
    cout << "服务器ip：" << endl;
    if (ip == "0") ip = "8.8,8.8";
    cin >> ip;

    cout << "端口："<<endl;
    if (port == 0) port = 53;
    cin >> port;
    DNSHeader header;
    DNSQuestion query;
    string bimessage="";
  
    SOCKET client_sock = startUDP(&port,ip);

    cout << "dig: ";
    cin >> domain;
    string message=dns_encode(domain, &header, &query);
    bimessage = htob(message);
    cout << endl << message<<endl;

    struct sockaddr_in server_addr;
    int len_server = sizeof(server_addr);

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    const char* addr = ip.c_str();
    server_addr.sin_addr.s_addr = inet_addr(addr);

    char buffrecv[4096];
    memset(buffrecv, 0, sizeof(buffrecv));
    if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        error_die("connect");
    }

    if (sendto(client_sock, bimessage.c_str(), bimessage.size(), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        cout << "客户端输入：" << bimessage << endl;
    }
    else
    {
        error_die("send");
    }
    if (recvfrom(client_sock, buffrecv, sizeof(buffrecv)-1, 0, (struct sockaddr*)&server_addr, &len_server))
    {
        cout << "服务端返回：" << buffrecv << endl;
    }
    else
    {
        error_die("recv");
    }
    return 0;
}
