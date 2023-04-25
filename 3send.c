#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

#define BUF_SIZE 1024 // 缓冲区大小
#define PORT 8888 // 端口号

int main(int argc, char* argv[]) {

    int sock;
    struct sockaddr_in serv_addr;

    sock = socket(AF_INET, SOCK_DGRAM, 0); // 创建UDP套接字
    if (sock == -1) {
        perror("socket");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 目标IP地址
    serv_addr.sin_port = htons(PORT); // 目标端口号

    char* buf = (char*)malloc(atoi(argv[1]) * sizeof(char)); // 分配缓冲区
    memset(buf, 'a', atoi(argv[1])); // 将缓冲区初始化为'a'

    int i, n;
    struct timeval start_time, end_time;
    long long sum_time = 0;

    for (i = 0; i < atoi(argv[2]); i++) { // 循环发送数据
        gettimeofday(&start_time, NULL); // 记录开始时间
        n = sendto(sock, buf, atoi(argv[1]), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); // 发送数据
        if (n == -1) {
            perror("sendto");
            exit(1);
        }
        n = recvfrom(sock, buf, atoi(argv[1]), 0, NULL, NULL); // 接收数据
        if (n == -1) {
            perror("recvfrom");
            exit(1);
        }
        gettimeofday(&end_time, NULL); // 记录结束时间
        sum_time += (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec); // 计算时间差并累加
    }

    printf("Average delay: %.3f us\n", (double)sum_time / atoi(argv[2]) / 2); // 计算平均时延并输出

    free(buf);
    close(sock);
    return 0;
}
