# CMake 基础命令与实例讲解

下面我将通过一个简单的C++项目示例，讲解这些CMake命令的用法。

## 项目结构示例

假设我们有以下项目结构：
```
my_project/
├── CMakeLists.txt          # 根CMake文件
├── include/
│   └── utils.h            # 头文件
├── src/
│   ├── utils.cpp          # 源文件
│   └── main.cpp           # 主程序
└── lib/
    └── third_party/       # 第三方库
```

## CMakeLists.txt 详细示例

```cmake
# 设置CMake最低版本要求
cmake_minimum_required(VERSION 3.10)

# 定义项目名称和使用的语言
project(MyProject VERSION 1.0 LANGUAGES CXX)

# 定义一个选项，允许用户在编译时决定是否启用调试信息
option(ENABLE_DEBUG "Enable debug compilation" ON)

# 添加可执行文件
add_executable(my_program 
    src/main.cpp
    src/utils.cpp
)

# 添加库 - 将utils.cpp编译为静态库
add_library(utils STATIC src/utils.cpp)

# 包含头文件目录
target_include_directories(my_program PUBLIC include)
target_include_directories(utils PUBLIC include)

# 链接库
target_link_libraries(my_program PRIVATE utils)

# 添加子目录 - 假设lib/third_party下有另一个CMake项目
add_subdirectory(lib/third_party)

# 链接第三方库 (假设第三方库名为third_party_lib)
target_link_libraries(my_program PRIVATE third_party_lib)

# 根据选项设置编译定义
if(ENABLE_DEBUG)
    target_compile_definitions(my_program PRIVATE DEBUG_MODE=1)
    message(STATUS "Debug mode enabled")
else()
    target_compile_definitions(my_program PRIVATE DEBUG_MODE=0)
endif()

# 使用PROJECT_SOURCE_DIR引用项目根目录
message(STATUS "Project source directory: ${PROJECT_SOURCE_DIR}")

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

## 命令详解

1. **`add_library()`**
   - 用于创建库文件
   - 示例：`add_library(utils STATIC src/utils.cpp)`
   - 创建一个名为`utils`的静态库，从`src/utils.cpp`编译

2. **`add_subdirectory()`**
   - 添加子目录，该目录应包含另一个CMakeLists.txt
   - 示例：`add_subdirectory(lib/third_party)`
   - 会执行`lib/third_party/CMakeLists.txt`

3. **`target_include_directories()`**
   - 为特定目标添加包含目录
   - 示例：`target_include_directories(my_program PUBLIC include)`
   - 为`my_program`添加`include`目录作为头文件搜索路径

4. **`target_link_libraries()`**
   - 指定目标需要链接的库
   - 示例：`target_link_libraries(my_program PRIVATE utils)`
   - 将`utils`库链接到`my_program`

5. **`PROJECT_SOURCE_DIR`**
   - CMake变量，表示项目根目录的完整路径
   - 示例：`${PROJECT_SOURCE_DIR}/include`

6. **`if()`**
   - 条件判断语句
   - 示例：检查选项是否启用

7. **`option()`**
   - 定义一个用户可以设置的选项
   - 示例：`option(ENABLE_DEBUG "Enable debug compilation" ON)`
   - 创建一个默认为ON的选项

8. **`target_compile_definitions()`**
   - 为目标添加编译定义
   - 示例：`target_compile_definitions(my_program PRIVATE DEBUG_MODE=1)`
   - 相当于在代码中添加`#define DEBUG_MODE 1`

## 构建流程

1. 创建build目录并进入：
   ```bash
   mkdir build && cd build
   ```

2. 运行CMake生成构建系统：
   ```bash
   cmake ..
   ```

3. 构建项目：
   ```bash
   cmake --build .
   ```

4. 运行程序：
   ```bash
   ./my_program
   ```

## 高级用法示例

如果你想禁用调试模式，可以在生成构建系统时指定：
```bash
cmake -DENABLE_DEBUG=OFF ..
```

这个示例展示了如何用CMake管理一个包含多个源文件、自定义库和第三方依赖的C++项目。