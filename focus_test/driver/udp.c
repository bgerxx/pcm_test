#include <linux/net.h>

static struct socket *sock;
static struct sockaddr_in addr_in;
static struct msghdr msg;
static struct iovec iov;
static char *buf = "test";
static int ret_val = 0;

void udp_test(void)
{
    if((ret_val = sock_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &sock)) < 0){
	printk("Create socket for UDP failed.");
        return;
    }
    
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(19999);
    addr_in.sin_addr.s_addr = in_aton("127.0.0.1");
    if ((ret_val = sock.ops->bind(sock, (struct sockaddr *)&addr_in, sizeof(struct sockaddr_in))) < 0){
        printk("Bind socket for UDP failed.");
        return;
    }

    memset(&msg, 0, sizeof(msg));
    msg.msg_flags = 0;
    msg.msg_name = &addr_in;
    msg.msg_namelen  = sizeof(struct sockaddr_in);
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_iov = &iov;

    lock_sock(sock->sk);
    sock->sk->broadcast = 1;
    release_sock(sock->sk);

    iov.iov_base = buf;
    iov.iov_len = strlen(buf);

    oldfs = get_fs(); 
    set_fs(KERNEL_DS);
    retval = sock_sendmsg(&sock, &msg, len);
    set_fs(oldfs);
}
