# Variables More

## 1. Flavors 和 Modification

在 Makefile 中，变量有两种"风味"(flavors)：
- **递归展开变量 (recursively expanded variables)**: 使用 `=` 定义，在引用时展开
- **简单展开变量 (simply expanded variables)**: 使用 `:=` 定义，在定义时展开

**示例**:
```makefile
# 递归展开变量
VAR1 = $(VAR2)
VAR2 = Hello

# 简单展开变量
VAR3 := $(VAR2)
VAR2 = World

all:
    @echo "VAR1: $(VAR1)"  # 输出 World (引用时展开)
    @echo "VAR3: $(VAR3)"  # 输出 Hello (定义时展开)
```

## 2. 命令行参数和覆盖

可以在命令行中覆盖 Makefile 中定义的变量。

**示例**:
```makefile
# Makefile 内容
CC = gcc
CFLAGS = -Wall

all:
    @echo "Using compiler: $(CC)"
    @echo "Flags: $(CFLAGS)"
```

命令行执行:
```bash
make CC=clang CFLAGS="-Wall -O2"
```
这将不会覆盖 Makefile 中的 `CC` 和 `CFLAGS` 定义。

```makefile
# Makefile 内容
override CC = gcc
override CFLAGS = -Wall

all:
    @echo "Using compiler: $(CC)"
    @echo "Flags: $(CFLAGS)"
```
这样写才会覆盖。

## 3. 命令列表和定义

可以在 Makefile 中定义命令序列，有两种主要方式：

**示例 1: 多行命令**
```makefile
build:
    @echo "Building..."
    gcc -c main.c
    gcc -o program main.o
```

**示例 2: 使用 define 定义命令序列**
```makefile
define BUILD_COMMANDS
@echo "Starting build process"
gcc -c $(SRC)
gcc -o $(TARGET) $(OBJ)
endef

all:
    $(BUILD_COMMANDS)
```

## 4. 目标特定变量

可以为特定目标设置变量，这些变量只在该目标及其依赖的规则中有效。

**示例**:
```makefile
CFLAGS = -Wall

debug: CFLAGS += -g
debug: program

program: main.c
    gcc $(CFLAGS) -o program main.c
```

执行 `make debug` 时，`CFLAGS` 会包含 `-g` 选项，而直接执行 `make program` 则不会。

## 5. 模式特定变量

可以为匹配特定模式的目标设置变量。

**示例**:
```makefile
# 为所有 .o 文件设置 CFLAGS
%.o: CFLAGS = -Wall -O2

main.o: main.c
    gcc $(CFLAGS) -c main.c

utils.o: utils.c
    gcc $(CFLAGS) -c utils.c
```

在这个例子中，所有 `.o` 文件的编译都会使用 `-Wall -O2` 选项。

## 综合示例

```makefile
# 变量定义
CC := gcc
SRC_DIR := src
BUILD_DIR := build
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
TARGET := program

# 模式特定变量
$(BUILD_DIR)/%.o: CFLAGS = -Wall -Wextra

# 调试版本特定设置
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# 发布版本特定设置
release: CFLAGS += -O3
release: $(TARGET)

# 主要构建规则
$(TARGET): $(OBJECTS)
    $(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
    $(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
    mkdir -p $(BUILD_DIR)

clean:
    rm -rf $(BUILD_DIR) $(TARGET)

# 命令定义
define PRINT_BUILD_INFO
@echo "Building $(TARGET)"
@echo "Compiler: $(CC)"
@echo "Flags: $(CFLAGS)"
endef

info:
    $(PRINT_BUILD_INFO)

.PHONY: clean debug release info
```

这个综合示例展示了：
1. 变量定义和使用 (`=`, `:=`)
2. 目标特定变量 (`debug`, `release`)
3. 模式特定变量 (`$(BUILD_DIR)/%.o`)
4. 命令定义 (`define PRINT_BUILD_INFO`)
5. 目录创建顺序限定 (`| $(BUILD_DIR)`)

可以通过不同方式调用：
```bash
make debug      # 构建调试版本
make release    # 构建发布版本
make info       # 显示构建信息
make clean      # 清理构建文件
```