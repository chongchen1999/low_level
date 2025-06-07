#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    char command[100];
    char* args[10]; // 假设命令最多有10个参数
    int status;

    while (1) {
        printf("myshell> ");
        fgets(command, sizeof(command), stdin);

        // 移除换行符
        command[strcspn(command, "\n")] = '\0';

        // 如果是 exit 命令则退出
        if (strcmp(command, "exit") == 0) {
            printf("Goodbye!\n");
            exit(0);
        }

        // 解析命令和参数
        char* token = strtok(command, " ");
        int i = 0;
        while (token != NULL && i < 9) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // execvp 要求参数列表以 NULL 结尾

        // 创建子进程
        pid_t pid = fork();

        if (pid < 0) {
            // fork 失败
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
            // 子进程
            execvp(args[0], args);
            // 如果 execvp 返回，说明执行失败
            perror("execvp failed");
            exit(1);
        } else {
            // 父进程等待子进程结束
            waitpid(pid, &status, 0);

            if (WIFEXITED(status)) {
                printf("Child process exited with status %d\n",
                       WEXITSTATUS(status));
            } else {
                printf("Child process terminated abnormally\n");
            }
        }
    }

    return 0;
}