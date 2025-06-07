# Makefile 规则详解与示例

Makefile 中有多种不同类型的规则，每种都有其特定的用途和语法。下面我将结合示例详细介绍这些规则类型。

## 1. 隐式规则 (Implicit Rules)

隐式规则是 make 内置的通用规则，用于基于文件扩展名自动推导如何构建目标。

**示例**:
```makefile
# 不需要显式写出如何从 .c 生成 .o
# make 会自动使用 $(CC) -c 命令

program: main.o utils.o
    $(CC) -o program main.o utils.o

# 等效于显式规则:
# main.o: main.c
#     $(CC) -c main.c -o main.o
# utils.o: utils.c
#     $(CC) -c utils.c -o utils.o
```

常用隐式规则：
- `.c` → `.o`: `$(CC) -c $(CPPFLAGS) $(CFLAGS)`
- `.cpp` → `.o`: `$(CXX) -c $(CPPFLAGS) $(CXXFLAGS)`

## 2. 静态模式规则 (Static Pattern Rules)

静态模式规则允许为多个目标指定一个通用规则，但只适用于特定目标列表。

**语法**:
```makefile
targets...: target-pattern: prereq-patterns...
    recipe
```

**示例**:
```makefile
# 定义要构建的对象文件
OBJECTS = main.o utils.o network.o

# 静态模式规则：所有 OBJECTS 都从对应的 .c 文件构建
$(OBJECTS): %.o: %.c
    $(CC) -c $(CFLAGS) $< -o $@
```

这个规则表示：对于 `$(OBJECTS)` 列表中的每个 `.o` 文件，都从对应的 `.c` 文件构建。

## 3. 静态模式规则与 filter 结合

可以结合 `filter` 函数对目标进行筛选，只对符合条件的文件应用规则。

**示例**:
```makefile
# 定义源文件和对象文件
SRC = main.c utils.c network.cpp logger.cpp
OBJ = $(SRC:.c=.o)  # 这会得到 main.o utils.o network.cpp.o logger.cpp.o

# 只对 .c 文件应用 C 编译规则
$(filter %.o,$(OBJ)): %.o: %.c
    $(CC) -c $(CFLAGS) $< -o $@

# 对 .cpp 文件应用 C++ 编译规则
$(filter %.cpp.o,$(OBJ)): %.cpp.o: %.cpp
    $(CXX) -c $(CXXFLAGS) $< -o $@
```

## 4. 模式规则 (Pattern Rules)

模式规则类似于隐式规则，但由用户定义，使用通配符 `%` 匹配文件名。

**语法**:
```makefile
%.target: %.prereq
    recipe
```

**示例**:
```makefile
# 自定义从 .c 到 .o 的规则
%.o: %.c
    $(CC) -c $(CFLAGS) $< -o $@

# 从 .txt 生成 .html 的规则
%.html: %.txt
    markdown $< > $@
```

与静态模式规则的区别：
- 模式规则适用于任何匹配模式的目标
- 静态模式规则只适用于明确列出的目标

## 5. 双冒号规则 (Double-Colon Rules)

双冒号规则允许一个目标有多个独立的规则，每个规则都会被执行（如果是单冒号规则，则只有最后一个规则有效）。

**语法**:
```makefile
target:: prereq1
    recipe1

target:: prereq2
    recipe2
```

**示例**:
```makefile
# 用于日志文件的规则
logfile::
    echo "Build started at $(shell date)" >> logfile

logfile:: src/main.c
    echo "Building from $<" >> logfile
    $(CC) -c $< -o main.o
    echo "Build completed at $(shell date)" >> logfile
```

双冒号规则常用于：
1. 需要执行多个独立操作的构建步骤
2. 收集构建信息
3. 增量式构建，不同来源可能需要不同处理

## 综合示例

```makefile
# 定义工具和标志
CC = gcc
CXX = g++
CFLAGS = -Wall -O2
CXXFLAGS = -Wall -O2 -std=c++11

# 源文件
C_SRCS = main.c utils.c
CPP_SRCS = network.cpp logger.cpp
OBJS = $(C_SRCS:.c=.o) $(CPP_SRCS:.cpp=.o)

# 最终目标
program: $(OBJS)
    $(CXX) -o $@ $^

# 静态模式规则：C 文件
$(filter %.o,$(OBJS)): %.o: %.c
    $(CC) -c $(CFLAGS) $< -o $@

# 静态模式规则：C++ 文件
$(filter %.o,$(OBJS)): %.o: %.cpp
    $(CXX) -c $(CXXFLAGS) $< -o $@

# 清理规则
clean::
    rm -f *.o
    echo "Object files removed"

clean::
    rm -f program
    echo "Program removed"

# 模式规则：从 .md 生成 .html
%.html: %.md
    pandoc $< -o $@

.PHONY: clean
```