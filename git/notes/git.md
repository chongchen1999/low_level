# Git 指令


## git add
当执行 `git add xxx` 命令时，`.git` 目录会发生以下关键变化：

---

1. **更新索引文件（`.git/index`）**
   • 作用：Git 的索引（Stage Area）是一个二进制文件，记录暂存的文件信息。

   • 变化：

     ◦ 文件的路径、大小、时间戳、SHA-1哈希值会被写入索引。

     ◦ 如果文件已存在索引中，则更新对应条目；否则新增条目。

   • 验证：可通过 `git ls-files --stage` 查看索引内容。


---

2. **创建或更新对象（`.git/objects/`）**
   • Blob 对象：

     ◦ Git 会将文件内容压缩为 Blob 对象，存储在 `.git/objects/xx/xxxx...`（`xx` 是 SHA-1 的前两位）。

     ◦ 可通过 `git cat-file -p <SHA-1>` 查看对象内容。

   • 重复优化：若文件内容未变化，Git 会复用已有的 Blob 对象（节省空间）。


---

3. **其他可能的变化**
   • 钩子触发（如有）：若配置了 `pre-commit` 或 `pre-add` 钩子，可能会执行自定义脚本（但默认无影响）。

   • 工作树缓存：某些操作（如 `git status`）的缓存可能会被更新以加速后续操作。


---

示例流程
假设执行 `git add README.md`：
1. Git 计算 `README.md` 的 SHA-1 哈希（如 `2d832d...`）。
2. 在 `.git/objects/2d/832d...` 创建 Blob 对象存储文件内容。
3. 在 `.git/index` 中记录 `README.md` 的路径和对应的 Blob 哈希。

---

注意事项
• .git 目录是 Git 的核心，手动修改可能导致仓库损坏。

• 未提交的数据：`git add` 仅更新暂存区，提交（`git commit`）后才会生成提交对象（Commit Object）。


如果需要深入检查，可使用以下命令：
```bash
# 查看索引内容
git ls-files --stage

# 查看对象类型和内容
git cat-file -t <SHA-1>  # 查看对象类型
git cat-file -p <SHA-1>  # 查看对象内容
```

## git commit 
当你执行 `git commit` 命令时，`.git` 目录会发生一系列变化，主要包括以下几个关键操作：

---

1. **生成新的 Commit 对象**
• 路径：`.git/objects/`  

  Git 会为本次提交创建一个新的 Commit 对象，内容包含：
  • 当前提交的元数据（作者、提交者、时间戳、提交消息等）。

  • 指向一个 Tree 对象（代表当前工作目录的快照）。

  • 指向父提交的指针（如果是首次提交则没有父提交）。

• 存储方式：  

  对象会通过 SHA-1 哈希命名（如 `abcd123...`），存储在 `.git/objects/ab/cd123...` 文件中（前两位作为目录名）。

---

2. **更新引用（Branch/Tag）**
• 分支引用更新：  

  • 当前分支的引用（如 `refs/heads/main`）会更新为指向新提交的 SHA-1。

  • 文件路径：`.git/refs/heads/<branch-name>` 或 `.git/packed-refs`（如果引用被压缩）。

• HEAD 指针：  

  • 如果是在普通分支上提交，`.git/HEAD` 文件仍然指向当前分支（如 `ref: refs/heads/main`）。

  • 如果是在分离头状态（detached HEAD），`.git/HEAD` 会直接指向新提交的 SHA-1。


---

3. **更新索引（Index）**
• 索引文件：`.git/index`  

  • 提交后，索引（暂存区）的内容会与当前提交的 Tree 对象保持一致。

  • 如果提交成功，索引通常不会变化（除非提交后立即修改了工作区文件并重新暂存）。


---

4. **更新 Reflog**
• 引用日志：`.git/logs/`  

  • 提交会被记录到 `HEAD` 和分支的 reflog 中（如 `.git/logs/refs/heads/main`）。

  • 记录内容包括旧 SHA-1、新 SHA-1、操作类型（`commit`）和操作者信息。


---

5. **可能的附加操作**
• 钩子（Hooks）执行：  

  如果配置了 `post-commit` 钩子（`.git/hooks/post-commit`），Git 会执行该脚本。
• 打包对象（可选）：  

  如果启用了自动打包，Git 可能会将松散对象压缩到 `.git/objects/pack/` 中。

---

示例流程
假设你修改了文件 `file.txt` 并提交：
1. 暂存：`git add file.txt`  
   • 文件的 Blob 对象被创建到 `.git/objects/`。

   • 索引（`.git/index`）更新为指向该 Blob。

2. 提交：`git commit -m "Update file"`  
   • 生成 Commit 对象（包含 Tree 和父提交）。

   • `refs/heads/main` 更新为新 Commit 的 SHA-1。

   • Reflog 和索引同步更新。


---

验证变化的命令
• 查看最新提交：`git cat-file -p HEAD`  

• 检查分支引用：`cat .git/refs/heads/main`  

• 查看 Reflog：`git reflog`  

## Changes

Do some changes on the feat branch.