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

int main() {
    int sock;
    struct sockaddr_in serv_addr, clnt_addr;

    sock = socket(AF_INET, SOCK_DGRAM, 0); // 创建UDP套接字
    if (sock == -1) {
        perror("socket");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) { // 绑定UDP套接字
        perror("bind");
        exit(1);
    }

    char* buf = (char*)malloc(BUF_SIZE * sizeof(char)); // 分配缓冲区
    int n;
    int count = 0, loss = 0;
    long long sum_size = 0;
    struct timeval start_time, end_time;

    while (count < 10000) { // 循环接收数据
        n = recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr*)&clnt_addr, NULL); // 接收数据
        if (n == -1) {
            perror("recvfrom");
            exit(1);
        }
        sum_size += n; // 累加接收到的数据大小
        gettimeofday(&start_time, NULL); // 记录开始时间
        n = sendto(sock, buf, n, 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr)); // 发送数据
        if (n == -1) {
            perror("sendto");
            exit(1);
        }
        gettimeofday(&end_time, NULL); // 记录结束时间
        if (end_time.tv_sec - start_time.tv_sec > 1 || (end_time.tv_sec - start_time.tv_sec == 1 && end_time.tv_usec < start_time.tv_usec)) { // 判断是否丢包
            loss++;
        }
        count++;
    }

    printf("Average throughput: %.3f Mbps\n", (double)sum_size * 8 / 1000000); // 计算平均吞吐量并输出
    printf("Packet loss rate: %.3f%%\n", (double)loss / count * 100); // 计算丢包率并输出

    free(buf);
    close(sock);
    return 0;
}
