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
} device_table[MAX_DEVICE_SIZE];
// the sum of the items in the device table
int device_item_nr = 0;


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

// 函数名: test
// 输入: 无
// 功能: 测试配置文件是否读取成功
// 输出: 无
void test()
{
    printf("\ndevice table has %d items\n", device_item_nr);
    for (int i = 0; i < device_item_nr; i++)
        printf("%s %s %s %s\n", device_table[i].interface, device_table[i].mac_addr, device_table[i].ip_addr, device_table[i].netmask);
    
    printf("\narp table has %d items\n", arp_item_nr);
    for (int i = 0; i < arp_item_nr; i++)
        printf("%s %s\n", arp_table[i].ip_addr, arp_table[i].mac_addr);

    printf("\nroute table has %d items\n", route_item_nr);
    for (int i = 0; i < route_item_nr; i++)
        printf("%s %s %s %s\n", route_table[i].destination, route_table[i].gateway, route_table[i].netmask, route_table[i].interface);
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

// 函数名: in_same_net
// 输入: 目的ip
// 功能: 判断目的ip和主机在不在同一个子网下
// 输出: 同一个子网返回对应表项下标, 非同一个子网返回-1
int in_same_net(char *dst_ip_addr)
{
    uint32_t ip1 = inet_addr(dst_ip_addr);
    for (int i = 0; i < route_item_nr; i++)
    {
        uint32_t ip2 = inet_addr(route_table[i].destination);
        uint32_t mask = inet_addr(route_table[i].netmask);
        if ((ip1&mask) == (ip2&mask))
        {
            if (!strcmp(route_table[i].gateway, "0.0.0.0"))
                return i;
            return -1;
        }
    }
    printf("destination unreachable\n");
    exit(-1);
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
           return route_table[i].gateway;
        }
    }
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
// 功能: 填充源mac地址, 获得源ip地址
// 输出: 源ip
char* search_device(uint8_t src[6], char *interface)
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

            // 返回源ip
            return device_table[i].ip_addr;
        }
}

// 函数名: get_dst_mac
// 输入: 存放目的mac的数组, 目的ip
// 功能: 根据目的ip填充存放mac的数组
// 输出: 源端口
char* get_dst_mac(uint8_t dst[6], char *dst_ip_addr)
{
    char *ip_next_hop = dst_ip_addr;
    char *interface; // 源端口号
    int index = in_same_net(dst_ip_addr); // 判断是否同子网
    if (index != -1) // 目的ip与主机处于同一子网
    {
        interface = route_table[index].interface;
        printf("in same net, search arp table directly\n");
    }
        
    else // 目的ip与主机不在同一子网
    {
        printf("not in same net, search ip_addr of next hop\n");
        ip_next_hop = search_route(dst_ip_addr, &interface); // 查路由表获得下一跳ip, 端口号
    }

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
// 输出: 源ip
char* get_src_mac(uint8_t src[6], char *interface)
{
   return search_device(src, interface);
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

// 函数名: fill_icmp
// 输入: 指向待填充区域的指针
// 功能: 填充icmp包
// 输出: icmp包的长度
int fill_icmp(uint8_t *icmp_buffer)
{
    struct icmp *icmp = (struct icmp*)(icmp_buffer); // icmp为ICMP头部结构体
    int len = 0; // icmp包头的长度
 
    icmp->icmp_type = ICMP_ECHO; // 初始化ICMP消息类型type
    
    icmp->icmp_code = 0; // 初始化消息代码code
    icmp->icmp_id = getpid(); // 把进程号给icmp_id
    icmp->icmp_seq = 1; // 发送的ICMP消息序号赋值给icmp序号

    char icmp_str[] = "Welcome to the Hotel California^_^";
    memcpy(icmp->icmp_data, icmp_str, strlen(icmp_str)); // 用icmp_str当作发送的数据

    len = 8 + strlen(icmp_str); // 8是包头其他部分的长度

    icmp->icmp_cksum = 0; // 初始化校验和
    icmp->icmp_cksum = checksum((char*)icmp, len); // 计算校验和

    return len;
}

// 函数名: fill_ip
// 输入: 指向待填充区域的指针, 目的ip, 源ip
// 功能: 填充ip包
// 输出: ip包的长度
int fill_ip(uint8_t *ip_buffer, char *dst_ip_addr, char *src_ip)
{
    struct ip *ip = (struct ip*)ip_buffer;

    ip->ip_hl = 5; // ip头长度为5个4字节
    ip->ip_v = 4; // for ipv4
    ip->ip_tos = 0x00; // 服务类型
    uint16_t ip_len = 20 + fill_icmp(ip_buffer + 20);
    ip->ip_len = htons(ip_len);
    ip->ip_id = getpid();
    ip->ip_off = IP_DF; // 不分片
    ip->ip_ttl = 64; // tll = 64
    ip->ip_p = 1; // 上层协议为icmp, 对应编号为1
   
    // 填充源和目的ip地址
    ip->ip_src.s_addr = inet_addr(src_ip);
    ip->ip_dst.s_addr = inet_addr(dst_ip_addr);

    ip->ip_sum = 0;
    ip->ip_sum = checksum((char*)ip, sizeof(struct ip)); // 检验和

    return ip_len;
}

// 函数名: fill_frame
// 输入: 指向待填充以太网帧的指针, 存放源MAC地址的数组, 存放目的MAC地址的数组, 目的ip, 源ip
// 功能: 填充以太网帧
// 输出: 以太网帧的长度
int fill_frame(uint8_t *ether_frame, uint8_t src_mac[6], uint8_t dst_mac[6], char *dst_ip_addr, char *src_ip)
{
    // 填充以太网帧头部
    memcpy(ether_frame, dst_mac, 6);
	memcpy(ether_frame + 6, src_mac, 6);
    ether_frame[12] = 0x08;
	ether_frame[13] = 0x00;

    return 14 + fill_ip(ether_frame + 14, dst_ip_addr, src_ip);
}

// 函数名: send_frame
// 输入: 目的IP地址
// 功能: 封装以太网帧并向目的IP发送
// 输出: 无
void send_frame(char *dst_ip_addr)
{
    int sock_fd; // 套接字描述符
    // 创建套接字并处理创建套接字失败的情况
    if ((sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
    { 
        printf("error when create raw socket\n");
        exit(-1);
    }

	uint8_t src_mac[6]; // 源mac地址
	uint8_t dst_mac[6]; // 目的mac地址
	uint8_t ether_frame[BUFFER_MAX]; // 待发送的以太网帧
    int frame_len = 0; // 记录以太网帧的长度
	struct sockaddr_ll device; // 配合sendto()函数来发送以太网帧

    
    char *interface = get_dst_mac(dst_mac, dst_ip_addr); // 获取目的mac地址, 源端口号

    char *src_ip = get_src_mac(src_mac, interface); // 根据源端口号, 获取源mac地址, 源ip地址

    fill_sockaddr_ll(&device, src_mac, interface); // 填充sockaddr_ll结构
    frame_len = fill_frame(ether_frame, src_mac, dst_mac, dst_ip_addr, src_ip); // 填充以太网帧

    // 发送该以太网帧4次
    for (int i = 0 ; i < 4; i++)
    {
        printf("send %d times\n", i);
        sendto(sock_fd, ether_frame, frame_len, 0, (struct sockaddr*) &device, sizeof(device));
    }

    close(sock_fd); // 关闭套接字描述符
}

int main(int argc, char * argv[])
{
    if (argc != 2) // 处理参数个数不正确的情况
    {      
        printf("Usage: %s dest_ip\n", argv[0]);
        return -1;
    }

    read_file(); // 读取配置文件

    send_frame(argv[1]); // 发送数据包
}