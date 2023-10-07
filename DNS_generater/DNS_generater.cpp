#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <winsock2.h>
#include <ctime>
#include<cstdio>
#include<cstdlib>
#include <string>
#include<cstring>
#include <sstream>
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
    header->flags = 0x0100;
    header->questions = htons(0x0100);
    header->answerRRs = 0;
    header->authorityRRs = 0;
    header->additionalRRs = 0;
}
void dtoq(string domain)
{
    int index = 0;
    int label =0;
    int L = 0;
    int flag = 1;
    query.length= domain.length();
    query.qname = new char [domain.length() + 1];
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
void DNS_query(string  domain, DNSQuestion* query)
{
    dtoq(domain);
    query->qtype = htons(0x0100);
    query->qclass = htons(0x0100);
}


string dtoh(long long num)  
{  
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

void print(string message)
{
    if (message == "1" )
    {
        cout << "00 01 ";
        return;
    }
    if (message == "0") {
        cout << "00 00 ";
        return;
    }
    if (message.length ()< 4)
    {
        for (int i = 0; i <4 - message.length(); i++) message = '0' + message;
    }
    cout << message[0]<<message[1] << ' ' << message[2]<< message[3] << ' ';
}

int main()
{
    string domain;
    cin >> domain;
    DNS_header(&header);
    domain.push_back('.');
    DNS_query(domain, &query);
    print(dtoh(stoi(to_string(header.ID))));
    print(dtoh(stoi(to_string(header.flags))));
    print(dtoh(stoi(to_string(header.questions))));
    print(dtoh(stoi(to_string(header.answerRRs))));
    print(dtoh(stoi(to_string(header.authorityRRs))));
    print(dtoh(stoi(to_string(header.additionalRRs))));
    
    
    for (int i = 0; i < query.length; i++) cout << hex << setw(2) << setfill('0') << (unsigned short)query.qname[i] << ' ';
    cout << "00 ";
    print(dtoh(stoi(to_string(query.qtype))));
    print(dtoh(stoi(to_string(query.qclass))));
    return 0;
}
