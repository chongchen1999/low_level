# Makefile 自动变量和通配符详解

## 通配符 (Wildcards)

通配符用于匹配文件名模式，常用的有：

- `*`：匹配任意数量的字符
- `?`：匹配单个字符
- `[...]`：匹配括号内的任意一个字符
- `%`：匹配模式 (Matching Mode) 以及 替换模式 (Replacing Mode)

### 示例 1：基本通配符使用

```makefile
# 匹配当前目录下所有.c文件
SOURCES := $(wildcard *.c)

# 将.c文件替换为.o文件
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))

all: $(OBJECTS)

%.o: %.c
	gcc -c $< -o $@
```

在这个例子中：
1. `$(wildcard *.c)` 获取所有.c文件
2. `$(patsubst %.c,%.o,...)` 将.c替换为.o
3. `%.o: %.c` 是一个模式规则，表示如何从.c文件生成.o文件

# 关于 Makefile 中 `%` 符号的详细解释

在 Makefile 中，`%` 符号是一个非常重要的通配符，它有两种主要使用模式：匹配模式和替换模式。

## 匹配模式 (Matching Mode)

在匹配模式下，`%` 可以匹配任意长度的非空字符串（称为"stem"，即"词干"）。

### 特点：
1. 可以匹配一个或多个字符
2. 匹配的部分会被记住，可以在后续操作中引用
3. 常用于模式规则(pattern rules)中

### 示例：
```makefile
%.o: %.c
    gcc -c $< -o $@
```
这里 `%.c` 匹配任何以 `.c` 结尾的文件名，`%.o` 匹配对应的以 `.o` 结尾的文件名。

## 替换模式 (Replacing Mode)

在替换模式下，`%` 代表之前匹配到的词干(stem)，用于生成新的字符串。

### 特点：
1. 使用之前匹配到的词干部分
2. 常用于生成目标文件或依赖文件列表
3. 常与自动变量如 `$@`, `$<` 等一起使用

### 示例：
```makefile
SOURCES = foo.c bar.c baz.c
OBJECTS = $(SOURCES:.c=.o)
```
这里 `$(SOURCES:.c=.o)` 将把所有 `.c` 文件替换为 `.o` 文件，`%` 在这里代表文件名的主体部分。

## 常见使用场景

1. **模式规则**：
   ```makefile
   %.o: %.c
       $(CC) -c $(CFLAGS) $< -o $@
   ```

2. **字符串替换函数**：
   ```makefile
   $(patsubst %.c,%.o,$(wildcard *.c))
   ```

3. **静态模式规则**：
   ```makefile
   objects = foo.o bar.o
   $(objects): %.o: %.c
       $(CC) -c $(CFLAGS) $< -o $@
   ```

## 注意事项

1. `%` 在匹配时是非贪婪的 - 它会匹配尽可能短的字符串
2. 当有多个 `%` 时，它们必须匹配相同的字符串
3. `%` 不能匹配空字符串
4. 在非模式规则中，`%` 没有特殊含义，只是一个普通字符

## 自动变量 (Automatic Variables)

自动变量在规则执行时由make自动设置，常用的有：

- `$@`：规则的目标文件名
- `$<`：第一个依赖文件名
- `$^`：所有依赖文件列表
- `$?`：比目标新的依赖文件列表
- `$*`：匹配模式规则中的%部分

### 示例 2：自动变量使用

```makefile
# 编译多个源文件
CC = gcc
CFLAGS = -Wall

program: main.o utils.o
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c utils.h
	$(CC) $(CFLAGS) -c $< -o $@

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f program *.o
```

在这个例子中：
- `$@` 在 `program` 规则中是 `program`，在 `main.o` 规则中是 `main.o`
- `$<` 在 `main.o` 规则中是 `main.c`
- `$^` 在 `program` 规则中是 `main.o utils.o`

### 示例 3：更复杂的自动变量使用

```makefile
# 处理多个目录中的源文件
SRC_DIR = src
OBJ_DIR = obj
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

program: $(OBJECTS)
	gcc $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	gcc -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) program
```

这个例子展示了：
1. 从src目录获取源文件
2. 在obj目录生成目标文件
3. `| $(OBJ_DIR)` 表示顺序依赖（obj目录必须存在）
4. 使用自动变量 `$@`, `$<`, `$^`

## 综合示例

```makefile
# 更完整的Makefile示例
CC = gcc
CFLAGS = -Wall -Iinclude
LDFLAGS = -Llib -lmylib

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.d,$(SOURCES))

TARGET = myapp

.PHONY: all clean

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
```

这个示例展示了：
1. 自动依赖生成 (`-MMD` 标志)
2. 多目录支持
3. 更完整的构建系统结构
4. 使用 `.PHONY` 声明伪目标

