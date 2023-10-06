#include<cstdio>
#include<iostream>
#include<WinSock2.h>     
#include<WinSock2.h>     

#include<iostream>
#include<cstdlib>
#include<cstdio>
#include<iomanip>
#include<cmath>
#pragma comment(lib,"WS2_32.lib")  
using namespace std;

void error_die(const char* str) {
    perror(str);
    exit(1);
}

int startTCP(unsigned short* port) {
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        error_die("socket");
    }

    sockaddr_in server_addr{};
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(*port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        error_die("bind");
    }

    if (listen(server_socket, 5) == SOCKET_ERROR) {
        error_die("listen");
    }

    return server_socket;
}

int startUDP(unsigned short* port) {
    SOCKET server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == INVALID_SOCKET) {
        error_die("socket");
    }

    sockaddr_in server_addr{};
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(*port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (const struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        error_die("bind");
    }

    return server_socket;
}

int get_line(int sock, char* buff, int size) {
    int numchars = 0;
    char c = '\0';

    while ((numchars < size - 1) && (c != '\n')) {
        int ret = recv(sock, &c, 1, 0);
        if (ret > 0) {
            if (c == '\r') {
                ret = recv(sock, &c, 1, MSG_PEEK);
                if ((ret > 0) && (c == '\n')) {
                    recv(sock, &c, 1, 0);
                }
                else {
                    c = '\n';
                }
            }
            buff[numchars] = c;
            numchars++;
        }
        else {
            c = '\n';
        }
    }

    buff[numchars] = '\0';
    return numchars;
}


DWORD WINAPI accept_request(LPVOID arg) {
    int client = (int)arg;
    char buff[1024];

    int ret = get_line(client, buff, sizeof(buff));
    if (ret <= 0) {
        closesocket(client);
        return -1;
    }

    // ... 进行请求解析和处理 ...

    closesocket(client);
    return 0;
}

DWORD WINAPI accept_TCP(LPVOID arg) {
    SOCKET server_socket = (SOCKET)arg;

    while (1) {
        struct sockaddr_in client_addr {};
        int client_addr_len = sizeof(client_addr);

        SOCKET client_sock = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_sock == INVALID_SOCKET) {
            error_die("accept");
        }
        else {
            cout << "accepted" << endl;
        }

        DWORD threadID = 0;
        HANDLE threadHandle = CreateThread(NULL, 0, accept_request, (LPVOID)client_sock, 0, &threadID);
        if (threadHandle == NULL) {
            error_die("CreateThread");
        }
        CloseHandle(threadHandle);
    }

    return 0;
}

DWORD WINAPI accept_UDP(LPVOID arg) {
    SOCKET server_socket = (SOCKET)arg;
    struct sockaddr_in client_addr {};
    int client_addr_len = sizeof(client_addr);

    while (1) {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int ret = recvfrom(server_socket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if (ret > 0) {
            // ... 进行UDP请求处理 ...
        }
    }

    return 0;
}

void runServer(unsigned short port) {
    WSADATA data;
    int ret = WSAStartup(MAKEWORD(2, 2), &data);
    if (ret) {
        error_die("WSAStartup");
    }

    SOCKET server_socket = startUDP(&port);
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    while (1) {
        SOCKET client_sock = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_sock == -1) {
            error_die("accept");
        }
        else {
            cout << "accepted" << endl;
        }

        DWORD threadID = 0;
        CreateThread(0, 0, accept_TCP, (void*)client_sock, 0, &threadID);
        CreateThread(0, 0, accept_UDP, (void*)server_socket, 0, &threadID);
    }

    WSACleanup();
    closesocket(server_socket);
}

struct flag
{
    unsigned short QR;
    unsigned short opcode;
    unsigned short AA;
    unsigned short TC;
    unsigned short RD;
    unsigned short RA;
    unsigned short Z;
    unsigned short rcode;
};

struct DNSHeader
{
    unsigned short ID;
    flag flags;
    unsigned short question;
    unsigned short answerRRs;
    unsigned short authorityRRs;
    unsigned short additionalRRs;
};

unsigned short btod(string binaryString)
{
    unsigned short decimal = 0;

    for (int i = 0; i < binaryString.length(); i++)
    {
        decimal += (int)(binaryString[i] - '0') * (pow(2, binaryString.length() - i - 1));
    }

    return decimal;
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

flag parseFlags(string flag) {
    flag parsedFlags;
    parsedFlags.QR = (int)(flag[0] - '0');
    parsedFlags.opcode = btod(flag.substr(1, 4));
    parsedFlags.AA = (int)(flag[5] - '0');
    parsedFlags.TC = (int)(flag[6] - '0');
    parsedFlags.RD = (int)(flag[7] - '0');
    parsedFlags.RA = (int)(flag[8] - '0');
    parsedFlags.Z = btod(flag.substr(9, 3));
    parsedFlags.rcode = btod(flag.substr(12, 4));
    return parsedFlags;
}

DNSHeader parseDNSHeader(string binaryHeader) {
    DNSHeader header;
    header.ID = btod(binaryHeader.substr(0, 16));
    header.flags = parseFlags(binaryHeader.substr(16, 16));
    header.question = btod(binaryHeader.substr(32, 16));
    header.answerRRs = btod(binaryHeader.substr(48, 16));
    header.authorityRRs = btod(binaryHeader.substr(64, 16));
    header.additionalRRs = btod(binaryHeader.substr(80, 16));
    return header;
}

int main()
{
    string message[12];
    string binary[12];
    int num = 0;
    for (int i = 0; i < 12; ++i) cin >> message[i];
    for (int i = 0; i < 12; ++i)
    {
        binary[i] =htob(message[i]);
        cout << binary[i] << endl;
    }
    DNSHeader header = parseDNSHeader(binary[0] + binary[1]);
    printf("-- Header --\n");
    printf("id:                         %u\n", header.ID);

    printf("query(0) or response(1):    %u\n", header.flags.QR);
    printf("opcode:                     %u\n", header.flags.opcode);
    printf("authoritative answer:       %u\n", header.flags.AA);
    printf("truncated packet:           %u\n", header.flags.TC);
    printf("recursion desired:          %u\n", header.flags.RD);

    printf("recursion available:        %u\n", header.flags.RA);
    printf("zero:                       %u\n", header.flags.Z);
    printf("response code (0 is okay):  %u\n", header.flags.rcode);

    printf("questions:                  %u\n", header.question);
    printf("answerRRs:                  %u\n", header.answerRRs);
    printf("authoritativeRRs:           %u\n", header.authorityRRs);
    printf("additionalRRs:              %u\n", header.additionalRRs);
    printf("---\n\n");

    cout << "QUESTION SECTION" << endl;
    string qname;
    string domain;
    unsigned short length = 0;
    do
    {
        cin >> qname;
        length = btod(htob(qname));
        while (length)
        {
            cin >> qname;
            domain.push_back((char)btod(htob(qname)));
            length--;
        }
        domain.push_back('.');

    } while (btod(htob(qname)));
    domain.erase(domain.length() - 1);
    domain.erase(domain.length() - 1);
    cout << "domain:                     " << domain << endl;
    string qtype[2], qclass[2];
    cin >> qtype[0] >> qtype[1] >> qclass[0] >> qclass[1];
    printf("type:                       %u\n", btod(htob(qtype[0] + qtype[1])));
    printf("class:                      %u\n", btod(htob(qclass[0] + qclass[1])));
    return 0;
}

int main() {
    unsigned short port = 8000;
    cout << "监听端口：";
    cin >> port;
    cout << "正在监听：" << port << endl;

    runServer(port);

    return 0;
}
