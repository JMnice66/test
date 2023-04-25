#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_SIZE 1048576  // 1MB
#define SERVER_IP "0.0.0.0"  // 本地 IP 地址
#define SERVER_PORT 8888  // 服务器端口号

int main() {
    // 创建套接字
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("socket error");
        exit(1);
    }

    // 设置本地地址
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);

    // 绑定本地地址
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind error");
        exit(1);
    }

    // 开始监听
    if (listen(listenfd, 10) < 0) {
        perror("listen error");
        exit(1);
    }

    printf("Listening on %s:%d...\n", SERVER_IP, SERVER_PORT);

    while (1) {
        // 接受客户端连接
        struct sockaddr_in cliaddr;
        socklen_t cliaddrlen = sizeof(cliaddr);
        int connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddrlen);
        if (connfd < 0) {
            perror("accept error");
            continue;
        }

        // 接收数据和 MD5 值
        char recv_buf[MAX_SIZE + MD5_DIGEST_LENGTH];
        int n = recv(connfd, recv_buf, sizeof(recv_buf), 0);
        if (n < 0) {
            perror("recv error");
            continue;
        }

        // 分离数据和 MD5 值
        char *data_buf = recv_buf;
        char *md5_buf = recv_buf + n - MD5_DIGEST_LENGTH;

        // 计算接收到的数据的 MD5 值
        unsigned char md5_value[MD5_DIGEST_LENGTH];
        MD5((const unsigned char *)data_buf, strlen(data_buf), md5_value);

        // 比较接收到的 MD5 值和计算出的 MD5 值是否一致
        if (memcmp(md5_value, md5_buf, MD5_DIGEST_LENGTH) != 0) {
            printf("MD5 values do not match.\n");
        } else {
            printf("Data received: %s\n", data_buf);
        }

        // 关闭连接
        close(connfd);
    }

    // 关闭套接字
    close(listenfd);

    return 0;
}
