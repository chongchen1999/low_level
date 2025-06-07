# Unix Filter Tools 介绍与综合示例

Unix 系统中的 filter tools 是一组强大的文本处理工具，它们可以接受输入、对数据进行转换或过滤，然后产生输出。下面我将介绍这些工具，并提供一个综合示例展示它们的协作使用。

## 常用 Filter Tools 简介

1. **cat** - 连接文件并打印到标准输出
   - 示例：`cat file.txt` 显示文件内容

2. **head** - 输出文件的开头部分
   - 示例：`head -n 5 file.txt` 显示文件前5行

3. **tail** - 输出文件的末尾部分
   - 示例：`tail -n 3 file.txt` 显示文件最后3行

4. **tee** - 从标准输入读取并写入标准输出和文件
   - 示例：`ls | tee listing.txt` 将ls结果显示并保存到文件

5. **cut** - 从文件的每一行中删除部分内容
   - 示例：`cut -d':' -f1 /etc/passwd` 提取用户名

6. **paste** - 合并文件的行
   - 示例：`paste file1.txt file2.txt` 并排合并两个文件

7. **sort** - 对文本行进行排序
   - 示例：`sort names.txt` 按字母顺序排序

8. **uniq** - 报告或忽略重复的行
   - 示例：`sort file.txt | uniq` 排序并去除重复行

9. **wc** - 统计文件的行数、字数和字节数
   - 示例：`wc -l file.txt` 统计行数

10. **tr** - 转换或删除字符
    - 示例：`tr 'a-z' 'A-Z' < file.txt` 转换为大写

11. **xargs** - 从标准输入构建和执行命令行
    - 示例：`find . -name "*.txt" | xargs rm` 删除所有txt文件

12. **find** - 在目录层次结构中搜索文件
    - 示例：`find /home -name "*.jpg"` 查找所有jpg文件

## 综合示例

假设我们有一个网站访问日志文件 `access.log`，内容如下：
```
192.168.1.1 - - [10/Oct/2023:13:55:36] "GET /index.html HTTP/1.1" 200 2326
192.168.1.2 - - [10/Oct/2023:13:55:38] "GET /about.html HTTP/1.1" 200 4321
192.168.1.1 - - [10/Oct/2023:13:55:40] "GET /contact.html HTTP/1.1" 404 1234
192.168.1.3 - - [10/Oct/2023:13:55:42] "GET /index.html HTTP/1.1" 200 2326
192.168.1.2 - - [10/Oct/2023:13:55:45] "GET /products.html HTTP/1.1" 200 5678
192.168.1.1 - - [10/Oct/2023:13:55:50] "GET /index.html HTTP/1.1" 200 2326
```

### 任务1：找出访问量最高的IP地址

```bash
cut -d' ' -f1 access.log | sort | uniq -c | sort -nr | head -n 3
```

解释：
1. `cut -d' ' -f1` - 提取IP地址（第一个字段）
2. `sort` - 排序IP地址
3. `uniq -c` - 统计每个IP出现的次数
4. `sort -nr` - 按计数降序排序
5. `head -n 3` - 显示前3个结果

输出可能类似：
```
   3 192.168.1.1
   2 192.168.1.2
   1 192.168.1.3
```

### 任务2：找出最常访问的页面

```bash
cut -d' ' -f7 access.log | sort | uniq -c | sort -nr | head -n 3
```

### 任务3：统计404错误的请求

```bash
grep " 404 " access.log | tee 404_errors.log | wc -l
```

### 任务4：创建所有访问页面的字母顺序列表（大写）

```bash
cut -d' ' -f7 access.log | sort | uniq | tr 'a-z' 'A-Z' | tee pages.txt
```

### 任务5：批量处理日志文件

假设我们有多个日志文件，想统计每个文件中不同IP的数量：

```bash
find /var/log/ -name "access*.log" -print0 | xargs -0 -I {} sh -c 'echo "{}:"; cut -d" " -f1 {} | sort | uniq | wc -l'
```

这个命令：
1. 使用`find`查找所有匹配的日志文件
2. 使用`xargs`对每个文件执行命令
3. 对每个文件，提取IP，排序去重后统计数量

## 总结

Unix filter tools 的强大之处在于它们可以像管道一样连接起来，每个工具专注于一个简单的任务，组合起来却能完成复杂的处理。掌握这些工具可以大大提高在Unix/Linux环境下处理文本数据的效率。