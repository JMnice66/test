#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

#define PORT 8888
#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    struct sockaddr_in serv_addr;
    int sock;
    char buf[BUF_SIZE];
    int send_size;
    int i;
    int cnt = 0;
    struct timeval start_time, end_time;


    // 创建UDP套接字
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(1);
    }

    // 设置服务器地址和端口号
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 将IP地址设置为服务器的IP地址
    serv_addr.sin_port = htons(PORT); // 将端口号设置为服务器监听的端口号

    // 开始循环发送数据
    gettimeofday(&start_time, NULL);
    while (1) {
        send_size = atoi(argv[1]); // 获取数据大小
        memset(buf, 'a', send_size); // 填充数据，这里使用字符'a'填充
        if (sendto(sock, buf, send_size, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) { // 发送数据
            perror("sendto");
            exit(1);
        }
        cnt++; // 统计发送次数
        gettimeofday(&end_time, NULL);
        if ((end_time.tv_sec - start_time.tv_sec) > 0) { // 每秒钟输出一次吞吐量
            printf("send rate: %ld Mbps\n", cnt * send_size * 8 / (end_time.tv_sec - start_time.tv_sec) / 1000000);
            cnt = 0;
            gettimeofday(&start_time, NULL);
        }
    }

    close(sock);
    return 0;
}
