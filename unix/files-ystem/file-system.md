# Unix File System

## 打开文件表（Open File Table）和文件描述符表（File Descriptor Table）


在C语言中，可以用以下简化的数据结构来描述 **全局打开文件表** 和 **进程/线程的文件描述符表**：

---

### 1. **全局打开文件表（系统级， 内核维护一个）**
内核维护一个全局链表或数组，每个条目（`struct file`）代表一个被打开的文件实例：
```c
struct file {
    int refcount;          // 引用计数（多个fd可能指向此文件）
    mode_t mode;           // 打开模式（读/写/追加等）
    off_t offset;          // 当前读写偏移量
    struct inode *inode;   // 指向文件的inode（实际文件数据）
    struct file_ops *ops;  // 文件操作函数（read/write等）
    // 其他字段（如锁、状态等）
};

// 全局打开文件表（简化为链表）
struct file *global_file_table;  // 实际可能是哈希表或更复杂的结构
```

---

### 2. **进程的文件描述符表（用户级，每个进程一个）**
每个进程有一个独立的文件描述符表（线程共享该表），通常是一个数组，索引即`fd`：
```c
// 进程的文件描述符表
struct files_struct {
    int count;              // 引用计数（多线程共享时使用）
    struct file **fd_array; // 指向file指针的数组
    int max_fds;           // 当前最大描述符数量
};

// 进程的PCB（简化的部分结构）
struct task_struct {
    // ...
    struct files_struct *files; // 指向该进程的文件描述符表
    // ...
};
```

---

### 3. **关系示意图**
```text
进程A的文件描述符表 (files_struct)
+-----+-----+-----+
| fd0 | fd1 | fd2 |  --> 每个fd指向全局表中的某个file
+-----+-----+-----+
            |
            v
        全局打开文件表 (file)
        +---------------+
        | refcount = 2  |  <-- 可能被多个fd引用
        | mode = O_RDWR |
        | offset = 100   |
        | inode = ...    |
        +---------------+
```

---

### 4. **关键操作示例**
#### 打开文件（`open`系统调用）：
```c
int sys_open(const char *path, int flags) {
    // 1. 根据path找到inode
    struct inode *ino = find_inode(path);
    
    // 2. 创建全局file条目
    struct file *new_file = kmalloc(sizeof(struct file));
    new_file->inode = ino;
    new_file->offset = 0;
    new_file->refcount = 1;
    
    // 3. 添加到全局表（省略具体实现）
    add_to_global_table(new_file);
    
    // 4. 在进程的fd表中分配一个空闲fd
    int fd = allocate_fd(current_task->files);
    current_task->files->fd_array[fd] = new_file;
    
    return fd;
}
```

#### 读取文件（`read`系统调用）：
```c
ssize_t sys_read(int fd, void *buf, size_t count) {
    // 1. 通过fd找到对应的file
    struct file *file = current_task->files->fd_array[fd];
    
    // 2. 调用文件操作的read函数
    ssize_t ret = file->ops->read(file, buf, count);
    
    // 3. 更新偏移量
    file->offset += ret;
    
    return ret;
}
```

---

### 5. 关键点
- **共享性**：多个`fd`（甚至跨进程）可能指向同一个`file`（通过`fork()`或`dup()`）。
- **隔离性**：不同进程的`fd`表独立，相同的`fd`值可能指向不同的文件。
- **线程**：线程默认共享进程的`files_struct`，因此共用相同的`fd`空间。

这个简化模型可以帮助理解Linux/Unix文件系统的核心逻辑，实际内核实现会更复杂（如RCU锁、性能优化等）。


## Standard in/out/err and Devices
这段内容讲述了UNIX系统的一个核心设计理念：**“一切皆文件”**（Everything is a file）。以下是详细解释：

---

### 1. **UNIX的创新：硬件设备也被抽象为文件**
   - 在UNIX中，不仅普通文件（如文本、程序）用文件表示，**硬件设备**（如键盘、打印机、调制解调器）也被视为**特殊文件**，存放在`/dev`目录下。
   - 例如：
     - `/dev/tty`：当前终端（键盘输入和屏幕输出）。
     - `/dev/lpr`：打印机设备。
     - `/dev/modem`：调制解调器设备。
   - 通过这种设计，用户或程序可以用**操作文件的统一方式**（如`read()`、`write()`）与硬件交互，无需关心底层细节。

---

### 2. **标准输入/输出/错误的默认设备**
   - 当一个程序启动时，UNIX会自动打开三个标准流：
     - **标准输入（stdin）**：默认绑定到`/dev/tty`（键盘输入）。
     - **标准输出（stdout）**：默认绑定到`/dev/tty`（屏幕输出）。
     - **标准错误（stderr）**：默认也绑定到`/dev/tty`（屏幕输出错误信息）。
   - 例如，`printf("Hello")`会向`/dev/tty`写入数据，最终显示在终端上。

---

### 3. **为什么这样设计？**
   - **统一接口**：程序无需区分文件、键盘、打印机等，只需用`read()`/`write()`等通用接口。
   - **灵活性**：可以通过重定向（如`>`或`|`）轻松切换输入/输出目标。  
     例如：`command > /dev/lpr`将输出重定向到打印机，而程序代码无需修改。
   - **抽象化**：隐藏硬件差异，简化开发和系统管理。

---

### 示例场景
```bash
# 直接写入终端（等同于echo "Hello"）
echo "Hello" > /dev/tty

# 将错误信息输出到终端（默认行为）
ls /nonexistent 2> /dev/tty
```

---

总结：UNIX通过**“一切皆文件”**的抽象，实现了对硬件和数据的统一管理，这是其简洁性和强大扩展性的关键之一。

## Redirection
上述内容介绍了 **Shell 中的重定向** 功能，主要分为以下几个方面：

### 1. **重定向的基本概念**
- 在命令执行前，可以修改其输入/输出的默认目标（终端）为文件。
- Shell 在子进程中修改文件描述符，而子程序对此无感知。

### 2. **常见的重定向操作**
- **输出重定向 `>`**  
  将命令的输出写入文件（覆盖原有内容）。  
  示例：`ls > my_files`（将 `ls` 结果保存到 `my_files`）。

- **输入重定向 `<`**  
  从文件读取输入而非键盘。  
  示例：`mail mohri < input.data`（用 `input.data` 作为 `mail` 的输入）。

- **追加输出 `>>`**  
  将输出追加到文件末尾（不覆盖）。  
  示例：`date >> logfile`（将日期追加到 `logfile`）。

- **文件描述符重定向（如 `fd>`）**  
  例如 `2>` 重定向标准错误（stderr）。  
  示例：`ls 2> error_log`（将错误信息保存到 `error_log`）。

### 3. **设备文件的重定向**
- 设备（如 `/dev` 目录下的特殊文件）也可作为重定向目标：
  - `/dev/tty`：当前终端。  
  - `/dev/lp`：打印机（需系统支持）。  
  - `/dev/null`：丢弃所有数据（“黑洞”）。  
  示例：  
  - `cat big_file > /dev/lp`（打印文件内容）。  
  - `cat big_file > /dev/null`（丢弃输出）。

### 总结
重定向通过修改输入/输出来灵活控制数据流向（文件或设备），是 Shell 编程中的重要功能。

## Links
在 Unix/Linux 系统中，**链接（Links）** 是一种让多个文件名指向同一个文件的方式，分为 **软连接（Symbolic Link，又称符号链接）** 和 **硬链接（Hard Link）**。

---

### **1. 硬链接（Hard Link）**
- **定义**：硬链接是文件系统中的一个目录项，直接指向文件的 **inode**（索引节点）。多个硬链接可以指向同一个 inode。
- **特点**：
  - 删除原始文件后，只要至少有一个硬链接存在，文件数据仍然存在。
  - 硬链接不能跨文件系统（因为 inode 是文件系统局部的）。
  - 不能对目录创建硬链接（防止循环引用问题）。
  - 所有硬链接地位平等，没有“原始文件”和“链接文件”之分。
- **创建方式**：
  ```bash
  ln source_file hard_link
  ```
- **示例**：
  ```bash
  $ echo "hello" > file1
  $ ln file1 file2       # 创建硬链接 file2
  $ ls -li               # 查看 inode（第一列）
  12345 -rw-r--r-- 2 user group 6 Jan 1 10:00 file1
  12345 -rw-r--r-- 2 user group 6 Jan 1 10:00 file2  # 相同 inode
  $ rm file1             # 删除 file1 后，file2 仍然可访问
  ```

---

### **2. 软连接（Symbolic Link，符号链接）**
- **定义**：软连接是一个特殊的文件，存储的是 **另一个文件的路径**（类似于 Windows 的快捷方式）。
- **特点**：
  - 可以跨文件系统。
  - 可以指向目录。
  - 如果原始文件被删除，软连接会变成“悬空链接”（dangling link），无法访问。
  - 软连接有自己的 inode，与目标文件不同。
- **创建方式**：
  ```bash
  ln -s source_file symbolic_link
  ```
- **示例**：
  ```bash
  $ echo "hello" > file1
  $ ln -s file1 symlink1  # 创建软连接 symlink1
  $ ls -l
  lrwxrwxrwx 1 user group 5 Jan 1 10:00 symlink1 -> file1
  $ rm file1             # 删除 file1 后，symlink1 失效
  ```

---

### **3. 文件可以没有链接吗？**
- **可以**，但正常情况下不会长期存在：
  - 当一个文件的 **链接计数（link count）** 降为 0 时（即没有硬链接指向它的 inode），文件系统会释放其存储空间。
  - 但如果某个进程仍打开该文件（比如程序正在读取它），文件数据会暂时保留，直到所有进程关闭它（Linux 的 `lsof` 可查看这类文件）。
  - 这种状态的文件称为 **“孤立的文件”**（unlinked but still open）。

---

### **对比总结**
| 特性                | 硬链接                     | 软连接                     |
|---------------------|--------------------------|--------------------------|
| **指向目标**        | inode                    | 文件路径                  |
| **跨文件系统**      | ❌ 不能                   | ✅ 可以                   |
| **链接目录**        | ❌ 不能                   | ✅ 可以                   |
| **原始文件删除后**  | 仍然有效（数据未删除）     | 失效（悬空链接）           |
| **inode**           | 与原始文件相同            | 不同（独立 inode）         |
| **`ls -l` 显示**    | 普通文件（无特殊标记）     | `->` 指向目标（如 `symlink -> file`） |

---

### **应用场景**
- **硬链接**：适合需要多个路径访问同一文件，且不希望因删除某个路径导致文件丢失的情况（如备份、日志轮转）。
- **软连接**：适合需要跨文件系统引用、指向目录或动态路径的情况（如 `/usr/bin/python` 指向具体版本）。