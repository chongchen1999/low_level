# Commands and Execution

## 1. Command Echoing/Silencing (命令回显/静默)

Makefile 默认会回显执行的命令，可以通过 `@` 前缀来静默命令。

```makefile
# 示例
all:
    @echo "这条命令不会显示"
    echo "这条命令会显示"
```

执行结果：
```
这条命令会显示
这条命令不会显示
```

## 2. Command Execution (命令执行)

每条命令都在独立的 shell 中执行，可以使用 `;` 或 `\` 连接多行命令。

```makefile
# 示例
test:
    cd /tmp; \
    pwd  # 这两条命令在同一个shell中执行
    pwd  # 这条命令在新的shell中执行
```

## 3. Default Shell (默认 shell)

Makefile 默认使用 `/bin/sh`，可以通过 `SHELL` 变量修改：

```makefile
SHELL = /bin/bash
```

## 4. Double dollar sign (双美元符号)

在 Makefile 中，`$` 有特殊含义，要使用真正的 `$` 需要写成 `$$`。

```makefile
# 示例
print-var:
    @var="Hello"; \
    echo "Make variable: $(VAR)"  # Make变量
    echo "Shell variable: $$var"  # Shell变量
```

## 5. Error handling with -k, -i, and - (错误处理)

- `-k` (--keep-going): 出错后继续执行
- `-i` (--ignore-errors): 忽略所有错误
- `-` (前缀): 忽略单条命令的错误

```makefile
# 示例
test-errors:
    -rm non-existent-file  # 忽略这条命令的错误
    @echo "这条命令会执行"
    
    rm another-non-existent-file  # 这条命令会报错
    @echo "这条命令不会执行"
```

使用 `make -k` 可以继续执行后续目标。

## 6. Interrupting or killing make (中断或终止 make)

- Ctrl+C 发送 SIGINT 中断 make
- `make --jobserver-auth=...` 可以控制并行任务

```makefile
# 示例 (使用并行任务)
.PHONY: all
all: target1 target2 target3

target1 target2 target3:
    @echo "开始 $@"; sleep 2; echo "完成 $@"
```

使用 `make -j3` 并行执行 3 个目标。

## 7. Recursive use of make (递归使用 make)

可以在 Makefile 中调用其他 Makefile：

```makefile
# 示例
subsystem:
    cd subdir && $(MAKE)
    
clean:
    cd subdir && $(MAKE) clean
```

## 8. Export, environments, and recursive make (导出、环境和递归 make)

- `export` 将变量传递给子 make
- `unexport` 取消导出

```makefile
# 示例
export PATH := /usr/local/bin:$(PATH)
CFLAGS = -g -O2

all:
    @echo "PATH = $(PATH)"
    @echo "CFLAGS = $(CFLAGS)"  # 不会被导出
    $(MAKE) -C subdir
```

## 9. Arguments to make (make 的参数)

常见参数：
- `-f` 指定 Makefile 文件
- `-C` 改变目录后执行
- `-n` 只打印不执行
- `-B` 强制重新构建

```makefile
# 示例
debug:
    @echo "构建调试版本"
    $(MAKE) -f debug.mk

release:
    @echo "构建发布版本"
    $(MAKE) -f release.mk
```

## 综合示例

```makefile
# 综合示例 Makefile
SHELL = /bin/bash
export BUILD_DIR = build
CFLAGS = -Wall -Wextra

.PHONY: all clean test

all: program

program: main.o utils.o
    @echo "正在链接..."
    $(CC) $(CFLAGS) $^ -o $@

%.o: %.c
    -@mkdir -p $(BUILD_DIR) 2>/dev/null
    $(CC) $(CFLAGS) -c $< -o $(BUILD_DIR)/$@
    @mv $(BUILD_DIR)/$@ .

clean:
    -rm -rf *.o program $(BUILD_DIR)

test: program
    @echo "运行测试..."
    @./program --test

# 递归调用子目录 Makefile
subsystem:
    $(MAKE) -C subdir

# 使用特殊变量
print:
    @echo "MAKEFLAGS = $(MAKEFLAGS)"
    @echo "MAKELEVEL = $(MAKELEVEL)"
    @echo "CURDIR = $(CURDIR)"
```

这个综合示例展示了：
1. 设置默认 shell
2. 导出变量
3. 静默命令 (`@`)
4. 忽略错误 (`-`)
5. 模式规则 (`%.o: %.c`)
6. 特殊变量 (`$@`, `$^`, `$<`)
7. 递归 make
8. 伪目标 (`.PHONY`)
9. 变量使用 (`CFLAGS`)
10. 目录创建和错误处理
