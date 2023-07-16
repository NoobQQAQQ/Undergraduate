#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h> 
#include <arpa/inet.h>
#include <sys/time.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#define BUFFER_MAX 2048

char icmp_str[] = "!abcdefg, hijklmn!"; // 用于填充icmp的数据段
struct sockaddr_in dst; // 被ping主机的ip

int nr_recv = 0; // 接收到的报文数量
int nr_sent = 0; // 发送的报文数量

struct timeval send_time; //发送ICMP包头的时间
struct timeval recv_time; //收到ICMP应答的时间

// 函数名: print_statistics
// 输入: 无
// 功能: 打印整个ping的统计数据
// 输出: 无
void print_statistics()
{
    printf("--- %s ping statistics ---\n", inet_ntoa(dst.sin_addr)); // inet_ntoa函数将网络地址转换成以'.'分割的字符串格式
    printf("%d packets transmitted, %d received, %0.0f%% packet loss\n",
           nr_sent, nr_recv, 1.0 * (nr_sent - nr_recv) / nr_sent * 100);
}

// 函数名: checksum
// 输入: 发送的icmp包缓冲区, icmp包长度
// 功能: 打印整个ping的统计数据
// 输出: icmp包的检验和
uint16_t checksum(char *buf, int len)
{
    uint32_t sum = 0;
    uint16_t *cbuf;
    
    cbuf = (uint16_t*)buf;
    
    while (len > 1)
    {
        sum += *cbuf++;
        len -= 2;
    }
    
    if (len)
        sum += *(uint8_t*)cbuf;
    
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    
    return ~sum;
}

// 函数名: send_packet
// 输入: 套接字描述符
// 功能: 构造icmp包头并向目的IP发送数据包
// 输出: 无
void send_packet(int sock_fd)
{
    struct icmp *icmp; // icmp为ICMP头部结构体
    int len = 0; // icmp包头的长度
    char send_buffer[BUFFER_MAX]; // 存放待发送的报文的缓冲区
 
    icmp = (struct icmp *)(send_buffer); // 类型转换以填充包头
    icmp->icmp_type = ICMP_ECHO; // 初始化ICMP消息类型type
    
    icmp->icmp_code = 0; // 初始化消息代码code
    icmp->icmp_id = getpid(); // 把进程号给icmp_id
    icmp->icmp_seq = nr_sent++; // 发送的ICMP消息序号赋值给icmp序号

    memcpy(icmp->icmp_data, icmp_str, strlen(icmp_str)); // 用icmp_str当作发送的数据
    len = 8 + strlen(icmp_str); // 8是包头其他部分的长度

    icmp->icmp_cksum = 0; // 初始化校验和
    icmp->icmp_cksum = checksum((char*)icmp, len); // 计算校验和

    gettimeofday(&send_time, NULL); // 获取当前时间
    sendto(sock_fd, send_buffer, len, 0, (struct sockaddr*)&dst, sizeof(dst)); //经socket传送数据
}

// 函数名: ICMP_analyse
// 输入: 存放收到的ip包的缓冲区首地址, 接受到的包长度
// 功能: 解析协议类型为ICMP的IP数据报
// 输出: 收到正确的包返回1, 否则返回0
int ICMP_analyse(char* buffer, int recv_len)
{
    struct ip *ip;
    struct icmp *icmp;
    
    int ip_len, icmp_len; // ip头长度 icmp包长度
    double rtt; // 往返时间
    
    ip = (struct ip*)buffer;
    
    ip_len = ip->ip_hl << 2; // ip->ip_hl记录了ip头含有多少个4字节
    icmp = (struct icmp*)(buffer + ip_len);
    icmp_len = recv_len - ip_len;

    // 判断收到的包是否正确
    if(icmp->icmp_type != ICMP_ECHOREPLY)
        return 0;
    
    // 计算往返时间并放入recv_time里
    recv_time.tv_sec -= send_time.tv_sec;
    recv_time.tv_usec -= send_time.tv_usec;

    // 计算rtt
    rtt = recv_time.tv_sec * 1000.0 + recv_time.tv_usec/1000.0;

    //打印结果
    printf("%d bytes from %s:icmp_seq=%u ttl=%d rtt=%.3f ms\n",
           icmp_len, // icmp数据长度
           inet_ntoa(ip->ip_src), // 发送数据包的ip地址
           icmp->icmp_seq, // icmp报文序列号
           ip->ip_ttl, // 生存时间
           rtt); // 往返时间

    return 1;
}

// 函数名: recv_packet
// 输入: 套接字描述符
// 功能: 收取目的IP发回的数据包并解析
// 输出: 无
void recv_packet(int sock_fd)
{
    char recv_buffer[BUFFER_MAX]; // 存放接收到的报文的缓冲区
    int recv_len = recvfrom(sock_fd, recv_buffer, BUFFER_MAX, 0, NULL, NULL); // 接收目的IP发回的ICMP数据包

    // 检测长度是否有错
    if (recv_len < 43 || recv_len > 1500)
    {
        printf("nr_recv = %d, error when recv msg \n", recv_len);
        exit(1);
    }

    gettimeofday(&recv_time, NULL); // 记录收到应答的时间
    if(ICMP_analyse(recv_buffer, recv_len)) // 接收到正确的ICMP应答信息
        nr_recv++;
}

int main(int argc, char* argv[])
{
    struct hostent *host; // host entry结构体, 用于解析域名, 实现ping外网的功能

    if (argc != 2) // 处理参数个数不正确的情况
    {      
        printf("Usage: %s hostname\n", argv[0]);
        return -1;
    }
    
    if ((host = gethostbyname(argv[1])) == NULL) // gethostbyname()返回对应于给定主机名的host entry之地址
    {    
        printf("can not parse the host name"); // 无法解析主机地址 
        return -1;
    }

    memset(&dst, 0, sizeof(dst)); // 初始化dst结构
    dst.sin_family = AF_INET;
    dst.sin_addr = *(struct in_addr*)host->h_addr_list[0]; // 给dst赋目的ip地址

    int sock_fd; // 套接字描述符

    // 创建套接字并处理创建套接字失败的情况
    if ((sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
    { 
        printf("error when create raw socket\n");
        return -1;
    }

    // 模仿ping
    printf("Ping %s(%s): 56(84) bytes of data.\n", argv[1], inet_ntoa(dst.sin_addr));

    // 只发4个包然后结束程序
    for (int i = 0; i < 4; i++)
    {
        send_packet(sock_fd); // 构造数据包并发送至目的主机
        recv_packet(sock_fd); // 接收目的主机发回的数据包, 解析并在终端中打印
    }
    print_statistics(); // 打印统计数据
}