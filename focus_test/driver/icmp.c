#include <linux/net.h>
#include <linux/uaccess.h>
#include <uapi/linux/icmp.h>
#include <linux/inet.h>
#include <net/sock.h>

static struct socket *sock;
static struct sockaddr_in addr_in;
static struct icmphdr icmp_hdr;
static struct msghdr msg;
static struct iovec iov;
static mm_segment_t oldfs;
static int ret_val = 0;

void icmp_test(void)
{
    char data[sizeof(icmp_hdr) + 5];
    if((ret_val = sock_create(AF_INET, SOCK_DGRAM, IPPROTO_ICMP, &sock)) < 0){
	printk("Create socket for ICMP failed.%d\n", ret_val);
        return;
    }
    
    //kernel_setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &on, sizeof(int));
    //kernel_setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = in_aton("192.168.199.1");

    memset(&icmp_hdr, 0, sizeof(icmp_hdr));
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.un.echo.id = 1234;
    icmp_hdr.un.echo.sequence = 1;

    //lock_sock(sock->sk);
    //sock->sk->broadcast = 1;
    //release_sock(sock->sk);
    memcpy(data, &icmp_hdr, sizeof icmp_hdr);
    memcpy(data + sizeof icmp_hdr, "hello", 5);
    
    iov.iov_base = data;
    iov.iov_len = sizeof(data);
    msg.msg_name = &addr_in;
    msg.msg_namelen = sizeof(struct sockaddr_in);
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_iov = &iov;


    oldfs = get_fs(); 
    set_fs(KERNEL_DS);
    sock_sendmsg(sock, &msg, sizeof(icmp_hdr)+5);
    set_fs(oldfs);
    //sock_release(sock);
}
