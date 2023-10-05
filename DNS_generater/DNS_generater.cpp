#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <winsock2.h>
#include <ctime>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include <iomanip>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

struct DNSHeader
{
    unsigned short ID;
    unsigned short flags;
    unsigned short questions;
    unsigned short answerRRs;
    unsigned short authorityRRs;
    unsigned short additionalRRs;
}header;

struct DNSQuestion
{
    char* qname;
    unsigned short qtype;
    unsigned short qclass;
    int length;
    int label;
}query;

void DNS_header(DNSHeader * header)
{
    srand(time(NULL));
    header->ID = htons(rand());
    header->flags = htons(0x0100);
    header->questions = htons(0x0001);
    header->answerRRs = htons(0);
    header->authorityRRs = htons(0);
    header->additionalRRs = htons(0);
}
void dtoq(const char* domain)
{
    int index = 0;
    int label =0;
    int L = 0;
    int flag = 1;
    query.length= strlen(domain);
    query.qname = new char [strlen(domain) + 1];
    for (int i = 0; i <query.length; i++)
    {
        if (domain[i] == '.')
        {
            if (flag)
            {
                query.qname[index - label] = label;
                label = 0;
                index++;
                query.label++;
                flag = 0;
            }
            else
            {index++;
                query.qname[index - label-1] = label;
                label = 0;
                
                query.label++;
            }
        }
        else 
        {
            index++;
            query.qname[index] =domain[i];
            label++;
        }
    }

}
void DNS_query(const char * domain, DNSQuestion* query)
{
    dtoq(domain);
    query->qtype = htons(0x0001);
    query->qclass = htons(0x0001);
}

int main()
{
    DNS_header(&header);
    DNS_query("www.baidu.com.", &query);
    cout << hex << header.ID << ' ' << "0100 0001 0000 0000 0000 ";
        // header.flags << ' ' << header.questions << ' ' << header.answerRRs << ' ' << header.authorityRRs << ' ' << header.additionalRRs<<' ';
    for (int i = 0; i < query.length; i++) cout << hex << setw(2) << setfill('0') << (unsigned short)query.qname[i] << ' ';
    cout << "0001 0001";
    //cout<<"00 "<< query.qtype << ' ' << query.qclass;
}
