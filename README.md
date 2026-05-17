# 迷宫求解程序

这是一个用 C++ 编写的迷宫求解程序。程序读取文本文件中的迷宫数据，使用 8 方向 Freeman 链码记录从入口到出口的路径，并在控制台输出坐标路径和迷宫图形。

## 项目文件

| 文件 | 说明 |
|---|---|
| `maze.cpp` | 主程序源码 |
| `maze.txt`、`maze1.txt` 到 `maze6.txt` | 迷宫样例数据 |
| `CMakeLists.txt` | CMake 构建配置 |
| `.editorconfig` | 编辑器编码和换行配置 |

## 迷宫格式

迷宫文件是纯文本格式，内容由数字组成：

```text
行数 列数
入口行 入口列
出口行 出口列
迷宫矩阵
```

矩阵中：

| 数字 | 含义 |
|---|---|
| `0` | 可通行 |
| `1` | 墙 |

坐标从 `0` 开始计数。迷宫边界通常设置为墙，入口和出口必须是可通行位置。

## 编译运行

使用 Visual Studio 2026 Build Tools：

```powershell
cmd /c ""C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cl /EHsc /utf-8 /nologo maze.cpp /Fe:maze.exe"
```

运行默认样例 `maze5.txt`：

```powershell
.\maze.exe
```

运行指定迷宫文件：

```powershell
.\maze.exe maze1.txt
```

## 使用 CMake

如果本机已安装 CMake，也可以使用：

```powershell
cmake -S . -B build
cmake --build build
```

生成的可执行文件会在 `build` 目录中。
