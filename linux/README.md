# Linux  Guide

## Linux文件结构

Linux 使用树状的文件系统结构，所有内容都从根目录 `/` 开始。理解各个目录的功能有助于系统管理和问题排查。

### 根目录 `/`

根目录是整个文件系统的起点，其下的每个子目录都有特定用途：

| 目录 | 全称/含义 | 功能描述 | 示例内容 |
|------|-----------|----------|-----------|
| `/bin` | Binary | **基本用户命令的二进制可执行文件目录**，系统启动后即可使用。 | `ls`, `cp`, `mv`, `cat`, `echo`, `grep` |
| `/sbin` | Superuser Binary | **系统管理员命令**，如系统设置和启动命令，普通用户通常无权运行。 | `reboot`, `ifconfig`, `fsck`, `iptables` |
| `/etc` | etcetera（杂项） | **配置文件目录**，所有系统范围的设置基本都在这里。 | `/etc/passwd`, `/etc/hosts`, `/etc/ssh/sshd_config` |
| `/home` | Home directory | **普通用户的主目录集合**，每个用户对应一个子目录。 | `/home/alice`, `/home/bob`, 各自有 `.bashrc`, `Downloads/` 等 |
| `/root` | Root user home | **超级用户 root 的主目录**，与普通用户的 home 分离以增强安全性。 | `/root/.bashrc`, `/root/scripts/` |
| `/usr` | Unix System Resources | **用户应用程序和库的主目录**，是系统中最大的目录之一。 | `/usr/bin/firefox`, `/usr/lib/`, `/usr/share/fonts` |
| `/lib` | Library | **系统基础库目录**，为 `/bin` 和 `/sbin` 中的程序提供依赖。 | `libc.so.6`, `ld-linux.so.2` 等动态链接库 |
| `/var` | Variable | **经常变化的文件目录**，如日志、缓存、邮件、数据库等。 | `/var/log/syslog`, `/var/spool/mail/`, `/var/cache/apt/` |
| `/tmp` | Temporary | **临时文件目录**，用户和程序存储临时数据，系统重启后通常清空。 | `/tmp/tmpfile123.txt`, `/tmp/.X11-unix/` |
| `/dev` | Device | **设备文件目录**，将硬件表示为文件，便于统一操作。 | `/dev/sda`, `/dev/tty`, `/dev/null`, `/dev/usb/` |
| `/proc` | Process | **虚拟文件系统**，提供当前系统和进程的内核视图。 | `/proc/cpuinfo`, `/proc/meminfo`, `/proc/1/`（进程1） |
| `/sys` | System | **用于内核与硬件交互的虚拟文件系统**，比 `/proc` 更现代化。 | `/sys/class/net/`, `/sys/block/sda/` |
| `/boot` | Boot | **启动相关文件**，包括内核、启动加载器配置文件等。 | `/boot/vmlinuz`, `/boot/grub/grub.cfg`, `/boot/initrd.img` |
| `/media` | Media | **自动挂载的外部设备目录**，如 USB、光盘，桌面系统使用较多。 | `/media/usb`, `/media/cdrom` |
| `/mnt` | Mount | **临时挂载点**，系统管理员可临时挂载额外文件系统。 | `mount /dev/sdb1 /mnt/usb` 挂载一个 U 盘到此目录 |
| `/opt` | Optional | **可选的第三方软件目录**，通常用于手动安装的软件包。 | `/opt/google/chrome`, `/opt/zoom/` |
| `/srv` | Service | **服务提供的数据存储目录**，用于 FTP、HTTP、Web 等。 | `/srv/www`, `/srv/ftp/` |

## 远程登陆

## Linux指令

### 文件目录指令

| 指令 | 功能说明 | 常用示例 |
|------|----------|-----------|
| `ls` | 列出目录内容 | `ls -l`（详细信息），`ls -a`（包括隐藏文件） |
| `cd` | 切换当前目录 | `cd /home/user`，`cd ..`（返回上一级） |
| `pwd` | 显示当前所在目录 | `pwd` |
| `mkdir` | 创建新目录 | `mkdir new_folder` |
| `rmdir` | 删除空目录 | `rmdir old_folder` |
| `rm -r` | 删除目录及其内容 | `rm -r folder_name`（慎用） |
| `cp` | 复制文件或目录 | `cp file1.txt file2.txt`，`cp -r dir1/ dir2/` |
| `mv` | 移动或重命名文件或目录 | `mv old.txt new.txt`，`mv file.txt /path/to/dir/` |
| `find` | 查找文件或目录 | `find . -name "*.txt"` |
| `tree` | 以树状结构显示目录内容（需安装） | `tree /home/user` |

#### 补充说明

- `ls -lh`：以人类可读的方式显示文件大小（如 KB、MB）
- `mkdir -p`：递归创建多级目录，例如 `mkdir -p a/b/c`
- `rm -rf`：强制删除目录及其中内容（危险操作，请谨慎使用）
- `cd ~`：快速返回用户主目录
- `cp -i` 和 `mv -i`：在覆盖文件前进行提示

> 💡 使用 `man 命令名`（如 `man ls`）可以查看命令的详细使用手册。

### 时间日期指令
