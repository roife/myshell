# 项目结构

```
|- err.h 错误处理宏定义
|- def.h 参数宏定义
|- main.c 主程序
|- ps.c 输出路径和命令提示符
|- parse.c 解析输入
|- cmd.c 运行命令
```

# 使用方式

```shell
$ cd myshell && make
$ ./myshell
```

<div style="page-break-after:always"></div>

# 功能概述

## 路径提示

对当前所在的路径进行提示，用绿色表示路径，用 `$` 分隔命令和路径。

如图所示，还支持路径折叠（即将 `HOME` 目录折叠成 `~`）。

![cwd](/img/cwd.png)

<div style="page-break-after:always"></div>

## 多参数支持

![multi args](/img/multi-args.png)

<div style="page-break-after:always"></div>

## 字符转义

找到所有 `"#include "*.h"`

![escaping characters](/img/escaping-characters.png)

<div style="page-break-after:always"></div>

## 管道支持

统计 c 文件数量：

![count c files](/img/count-c-files.png)

<div style="page-break-after:always"></div>

## 文件重定向

![redirection](/img/redir.png)

<div style="page-break-after:always"></div>

## 错误提示

![](/img/error.png)

<div style="page-break-after:always"></div>

## 内部命令 cd

![cd](/img/cd.png)

<div style="page-break-after:always"></div>

## 内部命令 exit：退出

![exit](/img/exit.png)

<div style="page-break-after:always"></div>

# 系统调用

## ps.c

- `getcwd()`：获取所在路径
- `getenv()`：获取 `$HOME` 环境变量，用于路径折叠

## cmd.c

- `chdir()`：进入路径，用于 `cd` 命令
- `exit()`：终止子进程
- `open()`/`close()`：打开/关闭文件描述符，用于文件重定向
- `dup2()`：复制文件描述符，用于文件重定向
- `execvp()`：调用命令
- `waitpid`：主进程等待子进程结束并获取返回值

<div style="page-break-after:always"></div>

## 流程图

### 解析输入 (parse)

```mermaid
graph TD
STR(str) -->|token| TOKENS(token_list) -->|parse| CMDS(commands)
```

<div style="page-break-after:always"></div>

### 执行单个命令

```mermaid
graph TD
A(开始)

A --> EXIT(exit)
EXIT --> EXIT0[exit 0]

A --> CD(cd)
CD --> CHDIR[chdir 或报错]

A --> CMD(外部命令)
CMD -->|fork| CHILD( 子进程)

CHILD --> CHILDPIPE(管道重定向) -->|dup2| CHILDEXEC
CHILD --> CHILDFILE(文件重定向) -->|open + dup2| CHILDEXEC

CHILDEXEC(执行指令) -->|execvp| CHILDERR(判断错误 + 错误处理) -->|errno| CHILDEXIT(结束子进程并返回状态) --> CMDERR

CMD -->|waitpid| CMDERR(主进程错误处理反馈)
```

<div style="page-break-after:always"></div>

## 组合多个命令（管道）

```mermaid
graph TD
A(开始)

A --> CREATEPIPE(创建管道 pipe)

CREATEPIPE --> P0(pipe)

STDIN(stdin) --> CMD1[执行命令 1]
CMD1 --> P0
P0 --> CMD2[执行命令 2]

CREATEPIPE --> P1(pipe)
CMD2 --> P1
P1 -->|...| PN(pipe)
PN --> CMDN[执行命令 N]
CMDN --> STDOUT(stdout)
```

<div style="page-break-after:always"></div>

# 源代码

项目开源在 [roife/Myshell](http://github.com/roife/myshell)。