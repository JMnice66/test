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
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    int sock;
    char buf[BUF_SIZE];
    int recv_size;
    int i;
    int cnt = 0;
    int loss_cnt = 0;
    struct timeval start_time, end_time;

    if (argc != 2) {
        printf("Usage: %s <data size>\n", argv[0]);
        exit(1);
    }

    // 创建UDP套接字
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(1);
    }

    // 绑定本地地址和端口号接收端代码：
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 将IP地址设置为INADDR_ANY，表示接收所有IP地址的数据
    serv_addr.sin_port = htons(PORT); // 将端口号设置为监听的端口号

    if (bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) { // 将套接字和本地地址绑定
        perror("bind");
        exit(1);
    }

    clnt_addr_size = sizeof(clnt_addr);

    // 开始循环接收数据
    gettimeofday(&start_time, NULL);
    while (1) {
        recv_size = recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size); // 接收数据
        if (recv_size == -1) {
            perror("recvfrom");
            exit(1);
        }
        cnt++; // 统计接收次数
        if (recv_size != atoi(argv[1])) { // 统计丢包次数
            loss_cnt++;
        }
        gettimeofday(&end_time, NULL);
        if ((end_time.tv_sec - start_time.tv_sec) > 0) { // 每秒钟输出一次吞吐量和丢包率
            printf("recv rate: %ld Mbps, loss rate: %d%%\n", cnt * atoi(argv[1]) * 8 / (end_time.tv_sec - start_time.tv_sec) / 1000000, loss_cnt * 100 / cnt);
            cnt = 0;
            loss_cnt = 0;
            gettimeofday(&start_time, NULL);
        }
    }

    close(sock);
    return 0;
}
