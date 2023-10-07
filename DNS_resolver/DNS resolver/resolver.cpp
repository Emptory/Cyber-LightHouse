#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<cstdlib>
#include<cstdio>
#include<iomanip>
#include <string>
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


struct DNSRRs
{
    string qname;
    unsigned short Type;
    unsigned short Class;
    unsigned short TTL;
    unsigned short RDlength;
    string Rdate;
};

struct DNSQuestion
{
    string name;
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
    string message;
    //string binary[12];
    int num = 0;
    cin >> message;
   // for (int i = 0; i < 12; ++i) cin >> message[i];
    //for (int i = 0; i < 12; ++i)
   // {
    //    binary[i] = htob(message[i]);
   //         cout << binary[i] << endl;
    //}
    header.ID=btod(message.substr(0,16));
    flag_re(message.substr(16, 16));
    header.question= btod(message.substr(32, 16));
    header.answerRRs =btod(message.substr(48, 16));
    header.authorityRRs= btod(message.substr(64, 16));
    header.additionalRRs= btod(message.substr(80, 16));

    string qname;
    string domain;
    unsigned short length = 0;
    int count = 1;
    do
    {
        length = btod(message.substr(80+8*count, 8));
        while (length)
        {
            count++;
            length--;
             domain.push_back((char)btod(message.substr(80 + 8 * count, 8)));
        }
        count++;
        domain.push_back('.');
        
    } while (btod(message.substr(80 + 8 * count, 8)));
    domain.erase(domain.length() - 1);
    domain=domain.substr(1);
    query.qtype = btod(message.substr(80 + 8 * count + 8, 16));
    query.qclass = btod(message.substr(80 + 8 * count + 8 + 16, 16));
    if (query.qtype != 1)
    {
        printf("不支持的格式\n");
        return 0;
    }
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
    cout << "domain:                     " <<domain << endl;
    printf("type:                       %u\n", query.qtype);
    printf("class:                      %u\n", query.qclass);
    DNSRRs answer, authority，additional;

        int count1 = 120 + 8 * count;
        for (int i = 0; i < header.answerRRs; i++)
        {
            answer.qname = btod(message.substr(count1, 16));
            count1 += 16;
            answer.Type = btod(message.substr(count1, 16));
            count1 += 16;
            answer.Class = btod(message.substr(count1, 16));
            count1 += 16;
            answer.TTL = btod(message.substr(count1, 32));
            count1 += 32;
            answer.RDlength = btod(message.substr(count1, 16));
            answer.Rdate= "";
            count1 += 16;
            if (answer.RDlength == 4)
            {   
               
                printf("-- Answer --\n");

                printf("Type:                       %u\n", answer.Type);
                printf("Class:                      %u\n", answer.Class);
                printf("TTL:                        %u\n", answer.TTL);
                printf("RDlength:                   %u\n", answer.RDlength);
                cout << "Rdate:                      " << btod(message.substr(count1, 8)) << '.'<< btod(message.substr(count1 + 8, 8)) << '.'<< btod(message.substr(count1 + 16, 8)) << '.' << btod(message.substr(count1 + 24, 8))  << endl;
                printf("---\n\n");
                count1 += 32;
                continue;
            }
            count = 0;
            

            int rd = answer.RDlength;
            cout << rd << endl;
            do
            {
                length = btod(message.substr(count1 + count * 8, 8));
                if (length == 192)
                {
                    answer.Rdate += "com";
                    count += 16;
                    cout << answer.Rdate << endl;
                    break;
                }
                while (length)
                {
                    count++;
                    length--;
                    answer.Rdate.push_back((char)btod(message.substr(count1 + 8 * count, 8)));
                    rd--;
                }
                count++;
                answer.Rdate.push_back('.');
                rd--;
            } while (rd);
            count1 += 8 * answer.RDlength;
            printf("-- Answer --\n");
            cout << "domain:                     " << "" << endl;

            printf("Type:                       %u\n",answer.Type);
            printf("Class:                      %u\n", answer.Class);
            printf("TTL:                        %u\n", answer.TTL);
            printf("RDlength:                   %u\n", answer.RDlength);
            cout << "Rdate:                      " << answer.Rdate<<endl;
            printf("---\n\n");


        }
    return 0;
}