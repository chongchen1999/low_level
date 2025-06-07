# Makefile 条件判断与变量检查

下面我将通过示例介绍 Makefile 中的条件判断(if/else)、检查变量是否为空、检查变量是否定义以及 `$(MAKEFLAGS)` 的用法。

## 1. 条件判断 if/else

Makefile 使用 `ifeq`, `ifneq`, `ifdef`, `ifndef` 进行条件判断，基本语法：

```makefile
ifeq (arg1, arg2)
    # 如果 arg1 等于 arg2 则执行这里
else
    # 否则执行这里
endif
```

### 示例

```makefile
# 比较两个值是否相等
COMPILER = gcc

build:
ifeq ($(COMPILER), gcc)
	@echo "Using GCC compiler"
else ifeq ($(COMPILER), clang)
	@echo "Using Clang compiler"
else
	@echo "Using unknown compiler"
endif
```

## 2. 检查变量是否为空

使用 `ifeq` 或 `ifneq` 与空字符串比较：

```makefile
ifeq ($(VAR),)
    # VAR 为空
endif

ifneq ($(VAR),)
    # VAR 不为空
endif
```

### 示例

```makefile
OUTPUT_DIR =

check-empty:
ifeq ($(OUTPUT_DIR),)
	@echo "OUTPUT_DIR is empty"
else
	@echo "OUTPUT_DIR is $(OUTPUT_DIR)"
endif
```

## 3. 检查变量是否定义

使用 `ifdef` 或 `ifndef`：

```makefile
ifdef VAR
    # VAR 已定义
endif

ifndef VAR
    # VAR 未定义
endif
```

### 示例

```makefile
# DEBUG 未定义

check-defined:
ifdef DEBUG
	@echo "Debug mode is on"
else
	@echo "Debug mode is off"
endif
```

## 4. $(MAKEFLAGS)

`$(MAKEFLAGS)` 包含了 make 命令的选项标志，常用于检测是否使用了特定选项。

### 示例

```makefile
check-flags:
# 检查是否包含 -s (silent) 选项
ifneq (,$(findstring s,$(MAKEFLAGS)))
	@echo "Make is running in silent mode"
else
	@echo "Make is running in verbose mode"
endif
```

## 综合示例

```makefile
# 定义变量
COMPILER = gcc
OUTPUT_DIR = bin
# DEBUG 未定义

build: check-flags
ifeq ($(COMPILER), gcc)
	@echo "Building with GCC"
else ifeq ($(COMPILER), clang)
	@echo "Building with Clang"
else
	$(error Unknown compiler: $(COMPILER))
endif

ifdef DEBUG
	@echo "Debug symbols will be included"
endif

ifneq ($(OUTPUT_DIR),)
	@mkdir -p $(OUTPUT_DIR)
	@echo "Output will be placed in $(OUTPUT_DIR)"
else
	@echo "Output will be placed in current directory"
endif

check-flags:
# 检查是否包含 -j (并行构建) 选项
ifneq (,$(findstring j,$(MAKEFLAGS)))
	@echo "Parallel build detected"
else
	@echo "Single-threaded build"
endif
```

使用示例：
```
# 普通构建
make build

# 并行构建
make -j4 build

# 静默模式
make -s build

# 使用不同编译器
make build COMPILER=clang

# 启用调试
make build DEBUG=1
```

