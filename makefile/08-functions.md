# Makefile 函数详解与示例

Makefile 提供了一系列内置函数，可以大大增强 Makefile 的功能和灵活性。下面我将结合具体示例介绍几个常用的 Makefile 函数。

## 1. 字符串替换函数 (String Substitution)

`$(subst from,to,text)` 函数用于在文本中进行字符串替换。

**示例**：
```makefile
text = Hello World
# 将空格替换为逗号
result := $(subst  , ,$(text))

all:
    @echo Original: $(text)
    @echo After substitution: $(result)
```

输出：
```
Original: Hello World
After substitution: Hello,World
```

`patsubst` (pattern substitution) 是 Makefile 中一个非常实用的字符串替换函数，它允许你使用模式匹配来进行更灵活的字符串替换。

```makefile
$(patsubst pattern,replacement,text)
```

- `pattern`：要匹配的模式，可以包含通配符 `%`
- `replacement`：替换模式，其中的 `%` 会被 `pattern` 中 `%` 匹配的内容替换
- `text`：要进行替换操作的文本

1. 类似于 `subst` 函数，但支持通配符 `%` 进行模式匹配
2. 常用于文件名后缀替换（如 `.c` 到 `.o`）
3. 比简单的 `subst` 更灵活，可以只替换特定模式的字符串

### 基本示例

```makefile
# 将所有的.c文件替换为.o文件
SRC = foo.c bar.c baz.c
OBJ = $(patsubst %.c,%.o,$(SRC))

all:
    @echo "Source files: $(SRC)"
    @echo "Object files: $(OBJ)"
```

输出：
```
Source files: foo.c bar.c baz.c
Object files: foo.o bar.o baz.o
```

### 添加路径前缀

```makefile
# 替换后缀并添加新的路径前缀
SRC = src/foo.c src/bar.c lib/baz.c
OBJ = $(patsubst %.c,obj/%.o,$(notdir $(SRC)))

all:
    @echo "Source files: $(SRC)"
    @echo "Object files: $(OBJ)"
```

输出：
```
Source files: src/foo.c src/bar.c lib/baz.c
Object files: obj/foo.o obj/bar.o obj/baz.o
```

### 多模式替换

```makefile
# 同时处理.c和.cpp文件
FILES = main.c util.cpp helper.c logger.cpp
OBJS = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(FILES)))

all:
    @echo "Original: $(FILES)"
    @echo "Objects: $(OBJS)"
```

输出：
```
Original: main.c util.cpp helper.c logger.cpp
Objects: main.o util.o helper.o logger.o
```

## 简写形式

Makefile 提供了一种更简洁的变量替换语法，实际上是 `patsubst` 的简写形式：

```makefile
OBJ = $(SRC:.c=.o)       # 将.c替换为.o
OBJ = $(SRC:%.c=%.o)    # 同上，更明确的写法
```

## 与 foreach 结合使用

```makefile
# 多个目录下的源文件处理
DIRS = src lib test
SRC = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
OBJ = $(patsubst %.c,obj/%.o,$(notdir $(SRC)))

all:
    @echo "Sources: $(SRC)"
    @echo "Objects: $(OBJ)"
```

## 实际应用示例

```makefile
# 更完整的Makefile示例
SRC_DIR = src
OBJ_DIR = obj
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

TARGET = myapp

$(TARGET): $(OBJ)
    gcc $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
    gcc -c $< -o $@

$(OBJ_DIR):
    mkdir -p $@

clean:
    rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
```

在这个示例中，`patsubst` 用于将源文件路径转换为目标文件路径，这是 Makefile 中非常常见的用法。

`patsubst` 是 Makefile 中处理文件名转换的核心函数之一，熟练掌握它可以大大提高 Makefile 编写的效率和灵活性。

## 2. foreach 函数

`$(foreach var,list,text)` 函数用于遍历列表并对每个元素执行操作。

**示例**：
```makefile
files = foo.c bar.c baz.c
# 为每个.c文件生成对应的.o文件目标
objects := $(foreach file,$(files),$(file:.c=.o))

all:
    @echo Source files: $(files)
    @echo Object files: $(objects)
```

输出：
```
Source files: foo.c bar.c baz.c
Object files: foo.o bar.o baz.o
```

## 3. if 函数

`$(if condition,then-part[,else-part])` 函数用于条件判断。

**示例**：
```makefile
DEBUG = 1
CFLAGS = $(if $(DEBUG),-g -O0,-O2)

all:
    @echo Debug mode: $(DEBUG)
    @echo Compiler flags: $(CFLAGS)
```

输出：
```
Debug mode: 1
Compiler flags: -g -O0
```

## 4. call 函数

`$(call variable,param,param,...)` 函数用于调用用户定义的变量（实际上是宏）。

**示例**：
```makefile
# 定义一个反转两个参数的宏
reverse = $2 $1

all:
    @echo $(call reverse,first,second)
```

输出：
```
second first
```

更复杂的例子：
```makefile
# 定义一个创建路径的宏
make-dir = mkdir -p $(1)

all:
    $(call make-dir,./build/obj)
    $(call make-dir,./build/bin)
```

## 5. shell 函数

`$(shell command)` 函数用于执行 shell 命令并获取输出。

**示例**：
```makefile
# 获取当前日期
DATE := $(shell date +%Y-%m-%d)

all:
    @echo Today is $(DATE)
    @echo Files in current directory: $(shell ls)
```

输出：
```
Today is 2023-11-15
Files in current directory: file1 file2 Makefile ...
```

## 6. filter 函数

`$(filter pattern...,text)` 函数用于从文本中筛选出匹配模式的单词。

**示例**：
```makefile
files = foo.c bar.h baz.c qux.h
# 只保留.c文件
c_files := $(filter %.c,$(files))

all:
    @echo All files: $(files)
    @echo C files only: $(c_files)
```

输出：
```
All files: foo.c bar.h baz.c qux.h
C files only: foo.c baz.c
```

## 综合示例

下面是一个结合多个函数的综合示例：

```makefile
# 定义源文件列表
SRCS = src/main.c src/utils.c lib/helper.c

# 使用foreach和subst将.c替换为.o，并添加build前缀
OBJS = $(foreach src,$(SRCS),build/$(subst .c,.o,$(notdir $(src))))

# 使用filter分离C和C++文件
C_SRCS = $(filter %.c,$(SRCS))
CPP_SRCS = $(filter %.cpp,$(SRCS))

# 根据DEBUG变量设置编译选项
DEBUG = 1
CFLAGS = $(if $(DEBUG),-g -O0,-O2)

all: $(OBJS)
    @echo "Building with flags: $(CFLAGS)"
    @echo "Object files: $(OBJS)"
    @echo "C sources: $(C_SRCS)"
    @echo "C++ sources: $(CPP_SRCS)"

build/%.o: src/%.c
    $(call make-dir,$(@D))
    gcc $(CFLAGS) -c $< -o $@

# 定义创建目录的函数
make-dir = mkdir -p $(1)

clean:
    rm -rf build
```