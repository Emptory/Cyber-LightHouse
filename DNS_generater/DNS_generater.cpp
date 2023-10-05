#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <winsock2.h>
#include <ctime>
#include<cstdio>
#include<cstdlib>
#include<cstring>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

struct DNSHeader
{
    unsigned short ID;
    unsigned short flags;
    unsigned short questions;
    unsigned short answers;
    unsigned short authorityRRs;
    unsigned short additionalRRs;
};

struct DNSQuestion
{
    char* qname;
    unsigned short qtype;
    unsigned short qclass;
};

void DNS_header(DNSHeader * header)
{
    srand(time(NULL));
    header->ID = htons(rand());
    header->flags = htons(0x0100);
    header->questions = htons(1);
    header->answers =htons(0);
    header->authorityRRs = 0x0000;
    header->additionalRRs = 0x0000;
}
void dtoq(const char* domain, char *qname)
{
    int index = 1;
    int label =0;
    int flag = 1;
    qname = new char [strlen(domain) + 2];
    for (int i = 0; i < strlen(domain); i++)
    {
        if (domain[i] == '.')
        {
            if (flag)
            {
                qname[index - label - 1] = (char)label;
                label = 0;
                flag = 0;
            }
            else
            {
                qname[index - label] = (char)label;
                label = 0;
            }
        }
        else
        {
            qname[index] =domain[i];
            index++;
            label++;
        }
    }

    qname[index - label] = (char)label; 
}
void DNS_query(const char * domain, DNSQuestion* query)
{
    dtoq(domain, query->qname);
    query->qtype = htons(1);
    query->qclass = htons(1);
}



int main()
{
    DNSHeader header;
    DNSQuestion query;
    DNS_header(&header);
    DNS_query("www.baidu.com", &query);
    cout << hex << header.ID << endl;
    cout << hex << ((header.flags & 0xf0) >> 4) << (header.flags & 0x0f)<< endl;
    cout << hex << ((header.questions & 0xf0) >> 4) << (header.questions & 0x0f) << endl;
    cout << hex << ((header.answers & 0xf0) >> 4) << (header.answers & 0x0f) <<  endl;
    cout << hex << ((header.authorityRRs & 0xf0) >> 4) << (header.authorityRRs & 0x0f)<< endl;
    cout << hex << ((header.additionalRRs & 0xf0) >> 4) << (header.additionalRRs & 0x0f) << endl;
    
}
