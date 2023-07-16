#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define BUFFER_MAX 2048

#define IP 0x0800
#define ARP 0x0806
#define RARP 0x8035

// 函数名: print_mac_info
// 输入: 存放以太网帧的缓冲区首地址
// 功能: 打印源mac地址和目的mac地址
// 输出: 无
void print_mac_info(char* buffer)
{
    char *eth_head = buffer; //指向以太网帧的首地址
    unsigned char *p = eth_head; // 活动指针, 用于打印数据报内容

    printf("MAC address: %02x:%02x:%02x:%02x:%02x:%02x ==> %02x:%02x:%02x:%02x:%02x:%02x\n",
    p[6],p[7],p[8],p[9],p[10],p[11],
    p[0],p[1],p[2],p[3],p[4],p[5]);
}

// 函数名: ICMP_analyse
// 输入: 存放以太网帧的缓冲区首地址, 接受到的帧长度
// 功能: 解析协议类型为ICMP的IP数据报
// 输出: 无
void ICMP_analyse(char* buffer, int nr_recv)
{
    char *icmp_head = buffer + 34; // 指向类型为ICMP的IP数据报的首地址
    unsigned char *p; // 活动指针, 用于打印数据报内容

    // ICMP类型
    p = icmp_head;
    unsigned char type = p[0];
    printf("Type: %d (", p[0]);
    switch(type)
    {
    case 0: printf("Echo Reply)\n"); break;
    case 3: printf("Destination Unreachable)\n"); break;
    case 4: printf("Source Quench)\n"); break;
    case 5: printf("Redirect)\n"); break;
    case 8: printf("Echo Request)\n"); break;
    case 9: printf("Router Advertisement)\n"); break;
    case 10: printf("Router Solicitation)\n"); break;
    case 11: printf("Time Exceeded)\n"); break;
    case 17: printf("Address Mask Request)\n"); break;
    case 18: printf("Address Mask Reply)\n"); break;
    default: printf("unknown type)\n");
    }
    // 代码
    p += 1;
    unsigned char code = p[0];
    printf("Code: %d\n", p[0]);
    // 检验和
    p += 1;
    printf("Checksum: 0x%02x%02x\n", p[0], p[1]);

    // 根据类型和代码判断是否继续解析
    if ((0 == code) && (0 == type || 8 == type))
    {
        // 标识符
        p += 2;
        printf("Identifier: 0x%02x%02x\n", p[0], p[1]);
        // 序列号
        p += 2;
        printf("Sequence Number: 0x%02x%02x\n", p[0], p[1]);
        // 数据长度
        int data_len = nr_recv - 42;
        printf("ICMP Data Length: %d bytes\n", data_len);
    }
}

// 函数名: IGMP_analyse(未完成)(似乎没遇上过, 以后再说)
// 输入: 存放以太网帧的缓冲区首地址
// 功能: 解析协议类型为IGMP的IP数据报
// 输出: 无
void IGMP_analyse(char* buffer)
{
    char *igmp_head = buffer + 34; // 指向类型为IGMP的IP数据报的首地址
    unsigned char *p; // 活动指针, 用于打印数据报内容
}

// 函数名: IPIP_analyse(未完成)(似乎没遇上过, 以后再说)
// 输入: 存放以太网帧的缓冲区首地址
// 功能: 解析协议类型为IPIP的IP数据报
// 输出: 无
void IPIP_analyse(char* buffer)
{
    char *ipip_head = buffer + 34; // 指向类型为IPIP的IP数据报的首地址
    unsigned char *p; // 活动指针, 用于打印数据报内容
}

// 函数名: TCP_analyse
// 输入: 存放以太网帧的缓冲区首地址
// 功能: 解析协议类型为TCP的IP数据报
// 输出: 无
void TCP_analyse(char* buffer)
{
    char *tcp_head = buffer + 34; // 指向类型为TCP的IP数据报的首地址
    unsigned char *p; // 活动指针, 用于打印数据报内容

    // 源端口号
    p = tcp_head;
    unsigned short sp = *(unsigned short*)p;
    printf("Source Port: %u\n", sp);
    // 目的端口号
    p += 2;
    unsigned short dp = *(unsigned short*)p;
    printf("Destination Port: %u\n", dp);
    // 序列号
    p += 2;
    int sn = *(int*)p;
    printf("Sequence Number: %u\n", sn);
    // 应答号
    p += 4;
    int an = *(int*)p;
    printf("Acknowledgement Number: %u\n", an);
    // 头部长度
    p += 4;
    printf("Header Length: %d bytes\n", (p[0] & 0xF0) << 2);
    // 6位保留位

    // 6位标志位
    p += 1;
    printf("Flags: 0x%03x\n", p[0]);
    // 标志位集合
    char URG = (p[0] >> 5) & 0x01;
    char ACK = (p[0] >> 4) & 0x01;
    char PSH = (p[0] >> 3) & 0x01;
    char RST = (p[0] >> 2) & 0x01;
    char SYN = (p[0] >> 1) & 0x01;
    char FIN = p[0] & 0x01;
    printf("URG: %d, ACK: %d, PSH: %d, RST: %d, SYN: %d, FIN: %d\n", URG, ACK, PSH, RST, SYN, FIN);
    // 窗口大小
    p += 1;
    unsigned short wsv = *(unsigned short*)p;
    printf("Window Size Value: %u\n", wsv);
    // 检验和
    p += 2;
    printf("Checksum: 0x%02x%02x\n", p[0], p[1]);
    // 紧急指针
    p += 2;
    unsigned short up = *(unsigned short*)p;
    printf("Urgent Pointer: %u\n", up);
}

// 函数名: UDP_analyse
// 输入: 存放以太网帧的缓冲区首地址
// 功能: 解析协议类型为UDP的IP数据报
// 输出: 无
void UDP_analyse(char* buffer)
{
    char *udp_head = buffer + 34; // 指向类型为udp的IP数据报的首地址
    unsigned char *p; // 活动指针, 用于打印数据报内容

    // 源端口号
    p = udp_head;
    unsigned short sp = *(unsigned short*)p;
    printf("Source Port: %u\n", sp);
    // 目的端口号
    p += 2;
    unsigned short dp = *(unsigned short*)p;
    printf("Destination Port: %u\n", dp);
    // UDP长度
    p += 2;
    unsigned short udp_len = *(unsigned short*)p;
    printf("Length: %u bytes\n", udp_len << 2);
    // 检验和
    p += 2;
    printf("Checksum: 0x%02x%02x\n", p[0], p[1]);

}

// 函数名: IP_analyse
// 输入: 存放以太网帧的缓冲区首地址, 接受到的帧长度
// 功能: 解析类型为IP的以太网帧
// 输出: 无
void IP_analyse(char* buffer, int nr_recv)
{
    // 解析IP报文时使用的的一组指针集合, 用于定位
    char *eth_head = buffer;
    char *ip_head = eth_head + 14;

    // 活动指针, 用于打印数据报内容
    unsigned char *p;

    // 版本
    p = ip_head;
    printf("Version: %d\n", (p[0] >> 4) & 0x0F);
    // 首部长度
    printf("Internet Header Length: %d bytes\n", (p[0]&0x0F) << 2);
    // 服务类型
    p += 1;
    printf("Type Of Service: 0x%02x\n", p[0]);
    // 总长度
    p += 1;
    printf("Total Length: 0x%02x%02x\n", p[0], p[1]);
    // 标识
    p += 2;
    printf("Identification: 0x%02x%02x\n", p[0], p[1]);
    // 标志
    p += 2;
    printf("Flags: 0b%d%d%d\n", p[0] >> 7, (p[0] >> 6) & 0x01, (p[0] >> 5) & 0x01);
    // 片位移
    printf("Fragment Offset: 0x%d%x%02x\n", (p[0] >> 4) & 0x01, p[0] & 0x0F, p[1]);
    // 生存时间
    p += 2;
    printf("Time To Live(TTL): %d\n", p[0]);
    // 协议类型
    p += 1;
    printf("Protocol: %d (", p[0]);
    int proto = p[0]; 
    switch (proto)
    {
    case IPPROTO_ICMP: printf("icmp)\n"); break;
    case IPPROTO_IGMP: printf("igmp)\n"); break;
    case IPPROTO_IPIP: printf("ipip)\n"); break;
    case IPPROTO_TCP: printf("tcp)\n"); break;
    case IPPROTO_UDP: printf("udp)\n"); break;
    default: printf("Pls query yourself\n");
    }
    // 首部检验和
    p += 1;
    printf("Header Checksum: 0x%02x%02x\n", p[0], p[1]);
    // 源IP地址
    p += 2;
    printf("Source: %d.%d.%d.%d\n", p[0], p[1], p[2], p[3]);
    // 目的IP地址
    p += 4;
    printf("Destination: %d.%d.%d.%d\n", p[0], p[1], p[2], p[3]);

    // 根据协议类型继续解析数据帧
    switch (proto)
    {
    case IPPROTO_ICMP: ICMP_analyse(buffer, nr_recv); break;
    case IPPROTO_IGMP: IGMP_analyse(buffer); break;
    case IPPROTO_IPIP: IPIP_analyse(buffer); break;
    case IPPROTO_TCP: TCP_analyse(buffer); break;
    case IPPROTO_UDP: UDP_analyse(buffer); break;
    default: printf("Pls query yourself\n");
    }
}

// 函数名: ARP_analyse
// 输入: 存放以太网帧的缓冲区首地址
// 功能: 解析类型为ARP的以太网帧
// 输出: 无
void ARP_analyse(char* buffer)
{
    // 解析ARP报文时使用的的一组指针集合, 用于定位
    char *eth_head = buffer;
    char *arp_head = eth_head + 14;

    // 活动指针, 用于打印数据报内容
    unsigned char *p;

    // 硬件类型
    p = arp_head;
    printf("Hardware type: 0x%02x%02x\n", p[0], p[1]);
    // 协议类型
    p += 2;
    printf("Protocol type: 0x%02x%02x\n", p[0], p[1]);
    // 硬件地址长度
    p += 2;
    printf("Hardware size: 0x%02x\n", p[0]);
    // 协议地址长度
    p += 1;
    printf("Protocol size: 0x%02x\n", p[0]);
    // 操作码
    p += 1;
    printf("Opcode: 0x%02x%02x\n", p[0], p[1]);
    // 源mac地址
    p += 2;
    printf("Sender MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n", p[0], p[1], p[2], p[3], p[4], p[5]);
    // 源ip地址
    p += 6;
    printf("Sender IP address: %d.%d.%d.%d\n", p[0], p[1], p[2], p[3]);
    // 目的mac地址
    p += 4;
    printf("Target MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n", p[0], p[1], p[2], p[3], p[4], p[5]);
    // 目的ip地址
    p += 6;
    printf("Target IP address: %d.%d.%d.%d\n", p[0], p[1], p[2], p[3]);
}

int main(int argc, char* argv[])
{
    int sock_fd; // 套接字描述符
    int nr_recv; // 接受到的报文的字节数(其实就是长度啦)
    char buffer[BUFFER_MAX]; // 存放报文的缓冲区

    // sock_fd = socket(family, type, protocol), socket函数原型
    // htons() "host to network short"  format: h/n to n/h s/l (), 用于大小端转换

    // 下面是对socket函数的三个参数的说明:
    // 第一个参数使用AF_INET可以接收协议类型为(tcp udp icmp等)发往本机的ip数据包, 而使用PF_PACKET可以监听网卡上的所有数据帧

    // 第二个参数有SOCK_RAW, SOCK_DGRAM和SOCK_PACKET三种, 第三种已经废弃不用
    // 使用SOCK_RAW发送的数据必须包含链路层的协议头, 接收到的数据包自然也包含链路层协议头. 而使用SOCK_DGRAM时数据不含链路层协议头

    // 第三个参数有ETH_P_IP, ETH_P_ARP, ETH_P_RARP, ETH_P_ALL等, 用于限定接收的数据帧类型

    // 创建套接字并处理创建套接字失败的情况
    if ((sock_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
    { 
        printf("error when create raw socket\n");
        return -1;
    }

    while (1)
    {
        // 接收数据包(放入buffer), 得到数据包长度(用nr_recv表示)
        // 最后两个参数置为NULL，表示不绑定地址，来了的数据包都接收
        nr_recv = recvfrom(sock_fd, buffer, BUFFER_MAX, 0, NULL, NULL);

        // 以太网帧中的数据长度规定最小46字节, 最大1500字节, ARP和RARP类型数据包的长度不够46字节, 会在后面补填充位
        if (nr_recv < 46 || nr_recv > 1500)
        {
            printf("nr_recv = %d, error when recv msg \n", nr_recv);
            continue;
        }

        // 打印mac地址信息
        print_mac_info(buffer);

        // 判断数据报类型并进行相应处理
        unsigned char *p = buffer + 12;
        unsigned short type = (p[0] << 8) + p[1];
        switch (type)
        {
        case IP: printf("type: IP (0x%04x)\n", type); IP_analyse(buffer, nr_recv); break;
        case ARP: printf("type: ARP (0x%04x)\n", type); ARP_analyse(buffer); break;
        // 似乎现在没有RARP类型的帧, 仅仅是根据ARP协议中的操作码加以区分, 若遇到该类型帧, 直接使用ARP_analyse函数进行解析
        case RARP: printf("type: RARP (0x%04x)\n", type); ARP_analyse(buffer); break;
        default: printf("unknown type(0x%04x)\n", type); 
        }
        printf("\n");
    }
    return -1;
}