# c++ 德州扑克/短牌 solver

[![release](https://img.shields.io/github/v/release/bupticybee/TexasSolver?style=flat-square)](https://github.com/bupticybee/TexasSolver/releases)
[![license](https://img.shields.io/github/license/bupticybee/TexasSolver?style=flat-square)](https://github.com/bupticybee/TexasSolver/blob/master/LICENSE)
[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/bupticybee/TexasSolver/blob/master/TexasSolverTechDemo.ipynb)

README [English](README.md) | [中文](README.zh-CN.md)

## 项目介绍

一个开源的，极其高效的德州扑克和短牌solver, 看看这个 [介绍视频](https://www.bilibili.com/video/BV1QQ4y1h7pM) 了解更多。

Features:
- 对于1~2bet+allin的游戏树求解速度超过piosolver 
- 支持 Mac, Linux and Windows平台
- 支持德州扑克和短牌
- 支持跨语言调用，支持console调用
- 结果和piosolver对齐
- 支持将最优策略保存为json文件
- 是 [TexasHoldemSolverJava](https://github.com/bupticybee/TexasHoldemSolverJava) 的c++版本,速度是前者的5倍以上，而内存使用仅有前者的1/3不到

你可以通过 [google colab demo](https://colab.research.google.com/github/bupticybee/TexasSolver/blob/master/TexasSolverTechDemo.ipynb) 体验这个solver。

## 和piosolver的速度对比

两者均在一个spr=10的flop局面上进行计算，结果对齐。

|                   | 输入配置                                            | 运行日志                                                       | 线程数 | 内存 | 结束精度 | 运行时间 |
| ----------------- | ------------------------------------------------------- | ------------------------------------------------------------------ | ------ | ------ | -------- | -------- |
| piosolver 1.0     | [config_piosolver](benchmark/benchmark_piosolver.txt)   | [log_piosolver](benchmark/benchmark_outputs/piosolver_log.txt)     | 6      | 492Mb  | 0.29%    | 242s     |
| TexasSolver 0.1.0 (Our solver) | [config_texassolver](benchmark/benchmark_texassolver.txt) | [log_texassolver](benchmark/benchmark_outputs/texassolver_log.txt) | 6      | 1600Mb | 0.275%   | 175s     |

对齐结果的图片见 [result_compair](benchmark/benchmark_outputs/result_compair.png). 如你所见，两者的结果非常接近。


## 安装

下载 [release包](https://github.com/bupticybee/TexasSolver/releases), 解压缩，你会看到类似如下的目录结构：

```text
--- Solver
 |- resources
 |- console_solver.exe
```

如果你下载的是MacOs版本，那么, ```console_solver.exe``` 会被一个 ```console_solver``` 的可执行文件替代.

把下载好的solver放在任意目录，安装就已经结束了，就是这么简单！

如果你使用Linux系统或者release包在你的计算机上无法正常运行，你可能需要 [重新编译](#编译源码)

## 使用

### 命令行solver

你可以很轻松的在命令行 (Windows的cmd, Linux的bash,  mac的terminal) 中使用solver.

比如, 如果你在使用windows, 进入windows cmd 命令行 (按下 windows，输入cmd, 然后回车), 进入solver路径，然后使用下面这个简单的命令:

```text
console_solver.exe -i resources/text/commandline_sample_input.txt 
```

MacOs 和 Linux 用户则使用如下命令:

```text
./console_solver -i resources/text/commandline_sample_input.txt 
```

![solving](imgs/solving.gif)

如果一切顺利，你会看到类似下面的log：

```text
EXEC FROM FILE
[##################################################] 100%
<<<START SOLVING>>>
Using 8 threads
Iter: 0
player 0 exploitability 43.637302
player 1 exploitability 27.916748
Total exploitability 71.55405 precent
-------------------
Iter: 11
player 0 exploitability 15.374577
player 1 exploitability 2.400461
Total exploitability 17.775038 precent
time used: 6.871
-------------------
....
```

当求解完成时， 一个 ```output_result.json``` 文件会被生成，这个文件包含了solver产生的最优策略

上面提到的输入文件 ```resources/text/commandline_sample_input.txt``` 包含了一系列建树和求解的指令，一个典型的输入文件如下：

```text
set_pot 50
set_effective_stack 200
set_board Qs,Jh,2h
set_range_ip AA,KK,QQ,JJ,TT,99:0.75,88:0.75,77:0.5,66:0.25,55:0.25,AK,AQs,AQo:0.75,AJs,AJo:0.5,ATs:0.75,A6s:0.25,A5s:0.75,A4s:0.75,A3s:0.5,A2s:0.5,KQs,KQo:0.5,KJs,KTs:0.75,K5s:0.25,K4s:0.25,QJs:0.75,QTs:0.75,Q9s:0.5,JTs:0.75,J9s:0.75,J8s:0.75,T9s:0.75,T8s:0.75,T7s:0.75,98s:0.75,97s:0.75,96s:0.5,87s:0.75,86s:0.5,85s:0.5,76s:0.75,75s:0.5,65s:0.75,64s:0.5,54s:0.75,53s:0.5,43s:0.5
set_range_oop QQ:0.5,JJ:0.75,TT,99,88,77,66,55,44,33,22,AKo:0.25,AQs,AQo:0.75,AJs,AJo:0.75,ATs,ATo:0.75,A9s,A8s,A7s,A6s,A5s,A4s,A3s,A2s,KQ,KJ,KTs,KTo:0.5,K9s,K8s,K7s,K6s,K5s,K4s:0.5,K3s:0.5,K2s:0.5,QJ,QTs,Q9s,Q8s,Q7s,JTs,JTo:0.5,J9s,J8s,T9s,T8s,T7s,98s,97s,96s,87s,86s,76s,75s,65s,64s,54s,53s,43s
set_bet_sizes oop,flop,bet,50
set_bet_sizes oop,flop,raise,60
set_bet_sizes oop,flop,allin
set_bet_sizes ip,flop,bet,50
set_bet_sizes ip,flop,raise,60
set_bet_sizes ip,flop,allin
set_bet_sizes oop,turn,bet,50
set_bet_sizes oop,turn,raise,60
set_bet_sizes oop,turn,allin
set_bet_sizes ip,turn,bet,50
set_bet_sizes ip,turn,raise,60
set_bet_sizes ip,turn,allin
set_bet_sizes oop,river,bet,50
set_bet_sizes oop,river,donk,50
set_bet_sizes oop,river,raise,60,100
set_bet_sizes oop,river,allin
set_bet_sizes ip,river,bet,50
set_bet_sizes ip,river,raise,60,100
set_bet_sizes ip,river,allin
set_allin_threshold 0.67
build_tree
set_thread_num 8
set_accuracy 0.5
set_max_iteration 200
set_print_interval 10
set_use_isomorphism 1
start_solve
set_dump_rounds 2
dump_result output_result.json
```

修改这个文件来控制德州扑克游戏树的参数，算法的并行度，等等。 默认提供的输入文件中包含了一个典型的flop游戏树。

## 分析求解器产生的结果

结果的分析参考 [This section](https://github.com/bupticybee/TexasHoldemSolverJava#reading-the-solvers-output) java版本. C++ 版本和java版本的输出json格式是完全一致的。

![solving](imgs/see_result.gif)

在本地查看结果非常简单，只需要打开firefox（浏览器），然后把求解结果文件拖拽进去就可以。


## 编译源码

通常来说在安装本软件时不需要从头编译. 但是如果release包不能在你的电脑上运行，或者你想要更改本项目的源码，那么就需要重新编译，按照下面的指令重新编译：

你可以参考下面这个colab来进行编译 https://colab.research.google.com/drive/1NWDb53ypcKpkb3g3orzEBDeHAEkAIC7y

### Linux

```bash
mkdir build
cd build
cmake ..
make install
cd ../install
./console_solver -i resources/text/commandline_sample_input.txt
```

### Mac

```bash
mkdir build
cd build
cmake ..
make install
cd ../install
./console_solver -i resources/text/commandline_sample_input.txt
```

### Windows
使用visual studio 2019编译
```
mkdir vsbuild
cd vsbuild
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
nmake install
cd ../install
./console_solver.exe -i resources/text/commandline_sample_input.txt
```

编译之后的release包可以在工程根目录的 ```install``` 目录下被找到. 

# License

[GNU AGPL v3](https://www.gnu.org/licenses/agpl-3.0.en.html)

# Email

icybee@yeah.net
