# Unix 进程

## 进程基本概念

- **程序(Program)**: 存储在文件中的可执行代码和数据
- **映像(Image)**: 程序执行时的计算机执行环境
- **进程(Process)**: 正在执行的程序映像

Unix系统可以同时执行多个进程，这是其多任务能力的体现。

## 进程创建机制

Unix有两个关键的系统调用用于进程创建：

1. **fork()系统调用**
   - 克隆当前进程，创建一个几乎完全相同的子进程
   - 采用写时复制(Copy-on-Write)技术提高效率
   - 复制父进程的工作目录、打开文件等上下文信息

2. **exec()系统调用**
   - 替换当前进程的映像为新的程序
   - 通常紧跟在fork()之后使用

## 典型应用模式

最常见的模式是**fork-exec组合**，如shell程序的示例所示：

```c
while(1) {
    display_prompt();
    read_input(cmd, params);
    pid = fork();      /* 创建子进程 */
    if (pid != 0)
        waitpid(-1, &stat, 0);  /* 父进程等待子进程结束 */
    else
        execve(cmd, params, 0); /* 子进程执行新程序 */
}
```

# Process Information Maintained

---

### **1. 进程信息示例**
#### **查看进程信息**
在 Linux 中，可以使用 `ps` 命令查看进程信息：
```bash
ps -ef
```
输出示例：
```
UID        PID  PPID  C STIME TTY          TIME CMD
root         1     0  0 10:00 ?        00:00:01 /sbin/init
user1     1234  1001  0 10:05 pts/0    00:00:00 bash
user1     5678  1234  0 10:06 pts/0    00:00:00 vim file.txt
```
- **PID (Process ID)**：进程的唯一标识，如 `1234`（bash）、`5678`（vim）。
- **PPID (Parent Process ID)**：父进程 ID，如 `bash` 的父进程是 `1001`（可能是终端进程）。
- **UID (User ID)**：进程的真实用户 ID（如 `user1`）。
- **PGID (Process Group ID)**：进程组 ID，通常与 shell 会话相关，可以用 `ps -o pid,pgid,cmd` 查看。

#### **文件描述符表**
每个进程都有一个文件描述符表，记录打开的文件、套接字等：
```bash
ls -l /proc/$$/fd  # 查看当前 shell 的文件描述符
```
输出示例：
```
0 -> /dev/pts/0   # 标准输入（stdin）
1 -> /dev/pts/0   # 标准输出（stdout）
2 -> /dev/pts/0   # 标准错误（stderr）
255 -> /dev/pts/0 # 可能用于 shell 内部使用
```

---

### **2. umask 示例**
`umask` 决定新创建文件的默认权限：
```bash
umask 022  # 设置 umask
touch newfile.txt
ls -l newfile.txt
```
输出：
```
-rw-r--r-- 1 user user 0 Jun  7 11:22 newfile.txt
```
- `umask 022` 表示：
  - 文件的默认权限是 `666 - 022 = 644`（`rw-r--r--`）。
  - 目录的默认权限是 `777 - 022 = 755`（`rwxr-xr-x`）。

---

### **3. 真实用户 ID（Real UID） vs. 有效用户 ID（Effective UID）**
#### **查看进程的 UID**
```bash
ps -eo pid,ruid,euid,cmd | grep bash
```
输出示例：
```
1234  1000  1000  bash
```
- **Real UID (RUID)**：启动进程的用户（如 `1000`）。
- **Effective UID (EUID)**：进程当前权限（默认与 RUID 相同，但 `setuid` 程序会改变它）。

---

### **4. setuid 机制示例**
#### **典型例子：`passwd` 命令**
`passwd` 允许普通用户修改 `/etc/shadow`（通常只有 root 可写）：
```bash
ls -l /usr/bin/passwd
```
输出：
```
-rwsr-xr-x 1 root root 59976 Nov 24  2022 /usr/bin/passwd
```
- **`s` 标志**：表示 `setuid` 位已设置，执行时进程的 **EUID** 变为 `root`（文件所有者）。
- 因此，普通用户运行 `passwd` 时，临时获得 `root` 权限，可以修改 `/etc/shadow`。

#### **自己写一个 setuid 程序**
假设有一个 C 程序 `test_setuid.c`：
```c
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());
    return 0;
}
```
编译并设置 `setuid`：
```bash
gcc test_setuid.c -o test_setuid
sudo chown root:root test_setuid  # 更改所有者
sudo chmod u+s test_setuid        # 设置 setuid
./test_setuid
```
输出：
```
Real UID: 1000     # 仍然是当前用户
Effective UID: 0   # 但 EUID 是 root（文件所有者）
```

---

### **5. 环境变量示例**
环境变量影响进程行为：
```bash
export MY_VAR="hello"  # 设置环境变量
env | grep MY_VAR      # 查看当前环境变量
```
输出：
```
MY_VAR=hello
```
- 子进程（如 `bash` 启动的 `python`、`gcc`）会继承父进程的环境变量。

---

### **6. 进程组（PGID）示例**
```bash
sleep 100 &  # 后台运行 sleep
ps -o pid,pgid,cmd
```
输出：
```
PID   PGID  CMD
1234  1234  bash
5678  1234  sleep 100
```
- `sleep` 和 `bash` 属于同一个进程组（PGID=`1234`，即 shell 的 PID）。
- 可以用 `kill -TERM -1234` 终止整个进程组（包括 shell 和子进程）。

---

### **总结**
| **概念** | **示例** | **说明** |
|----------|----------|----------|
| **PID/PPID** | `ps -ef` | 进程 ID 和父进程 ID |
| **文件描述符** | `ls -l /proc/$$/fd` | 进程打开的文件、管道等 |
| **umask** | `umask 022` | 控制新文件的默认权限 |
| **Real UID / EUID** | `passwd` 命令 | `setuid` 让进程临时提权 |
| **setuid** | `chmod u+s` | 使程序以所有者权限运行 |
| **环境变量** | `export VAR=value` | 影响进程行为 |
| **进程组** | `kill -TERM -PGID` | 管理一组相关进程 |


# Unix Signal机制详解

## 什么是Signal

Signal是Unix/Linux系统中进程间通信的一种基本机制，用于通知进程发生了某种异步事件。它可以被看作是一种软件中断，允许进程或内核中断其他进程的正常执行流程。

## Signal的主要特点

1. **异步通知**：Signal可以在任何时候发送给进程
2. **预定义类型**：系统定义了一系列标准signal类型
3. **轻量级**：比进程间通信的其他方式(如管道、套接字)更轻量
4. **有限信息**：只能传递signal编号，不能携带更多数据

## 常见Signal类型

| Signal | 值 | 默认动作 | 说明 |
|--------|----|----------|------|
| SIGHUP | 1 | Term | 终端挂起或控制进程终止 |
| SIGINT | 2 | Term | 键盘中断(Ctrl+C) |
| SIGQUIT | 3 | Core | 键盘退出(Ctrl+\) |
| SIGKILL | 9 | Term | 强制终止(不能被捕获或忽略) |
| SIGSEGV | 11 | Core | 无效内存引用 |
| SIGTERM | 15 | Term | 终止信号(可以被捕获) |
| SIGCHLD | 17 | Ign | 子进程状态改变 |

## Signal处理方式

1. **默认处理**：执行系统预定义的动作
2. **忽略信号**：完全忽略该信号
3. **捕获信号**：注册信号处理函数，当信号发生时调用

## 示例：捕获SIGINT信号(Ctrl+C)

下面是一个C语言示例，展示如何捕获SIGINT信号并执行自定义处理：

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
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
    while(1) {
        sleep(1);
    }
    
    return 0;
}
```

### 示例说明

1. 程序首先使用`signal()`函数注册了一个SIGINT信号的处理函数`sigint_handler`
2. 当用户第一次按下Ctrl+C时，会调用`sigint_handler`函数而不是终止程序
3. 在处理函数中，我们将SIGINT的处理恢复为默认行为
4. 当用户第二次按下Ctrl+C时，程序会正常终止

### 编译运行

1. 将代码保存为`sigexample.c`
2. 编译：`gcc sigexample.c -o sigexample`
3. 运行：`./sigexample`


# Unix管道(pipe)的底层实现原理

Unix管道是操作系统内核提供的一种进程间通信(IPC)机制，其底层实现涉及多个关键概念和数据结构。让我们深入探讨其工作原理：

## 1. 基本数据结构

当创建一个管道时，内核会建立以下结构：

- **两个文件描述符**：一个用于读取(`pipefd[0]`)，一个用于写入(`pipefd[1]`)
- **环形缓冲区**：内核内存中的一个固定大小的缓冲区(通常为4KB-64KB)
- **等待队列**：当管道为空或满时，用于阻塞等待的进程队列

## 2. 系统调用层面

管道通过以下系统调用实现：

```c
int pipe(int pipefd[2]);
```

这个系统调用会：
1. 在内核空间创建一个管道对象
2. 返回两个文件描述符给用户空间
3. 这两个描述符指向同一个管道对象

## 3. 内核实现细节

### 3.1 数据结构

在Linux内核中，管道主要涉及：

```c
struct pipe_inode_info {
    wait_queue_head_t wait;  // 等待队列
    unsigned int nrbufs;     // 未读缓冲区数量
    struct pipe_buffer bufs[PIPE_BUFFERS]; // 缓冲区数组
    // ...其他字段
};
```

### 3.2 读写操作

**写入过程**：
1. 进程调用`write()`系统调用
2. 内核检查管道是否有空闲缓冲区
   - 如果满，写入进程被放入等待队列(阻塞)
3. 数据被复制到内核缓冲区
4. 唤醒等待读取的进程

**读取过程**：
1. 进程调用`read()`系统调用
2. 内核检查管道是否有数据
   - 如果空，读取进程被放入等待队列(阻塞)
3. 数据从内核缓冲区复制到用户空间
4. 唤醒等待写入的进程

### 3.3 缓冲区管理

- 使用环形缓冲区设计，提高空间利用率
- 采用生产者-消费者模型
- 当缓冲区满时，写入操作会阻塞
- 当缓冲区空时，读取操作会阻塞

## 4. 关键特性实现

### 4.1 阻塞与非阻塞模式

管道可以设置为非阻塞模式(通过`fcntl`设置`O_NONBLOCK`)：
- 非阻塞模式下，读/写操作会立即返回而不等待
- 通过`EAGAIN`错误码表示暂时无法完成操作

### 4.2 原子性保证

- 小于`PIPE_BUF`(通常4096字节)的写入是原子的
- 多个进程同时写入时，小写入不会被交错

### 4.3 管道容量限制

- 默认缓冲区大小由`/proc/sys/fs/pipe-max-size`决定
- 可以通过`fcntl`的`F_SETPIPE_SZ`操作动态调整

## 5. 与Shell管道的关联

当Shell执行`cmd1 | cmd2`时：
1. Shell调用`pipe()`创建管道
2. 调用`fork()`创建两个子进程
3. 在子进程1中：
   - 关闭管道的读端
   - 将stdout重定向到管道的写端(`dup2(pipefd[1], STDOUT_FILENO)`)
   - 执行cmd1
4. 在子进程2中：
   - 关闭管道的写端
   - 将stdin重定向到管道的读端(`dup2(pipefd[0], STDIN_FILENO)`)
   - 执行cmd2

## 6. 性能考虑

- 数据在内核和用户空间之间需要拷贝两次(写和读各一次)
- 零拷贝技术(如Linux的`splice`系统调用)可以优化这种开销
- 管道通信比临时文件效率高，因为不涉及磁盘I/O

管道这种简单的抽象背后是精心设计的内核机制，体现了Unix"简单而强大"的设计哲学。