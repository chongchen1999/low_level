#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 信号处理函数
void sigint_handler(int sig) {
    printf("\n捕获到SIGINT信号(%d)，但我不退出!\n", sig);
    printf("再按一次Ctrl+C将退出程序\n");

    // 恢复默认处理，这样下次再按Ctrl+C就会退出
    signal(SIGINT, SIG_DFL);
}

int main() {
    // 注册信号处理函数
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("无法设置信号处理");
        exit(EXIT_FAILURE);
    }

    printf("程序运行中... 按下Ctrl+C试试\n");

    // 无限循环，保持程序运行
    while (1) {
        sleep(1);
    }

    return 0;
}