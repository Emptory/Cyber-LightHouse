#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<cstdlib>
#include<cstdio>
#include<iomanip>

using namespace std;

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
}header;


struct DNSQuestion
{
    string qname;
    unsigned short qtype;
    unsigned short qclass;
}query;


unsigned short btod(string binaryString)
{
    unsigned short decimal = 0;

    for (int i = 0; i < binaryString.length(); i++)
    {
        decimal+= (int)(binaryString[i] - 48) * (pow(2, binaryString.length()-i-1));
    }

    return decimal;
}

string htob(string message)
{
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



void flag_re(string flag)
{
    header.flags.QR = (int)(flag[0]-48);
    header.flags.opcode = btod(flag.substr(1,4));
    header.flags.AA= (int)(flag[5] - 48);
    header.flags.TC = (int)(flag[6] - 48);
    header.flags.RD = (int)(flag[7] - 48);
    header.flags.RA = (int)(flag[8] - 48);
    header.flags.Z = btod(flag.substr(9.3));
    header.flags.rcode = btod(flag.substr(12,4));
}

int main()
{
    string message[12];
    string binary[12];
    int num = 0;
    for (int i = 0; i < 12; ++i) cin >> message[i];
    for (int i = 0; i < 12; ++i)
    {
        binary[i] = htob(message[i]);
            cout << binary[i] << endl;
    }
    header.ID=btod(binary[0] + binary[1]);
    flag_re(binary[2] + binary[3]);
    header.question= btod(binary[4] + binary[5]);
    header.answerRRs =btod(binary[6] + binary[7]);
    header.authorityRRs= btod(binary[8] + binary[9]);
    header.additionalRRs= btod(binary[10] + binary[11]);
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
    cout << "domain:                     " <<domain << endl;
    string qtype[2], qclass[2];
    cin >> qtype[0] >> qtype[1] >> qclass[0] >> qclass[1];
    printf("type:                       %u\n", btod(htob(qtype[0]+qtype[1])));
    printf("class:                      %u\n", btod(htob(qclass[0] + qclass[1])));
    return 0;
}