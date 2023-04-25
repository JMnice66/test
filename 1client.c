#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_SIZE 1048576  // 1MB
#define SERVER_IP "127.0.0.1"  // 服务器 IP 地址
#define SERVER_PORT 8888  // 服务器端口号

int main() {
    // 创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        exit(1);
    }

    // 设置服务器地址
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    servaddr.sin_port = htons(SERVER_PORT);

    // 连接服务器
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
        exit(1);
    }

    // 获取用户输入的数据
    char data_buf[MAX_SIZE];
    printf("Please enter the data to send:\n");
    fgets(data_buf, sizeof(data_buf), stdin);
    data_buf[strlen(data_buf) - 1] = '\0';  // 去除末尾的换行符

    // 计算 MD5 值
    unsigned char md5_value[MD5_DIGEST_LENGTH];
    MD5((const unsigned char *)data_buf, strlen(data_buf), md5_value);

    // 将数据和 MD5 值合并为一个字符串
    char send_buf[MAX_SIZE + MD5_DIGEST_LENGTH];
    memcpy(send_buf, data_buf, strlen(data_buf));
    memcpy(send_buf + strlen(data_buf), md5_value, MD5_DIGEST_LENGTH);

    // 发送数据和 MD5 值
    int n = send(sockfd, send_buf, strlen(data_buf) + MD5_DIGEST_LENGTH, 0);
    if (n < 0) {
        perror("send error");
        exit(1);
    }

    printf("Data sent successfully.\n");

    // 关闭套接字
    close(sockfd);

    return 0;
}
