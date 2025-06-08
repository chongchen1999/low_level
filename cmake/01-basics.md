# CMake 核心概念与示例讲解

下面我将通过一个完整的示例项目来介绍这些 CMake 核心概念。

## 示例项目结构

假设我们有一个简单的 C++ 项目，结构如下：

```
MyProject/
├── CMakeLists.txt
├── include/
│   └── mylib.h
├── src/
│   └── mylib.cpp
└── config.h.in
```

## CMakeLists.txt 示例

```cmake
# 1. cmake_minimum_required() - 指定CMake的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 2. project() - 定义项目名称和相关信息
project(MyProject
    VERSION 1.0
    DESCRIPTION "A simple example project"
    LANGUAGES CXX)

# 3. 设置C++标准
set(CMAKE_CXX_STANDARD 17)          # 设置C++标准为C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 要求必须支持指定的C++标准

# 4. 配置版本信息文件
# 使用<PROJECT-NAME>_VERSION_MAJOR和<PROJECT-NAME>_VERSION_MINOR
configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

# 5. 添加可执行文件
add_executable(MyApp 
    src/mylib.cpp
    src/main.cpp
)

# 6. 设置包含目录
target_include_directories(MyApp
    PRIVATE 
        "${PROJECT_BINARY_DIR}"  # 用于生成的config.h
        "${PROJECT_SOURCE_DIR}/include"
)
```

## 概念详细解释

### 1. `cmake_minimum_required(VERSION min_version)`
- **作用**：指定构建该项目所需的CMake最低版本
- **示例**：`cmake_minimum_required(VERSION 3.10)`
- **说明**：如果系统CMake版本低于指定版本，会报错

### 2. `project()`
- **作用**：定义项目名称和相关属性
- **示例**：
  ```cmake
  project(MyProject
      VERSION 1.0
      DESCRIPTION "A simple example"
      LANGUAGES CXX)
  ```
- **相关变量**：
  - `PROJECT_NAME`：项目名称(这里是"MyProject")
  - `MyProject_VERSION`：完整版本号(1.0)
  - `MyProject_VERSION_MAJOR`：主版本号(1)
  - `MyProject_VERSION_MINOR`：次版本号(0)

### 3. C++标准设置
- `CMAKE_CXX_STANDARD`：设置C++标准版本
  - 示例：`set(CMAKE_CXX_STANDARD 17)` # C++17
- `CMAKE_CXX_STANDARD_REQUIRED`：是否强制要求该标准
  - `ON`：必须支持，否则报错
  - `OFF`：尝试使用，不支持则降级

### 4. `configure_file()`
- **作用**：将输入文件复制到输出文件，并替换其中的变量
- **示例**：
  ```cmake
  configure_file(
      "${PROJECT_SOURCE_DIR}/config.h.in"
      "${PROJECT_BINARY_DIR}/config.h"
  )
  ```
- **config.h.in 示例内容**：
  ```cpp
  #define MYPROJECT_VERSION_MAJOR @MyProject_VERSION_MAJOR@
  #define MYPROJECT_VERSION_MINOR @MyProject_VERSION_MINOR@
  ```

  在 CMake 的 `configure_file` 指令中，`@` 符号用于包裹 **CMake 变量**，这些变量会在文件生成时被替换为实际的值。这是一种模板替换机制。

### 具体用法解析

对于你提到的例子：
```cpp
#define MYPROJECT_VERSION_MAJOR @MyProject_VERSION_MAJOR@
#define MYPROJECT_VERSION_MINOR @MyProject_VERSION_MINOR@
```

1. **`@VARIABLE_NAME@` 语法**：
   - 这是 CMake 的模板替换标记
   - 当执行 `configure_file()` 时，CMake 会查找输入文件中所有的 `@VAR@` 模式
   - 将每个 `@VAR@` 替换成该变量的当前值

2. **变量来源**：
   - `MyProject_VERSION_MAJOR` 和 `MyProject_VERSION_MINOR` 是自动生成的变量
   - 它们来自 `project(MyProject VERSION 1.0)` 的定义
     - `MyProject_VERSION_MAJOR` = 1
     - `MyProject_VERSION_MINOR` = 0

3. **实际生成结果**：
   处理后的 `config.h` 文件将会是：
   ```cpp
   #define MYPROJECT_VERSION_MAJOR 1
   #define MYPROJECT_VERSION_MINOR 0
   ```

### 为什么用 @ 符号？

1. **明确区分**：`@` 使得模板变量与普通代码明显区分
2. **避免冲突**：不会与 C/C++ 代码中的其他符号冲突
3. **CMake 约定**：这是 CMake 的标准语法约定

### 其他类似语法

CMake 还支持另一种替换语法 `${VAR}`，但注意：
- `@VAR@` 用于 `configure_file()` 的模板替换
- `${VAR}` 用于 CMake 脚本内部的变量引用

### 实际应用示例

假设你有以下 `config.h.in`：
```cpp
// 自动生成的配置文件
#define APP_NAME "@PROJECT_NAME@"
#define VERSION "@MyProject_VERSION_MAJOR@.@MyProject_VERSION_MINOR@"
#define BUILD_TIMESTAMP "@TIMESTAMP@"
```

然后在 CMakeLists.txt 中：
```cmake
# 获取当前时间戳
string(TIMESTAMP TIMESTAMP)

configure_file(
  config.h.in
  ${CMAKE_BINARY_DIR}/config.h
)
```

最终生成的 `config.h` 可能类似：
```cpp
// 自动生成的配置文件
#define APP_NAME "MyProject"
#define VERSION "1.0"
#define BUILD_TIMESTAMP "2023-08-20T14:25:03Z"
```

这种机制非常有用，可以：
- 将构建信息注入到代码中
- 生成平台特定的配置
- 创建版本化的头文件
- 实现一次编写，多处使用的配置模板

### 5. `add_executable()`
- **作用**：定义要构建的可执行文件及其源文件
- **示例**：
  ```cmake
  add_executable(MyApp 
      src/mylib.cpp
      src/main.cpp
  )
  ```

### 6. `target_include_directories()`
- **作用**：为特定目标指定头文件搜索路径
- **示例**：
  ```cmake
  target_include_directories(MyApp
      PRIVATE 
          "${PROJECT_BINARY_DIR}"  # 生成的config.h位置
          "${PROJECT_SOURCE_DIR}/include"
  )
  ```
- **关键字**：
  - `PRIVATE`：仅该目标使用
  - `INTERFACE`：仅依赖该目标的目标使用
  - `PUBLIC`：该目标及其依赖目标都使用

### 7. `set()`
- **作用**：设置普通变量、缓存变量或环境变量
- **示例**：
  ```cmake
  set(MY_VARIABLE "value")  # 设置普通变量
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") # 修改编译标志
  ```

## 完整工作流程

1. CMake读取`CMakeLists.txt`
2. 检查版本要求(`cmake_minimum_required`)
3. 定义项目信息(`project`)
4. 设置C++标准
5. 处理`config.h.in`生成`config.h`
6. 定义可执行文件及其源文件
7. 指定头文件搜索路径
8. 生成构建系统(Makefile等)