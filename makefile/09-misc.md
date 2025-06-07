# Makefile 高级特性详解

下面我将结合实例详细介绍 Makefile 中的 `vpath` 指令、多行命令、`.PHONY` 伪目标和 `.DELETE_ON_ERROR` 特殊目标的用法。

## 1. vpath 指令

`vpath` 指令用于指定 Make 在不同目录中搜索文件的规则。

### 基本语法
```
vpath <pattern> <directories>
```

### 实例
```makefile
# 在当前目录和src目录中搜索.c文件
vpath %.c ./src

# 在include和../headers目录中搜索.h文件
vpath %.h ./include ../headers

# 清除所有vpath设置
vpath %

# 编译规则
%.o: %.c
    gcc -c $< -o $@
```

当执行 `make foo.o` 时，Make 会先在当前目录查找 `foo.c`，如果没找到，会去 `src` 目录查找。

## 2. 多行命令

Makefile 中的命令可以通过反斜杠 `\` 实现多行连接。

### 实例
```makefile
complex-task:
    @echo "开始执行复杂任务..." && \
    mkdir -p build/output && \
    cp src/*.txt build/output/ && \
    echo "任务完成"
```

注意事项：
1. 每行结尾要有 `\` 符号（最后一行除外）
2. `\` 后不能有空格或其他字符
3. 整个多行命令被视为一个单独的 shell 会话

## 3. .PHONY 伪目标

`.PHONY` 用于声明那些不是真实文件名的目标，即使存在同名文件也会执行。

### 实例
```makefile
.PHONY: clean install all

all: program

program: main.o utils.o
    gcc main.o utils.o -o program

clean:
    rm -f *.o program

install: program
    cp program /usr/local/bin
```

为什么需要 `.PHONY`：
- 避免与同名文件冲突
- 提高性能（Make 不需要检查文件时间戳）
- 明确表明这是特殊目标

## 4. .DELETE_ON_ERROR

当目标命令执行失败时，删除该目标文件。

### 实例
```makefile
.DELETE_ON_ERROR:

output.txt: input.txt
    process-input < input.txt > output.txt
```

如果 `process-input` 命令失败，`output.txt` 会被自动删除，避免留下不完整的文件。

## 5. Include Makefiles
The include directive tells make to read one or more other makefiles. It's a line in the makefile that looks like this:

```makefile
include filenames...
```

## 综合实例

```makefile
# 特殊目标声明
.PHONY: all clean
.DELETE_ON_ERROR:

# 设置搜索路径
vpath %.c src
vpath %.h include

# 编译器设置
CC = gcc
CFLAGS = -Wall -Iinclude

# 目标文件列表
OBJS = main.o utils.o

# 默认目标
all: myprogram

# 链接目标程序
myprogram: $(OBJS)
    $(CC) $(CFLAGS) -o $@ $^

# 模式规则编译.c文件
%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
    rm -f $(OBJS) myprogram \
        *.bak *~

# 复杂命令示例
backup:
    @echo "开始备份..." && \
    tar -czf backup.tar.gz src/*.c include/*.h && \
    echo "备份完成: backup.tar.gz"
```

这个综合实例展示了：
1. 使用 `vpath` 指定源文件和头文件的搜索路径
2. 使用多行命令实现复杂的备份操作
3. 声明 `all` 和 `clean` 为伪目标
4. 设置 `.DELETE_ON_ERROR` 确保构建失败时删除不完整的目标文件

这些高级特性可以显著提高 Makefile 的灵活性和健壮性，特别是在管理大型项目时非常有用。