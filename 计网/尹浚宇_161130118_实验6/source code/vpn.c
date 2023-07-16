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
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h> 
#include <net/if_arp.h>
#include <net/ethernet.h>

#define MAX_ROUTE_SIZE 10
#define MAX_ARP_SIZE 10
#define MAX_DEVICE_SIZE 3
#define BUFFER_MAX 2048

#define IP 0x0800
#define ARP 0x0806
#define RARP 0x8035

// route table
struct route_item
{
    char destination[16];
    char gateway[16];
    char netmask[16];
    char interface[16];
} route_table[MAX_ROUTE_SIZE];
// the sum of the items in the route table
int route_item_nr = 0;

// arp table
struct arp_item
{
    char ip_addr[16];
    char mac_addr[18];
} arp_table[MAX_ARP_SIZE];
// the sum of the items in the arp table
int arp_item_nr = 0;

// device table
struct device_item
{
    char interface[14];
    char mac_addr[18];
    char ip_addr[16];
    char netmask[16];
    int is_entrance;
} device_table[MAX_DEVICE_SIZE];
// the sum of the items in the device table
int device_item_nr = 0;

// vpn infomation
struct vpn_item
{
    char dst_ip[16];
    char src_ip[16];
} vpn_info;

// 函数名: read_device
// 输入: 无
// 功能: 读取程序目录下的设备表
// 输出: 无
void read_device()
{
    printf("reading device table...");

    FILE *fp = fopen("config/device.txt", "r");
    if (fp == NULL)
    {
        printf("open file failed\n");
        exit(-1);
    }
    fscanf(fp, "%d", &device_item_nr);
    int i = 0;
    while (i < device_item_nr)
    {
        fscanf(fp, "%s", device_table[i].interface);
        fscanf(fp, "%s", device_table[i].mac_addr);
        fscanf(fp, "%s", device_table[i].ip_addr);
        fscanf(fp, "%s", device_table[i].netmask);
        fscanf(fp, "%d", &device_table[i].is_entrance);
        i++;
    }
    fclose(fp);

    printf("\t\tdone\n");
}

// 函数名: read_arp
// 输入: 无
// 功能: 读取程序目录下的arp表
// 输出: 无
void read_arp()
{
    printf("reading arp table...");

    FILE *fp = fopen("config/arp.txt", "r");
    if (fp == NULL)
    {
        printf("open file failed\n");
        exit(-1);
    }
    fscanf(fp, "%d", &arp_item_nr);
    int i = 0;
    while (i < arp_item_nr)
    {
        fscanf(fp, "%s", arp_table[i].ip_addr);
        fscanf(fp, "%s", arp_table[i].mac_addr);
        i++;
    }
    fclose(fp);

    printf("\t\tdone\n");
}

// 函数名: read_route
// 输入: 无
// 功能: 读取程序目录下的路由表
// 输出: 无
void read_route()
{
    printf("reading route table...");

    FILE *fp = fopen("config/route.txt", "r");
    if (fp == NULL)
    {
        printf("open file failed\n");
        exit(-1);
    }
    fscanf(fp, "%d", &route_item_nr);
    int i = 0;
    while (i < route_item_nr)
    {
        fscanf(fp, "%s", route_table[i].destination);
        fscanf(fp, "%s", route_table[i].gateway);
        fscanf(fp, "%s", route_table[i].netmask);
        fscanf(fp, "%s", route_table[i].interface);
        i++;
    }
    fclose(fp);

    printf("\t\tdone\n");
}

// 函数名: read_vpn
// 输入: 无
// 功能: 读取程序目录下的vpn信息
// 输出: 无
void read_vpn()
{
    printf("reading vpn info...");

    FILE *fp = fopen("config/vpn.txt", "r");
    if (fp == NULL)
    {
        printf("open file failed\n");
        exit(-1);
    }
    fgets(vpn_info.dst_ip, 16, fp);
    fgets(vpn_info.src_ip, 16, fp);
    fclose(fp);

    printf("\t\tdone\n");
}

// 函数名: test
// 输入: 无
// 功能: 测试配置文件是否读取成功
// 输出: 无
void test()
{
    printf("\ndevice table has %d items\n", device_item_nr);
    for (int i = 0; i < device_item_nr; i++)
        printf("%s %s %s %s %d\n", device_table[i].interface, device_table[i].mac_addr, device_table[i].ip_addr, device_table[i].netmask, device_table[i].is_entrance);
    
    printf("\narp table has %d items\n", arp_item_nr);
    for (int i = 0; i < arp_item_nr; i++)
        printf("%s %s\n", arp_table[i].ip_addr, arp_table[i].mac_addr);

    printf("\nroute table has %d items\n", route_item_nr);
    for (int i = 0; i < route_item_nr; i++)
        printf("%s %s %s %s\n", route_table[i].destination, route_table[i].gateway, route_table[i].netmask, route_table[i].interface);

    printf("\nvpn info\n");
    printf("%s%s\n", vpn_info.dst_ip, vpn_info.src_ip);

    printf("\n");
}

// 函数名: read_file
// 输入: 无
// 功能: 读取程序目录下的配置文件
// 输出: 无
void read_file()
{
    read_device(); // 读设备表
    read_arp(); // 读arp表
    read_route(); // 读路由表
    read_vpn(); // 读vpn信息
    test(); // 测试读取结果
}

// 函数名: checksum
// 输入: 存放待计算检验和的数据的缓冲区和数据长度
// 功能: 计算检验和
// 输出: 检验和
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

// 函数名: search_route
// 输入: 目的ip, 指向源端口的指针
// 功能: 根据目的ip查询路由表获取下一跳ip, 源端口
// 输出: 指向下一跳ip的指针
char* search_route(char *dst_ip_addr, char **interface)
{
    uint32_t ip1 = inet_addr(dst_ip_addr);
    for (int i = 0; i < route_item_nr; i++)
    {
        uint32_t ip2 = inet_addr(route_table[i].destination);
        uint32_t mask = inet_addr(route_table[i].netmask);
        if ((ip1&mask) == ip2)
        {
           *interface = route_table[i].interface;
           if (!strcmp(route_table[i].gateway, "0.0.0.0"))
                return dst_ip_addr;
            return route_table[i].gateway;
        }
    }
    return NULL;
}

// 函数名: search_arp
// 输入: 下一跳ip, 待填的目的mac数组
// 功能: 根据目的ip填充存放mac的数组
// 输出: 无
void search_arp(char *ip_next_hop, uint8_t dst_mac[6])
{
    for (int i = 0; i < arp_item_nr; i++)
    {
        if (!strcmp(ip_next_hop, arp_table[i].ip_addr))
        {
            sscanf(arp_table[i].mac_addr, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
            &dst_mac[0], &dst_mac[1], &dst_mac[2], &dst_mac[3], &dst_mac[4], &dst_mac[5]); 

            return;
        }
    }

    // 可考虑arp广播的拓展功能
}

// 函数名: search_device
// 输入: 存放源mac地址的数组, 源端口
// 功能: 填充源mac地址
// 输出: 无
void search_device(uint8_t src[6], char *interface)
{
    for (int i = 0; i < device_item_nr; i++)
        if (!strcmp(interface, device_table[i].interface))
        {
            // 使用sscanf函数获取源mac地址
            sscanf(device_table[i].mac_addr, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", 
            &src[0], &src[1], &src[2], &src[3], &src[4], &src[5]); 
            // 检查获取结果
            printf("src mac is %02x:%02x:%02x:%02x:%02x:%02x\n", 
            src[0], src[1], src[2], src[3], src[4], src[5]);
            return;
        }
}

// 函数名: get_dst_mac
// 输入: 存放目的mac的数组, 目的ip
// 功能: 根据目的ip填充存放mac的数组
// 输出: 源端口
char* get_dst_mac(uint8_t dst[6], char *dst_ip_addr)
{
    char *ip_next_hop; // 下一跳ip
    char *interface; // 源端口号

    ip_next_hop = search_route(dst_ip_addr, &interface); // 查路由表获得下一跳ip, 端口号
    if (ip_next_hop == NULL)
        return NULL;

    printf("ip_next_hop is %s\n", ip_next_hop);
    printf("interface is %s\n", interface);
    search_arp(ip_next_hop, dst); // 查arp表获得目的mac
    // 检查获取结果
    printf("dst mac is %02x:%02x:%02x:%02x:%02x:%02x\n", dst[0], dst[1], dst[2], dst[3], dst[4], dst[5]);
    return interface;
}

// 函数名: get_src_mac
// 输入: 存放源mac地址的数组, 源端口
// 功能: 填充输入的数组
// 输出: 无
void get_src_mac(uint8_t src[6], char *interface)
{
    search_device(src, interface);
}

// 函数名: fill_sockaddr_ll
// 输入: 指向待填充的sockaddr_ll结构体的指针, 存放源MAC地址的数组, 源端口
// 功能: 填充指向的结构体
// 输出: 无
void fill_sockaddr_ll(struct sockaddr_ll *pd, uint8_t src[6], char *interface)
{
    memset(pd, 0, sizeof(struct sockaddr_ll)); // 初始化结构体为0

    pd->sll_family = AF_PACKET; // 地址族为AF_PACKET与创建套接字时保持一致

    pd->sll_protocol = htons(ETH_P_IP); // 上层协议为IP

    pd->sll_ifindex = if_nametoindex(interface); // 网口编号通过网口名来获取

    // --------------以下两个变量只在接受分组时有意义---------------
    pd->sll_hatype = ARPHRD_ETHER; // arp硬件地址类型

    pd->sll_pkttype = PACKET_OTHERHOST; // 在混杂模式下的设备驱动器发向其他主机的分组
    // ----------------------------------------------------------

    pd->sll_halen = htons(ETH_ALEN); // 物理层地址长度, 这里为6字节

    memcpy(pd->sll_addr, src, ETH_ALEN); // 填充源mac地址
}

// 函数名: repack
// 输入: 存放以太网帧的缓冲区首地址, 帧长度
// 功能: 重新包装该ip包并转发
// 输出: 无
void repack(char *buffer, int frame_len)
{
    char *dst_ip = vpn_info.dst_ip;
    char *src_ip = vpn_info.src_ip;
    char temp[512];
    memcpy(temp, buffer + 14, frame_len - 14); // 暂存除以太网头的剩余包
    memset(buffer, 0, sizeof(char)*frame_len); // buffer清零

    uint8_t src_mac[6]; // 源mac地址
	uint8_t dst_mac[6]; // 目的mac地址
    struct sockaddr_ll device; // 配合sendto()函数来发送以太网帧
    char *interface = get_dst_mac(dst_mac, dst_ip); // 获取目的mac地址, 源端口号
    get_src_mac(src_mac, interface); // 获取源mac地址
    fill_sockaddr_ll(&device, src_mac, interface); // 填充sockaddr_ll结构
    // 填充以太网头部
    memcpy(buffer, dst_mac, 6);
	memcpy(buffer + 6, src_mac, 6);
    buffer[12] = 0x08;
	buffer[13] = 0x00;
    // 填充新的ip头部
    struct ip *ip = (struct ip*)(buffer + 14);

    ip->ip_hl = 5; // ip头长度为5个4字节
    ip->ip_v = 4; // for ipv4
    ip->ip_tos = 0x00; // 服务类型
    uint16_t ip_len = 20 + frame_len - 14;
    ip->ip_len = htons(ip_len);
    ip->ip_id = getpid();
    ip->ip_off = IP_DF; // 不分片
    ip->ip_ttl = 64; // tll = 64
    ip->ip_p = 4; // 上层协议为ipip, 对应编号为4
   
    // 填充源和目的ip地址
    ip->ip_src.s_addr = inet_addr(src_ip);
    ip->ip_dst.s_addr = inet_addr(dst_ip);

    ip->ip_sum = 0;
    ip->ip_sum = checksum((char*)ip, sizeof(struct ip)); // 检验和
    // 填充之后的部分
    memcpy(buffer + 34, temp, frame_len - 14);

    int new_len = frame_len + 20; // 新的包的长度
    int sock_fd; // 套接字描述符
    // 创建套接字并处理创建套接字失败的情况
    if ((sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
    { 
        printf("error when create raw socket\n");
        exit(-1);
    }
    sendto(sock_fd, buffer, new_len, 0, (struct sockaddr*) &device, sizeof(device));
    close(sock_fd); // 关闭套接字描述符
}

// 函数名: unpack
// 输入: 存放以太网帧的缓冲区首地址, 帧长度
// 功能: 解ip包并转发
// 输出: 无
void unpack(char *buffer, int frame_len)
{
    char temp[512];
    memcpy(temp, buffer + 34, frame_len - 34); // 暂存除以太网头的剩余包
    memset(buffer, 0, sizeof(char)*frame_len); // buffer清零
    struct ip *ip = (struct ip*)temp; // 强制类型转换为ip头

    char dst_ip[16];
    strcpy(dst_ip, inet_ntoa(ip->ip_dst));

    uint8_t src_mac[6]; // 源mac地址
	uint8_t dst_mac[6]; // 目的mac地址
    struct sockaddr_ll device; // 配合sendto()函数来发送以太网帧
    char *interface = get_dst_mac(dst_mac, dst_ip); // 获取目的mac地址, 源端口号
    get_src_mac(src_mac, interface); // 获取源mac地址
    fill_sockaddr_ll(&device, src_mac, interface); // 填充sockaddr_ll结构
    // 填充以太网头部
    memcpy(buffer, dst_mac, 6);
	memcpy(buffer + 6, src_mac, 6);
    buffer[12] = 0x08;
	buffer[13] = 0x00;
    // 填充ip
    memcpy(buffer + 14, temp, frame_len - 34);
    int new_len = frame_len - 20;

    int sock_fd; // 套接字描述符
    // 创建套接字并处理创建套接字失败的情况
    if ((sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
    { 
        printf("error when create raw socket\n");
        exit(-1);
    }
    sendto(sock_fd, buffer, new_len, 0, (struct sockaddr*) &device, sizeof(device));
    close(sock_fd); // 关闭套接字描述符
}

// 函数名: maccmp
// 输入: mac数组和mac字符串
// 功能: 比较两种形式下的mac地址是不是同一个mac地址
// 输出: 是返回1, 否则返回0
int maccmp(uint8_t mac1[6], char *mac2)
{
    uint8_t tmp[6];
    // 使用sscanf函数转换字符串
    sscanf(mac2, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", 
    &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]); 

    for (int i = 0; i < 6; i++)
        if (tmp[i] != mac1[i])
            return 0;
    
    return 1;
}

// 函数名: judge
// 输入: 存放以太网帧的缓冲区
// 功能: 判断抓到的包的情况
// 输出: 1表示从内部到外部的包, 2表示从外部到内部的包, 0表示其他情况
int judge(char *buffer)
{
    // 获取目的mac
    uint8_t dst_mac[6];
    memcpy(dst_mac, buffer, 6);
    
    for (int i = 0; i < MAX_DEVICE_SIZE; i++)
    {
        if (maccmp(dst_mac, device_table[i].mac_addr))
        {
            if (device_table[i].is_entrance == 0) // 外部来的包
                return 2;
            else if (device_table[i].is_entrance == 1) // 内部来的包
                return 1;
        }
    }
    return 0;
}

// 函数名: IP_analyse
// 输入: 存放以太网帧的缓冲区首地址, 帧长度
// 功能: 解析上层协议为IP的以太网帧并进行下一步操作
// 输出: 无
void IP_analyse(char* buffer, int frame_len)
{
    struct ip *ip = (struct ip*)(buffer + 14); // 强制类型转换为ip头

    char dst_ip[16];
    char src_ip[16];
    strcpy(dst_ip, inet_ntoa(ip->ip_dst));
    strcpy(src_ip, inet_ntoa(ip->ip_src));
 
    printf("receive a packet from %s to %s\n", src_ip, dst_ip); // 打印信息

    int state = judge(buffer); // 判断是从内部到外部的包还是从外部到内部的包

    if (state == 1) // 从内部到外部, 需要额外加包头
    {
        printf("vpn server repack the packet\n");
        repack(buffer, frame_len);
    }

    else if (state == 2) // 从外部到内部, 需要解包
    {
        printf("vpn server unpack the packet\n");
        unpack(buffer, frame_len);
    }

    else // 其他情况不处理
        printf("no need for processing, drop the packet\n");
}

// 函数名: ARP_analyse
// 输入: 存放以太网帧的缓冲区首地址, 帧长度
// 功能: 解析上层协议为ARP的以太网帧并进行下一步操作
// 输出: 无
void ARP_analyse(char* buffer, int frame_len)
{
    printf("\n");
    /*
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
    */
}

// 函数名: capture_pkt
// 输入: 无
// 功能: 解析捕获的包, 并根据解析情况进一步操作
// 输出: 无
void capture_pkt()
{
    int sock_fd; // 套接字描述符
    int nr_recv; // 接受到的报文的字节数
    char buffer[BUFFER_MAX]; // 存放报文的缓冲区

    if ((sock_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
    { 
        printf("error when create raw socket\n");
        exit(-1);
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

        // 判断数据报类型并进行相应处理
        uint8_t *p = buffer + 12;
        uint16_t type = (p[0] << 8) + p[1];
        switch (type)
        {
        case IP: printf("type: IP (0x%04x)\n", type); IP_analyse(buffer, nr_recv); break;
        case ARP: printf("type: ARP (0x%04x)\n", type); ARP_analyse(buffer, nr_recv); break;
        // 似乎现在没有RARP类型的帧, 仅仅是根据ARP协议中的操作码加以区分, 若遇到该类型帧, 直接使用ARP_analyse函数进行解析
        case RARP: printf("type: RARP (0x%04x)\n", type); ARP_analyse(buffer, nr_recv); break;
        default: printf("unknown type(0x%04x)\n", type); 
        }
        printf("\n");
    }
}

int main(int argc, char * argv[])
{
    read_file(); // 读取配置文件

    capture_pkt(); // 抓包解析

    return -1;
}