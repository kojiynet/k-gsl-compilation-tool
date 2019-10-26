
# kgslb
A tool to help build GNU Scientific Library

Copyright (C) 2013-2019 Koji Yamamoto a.k.a. utilcraft

The explanations are still imcomplete and mostly in Japanese only.
Sorry for inconvenience...

The explanations in English follow the Japanese ones.


# kgslbとは？

kgslbは、GNU Scientific Library（GSL）をビルドする際に、補助するツールです。

kgslbは、シンプルに、「どのソースファイルをどういう順番でコンパイルし、どのようにリンクすればよいか」を、具体化して、いわゆるDOSのバッチファイルの形式で提供します。kgslbは、「GSLをビルドするためのバッチファイル」を作成するのです。この際、GSLのMakefile.amの情報を用います。  

GSLをビルドするための、既存の方法として、以下のものがあげられます。

- Linux-compatibleなシェルで、./configureを実行した上で、GNU Makeを実行する。
- CMakeを使用する。GSLをビルドするためのCMakeList.txtは公開されている。

※詳しくは、次のリンクを参照してください。  
  [GSL - GNU Scientific Library - GNU Project - Free Software Foundation](https://www.gnu.org/software/gsl/)  
  [Building GSL on Windows Using Native Tools](https://www.gnu.org/software/gsl/extras/native_win_builds.html)

kgslbはこれらとは異なるアプローチを提供しています。

※GSLの使用にあたっては、GSLのライセンスを遵守しなければなりません。  
  
    
# kgslbの使用方法
  
  
## "kgslb.exe"の作成

`include`ディレクトリ内のファイルをincludeできるようにしながら、`kgslb.cpp`をコンパイルし、exeファイルを作成します。この際、Boost.Filesystemが必要です。
 
  
## 準備

kgslb.exeを実行する前に、以下の準備をします。

(1) GSLをダウンロードして展開します。  
展開後のGSLのトップディレクトリ内に、Makefile.amと、config.h.inが存在することを確認します。

(2) kgslbがビルド時に使用するワークスペースディレクトリを準備します。  
通常はkgslbの実行ファイル（.exeファイル）と同じディレクトリで構わないと思われます。  

(3)  ワークスペースディレクトリに、GSL内のconfig.h.inをコピーして、名称をconfig.hに変更する。  

(4)  ワークスペースディレクトリのconfig.hを、自分の環境に合わせて修正する。  
コンパイルにVCかGCCを使用する場合、コンパイラに以下の関数またはマクロが付属しているので、これをconfig.hに反映させるとよいでしょう。  
- isnan, isfinite, isinf
- GCCの場合には、さらに、finite
  
  
## kgslb.exeの実行

kgslbを実行する際、コンソールでオプションを指定します。  

`$ kgslb.exe -S<dir> [-W<dir>] [-C<compiler>]`

指定できるのは、以下の項目です。  

- ソースファイルディレクトリ (-S)  
- ワークスペースディレクトリ (-W)   
- コンパイラの種別 (-C)  

以下で説明します。  

(i)　  ソースファイルディレクトリ  
"-Sc:\xxx\xxx"のように指定します。  
このディレクトリとそのサブディレクトリから、ソースファイルを読み込んでコンパイルします。  
指定されるソースファイルディレクトリには、トップレベルのMakefile.amが存在しているとみなされます。  

(ii)　 ワークスペースディレクトリ  
"-Wc:\xxx\xxx"のように指定します。  
デフォルトでは、カレントディレクトリ（"."）が指定されます。  
ワークスペースには、最終的なライブラリが出力されます。  
また、ヘッダファイルのディレクトリ"gsl"がつくられます。  
さらに、GCCを使用する場合には、リンク用の一時ファイル（`tmp0000.txt`などの名称）が出力されますが、これはバッチファイル実行時に消去されます。この一時ファイルはバッチファイル実行に必要なので、その前にユーザが消去すると、バッチファイルで正常にビルドすることができません。  
  
(iii)　コンパイラの種別  
"-Cm"または"-Cg"で指定します。  
デフォルトでは、VC（"-Cm"）が指定されます。  
"-Cm"を指定すると、Microsoft Visual C++用のコマンドをバッチファイルに記述します。  
"-Cg"を指定すると、GCC用のコマンドをバッチファイルに記述します。  
  
  
## バッチファイル"kgslb.bat"の実行

kgslb.exeを実行すると、バッチファイル"kgslb.bat"がそのディレクトリに出力されます。  

このバッチファイルを実行すると、ビルドが始まります。この工程には通常かなりの時間がかかります。  

ビルドが成功すると、ワークスペースにライブラリファイルが出力されます。デフォルトのMakefile.amを用いている場合、libgsl.laとlibgslcblas.laが生成されることになります。また、GSL用のインクルードファイルがコピーされたgslというディレクトリも作成されます。  

ユーザがGSLを用いたプログラムを作成する場合、このgslにあるインクルードファイルをインクルードし、また、完成したライブラリファイルをリンクします。  
  
  
# Enjoy!! 

I hope you all enjoy hacking!
  
  
# Background （背景）

いやいや、Unix/Linuxのスクリプトとか、autotoolsとか、GNU makeとか、ちょっとわからないんで。。。  
  
ネット上で得られる、GSL用のCMakeのMakeListを用いても、自分の環境では正常に利用できるライブラリができなかったので。。
  
単純に、ビルドするバッチファイルがあればよいのでは！？  
  
…っていうコンセプトで、つくったものです。  

   
# What is kgslb? 

`kgslb` is a tool to ease building GNU Scientific Library (GSL).

`kgslb` will simply provide you with a batchfile for DOS shell which indicates "in what order source files should be compiled and liked." It will parse the files named `Makefile.am` accompaning with GSL.

Some existing and known methods to build GSL, for particularly DOS, include:

- Executing GNU MAKE after `./configure` on Linux-compatible shell
- Utilizing CMake; CMakeList.txt for building GSL is available on the Internet

For more detail, please see the following sites:  
  [GSL - GNU Scientific Library - GNU Project - Free Software Foundation](https://www.gnu.org/software/gsl/)  
  [Building GSL on Windows Using Native Tools](https://www.gnu.org/software/gsl/extras/native_win_builds.html)

Using `kgslb` is yet another way relative to these methods.

Note that, in implementing GSL's functionatilies, you should comply with the license of GSL.  
  

# How to Use kgslb?
  
  
## Building "kgslb.exe"

Complie `kgslb.cpp` while including the files in `include` directory. Then link the object file into exetutable file `kgslb.exe.` You will need `Boost.Filesystem` in doing this.

  
## Preparation 

Before running `kgslb.exe` you should prepare GSL's source files, etc., as follow.

### GSL Source Files

Download and get GSL's source files unpacked. Be sure that you have `Makefile.am` and `config.h.in` in GSL's top directory. 

### Workspace Directory

Prepare the workspace directory for `kgslb`. The same directory as the one containing `kgslb.exe` will be most convenient if you have no reason to avoid it.

### Preraring `config.h`

Copy `config.h.in` in GSL's top directory into the workspace directory. Then, rename it as `config.h`

Then, modify `config.h` according to your envrionment.  
Specific recommendations follow:
- VC:  
    VC has `isnan`, `isfinite`, and `isinf` functions (or macros), and so you should specify the existense of them in `config.h`.  
- GCC:  
    GCC has `finite`, in addition to `isnan`, `isfinite`, and `isinf` functions (or macros), and so you should specify the existense of them in `config.h`.  


## Running kgslb.exe 

COMING SOON!!


## Running the batchfile "kgslb.bat" 

![COMING SOON!!](https://github.com/kojiynet/kgslb/img/somingsoon.png)



# Misc
Please comply with the terms appearing in LICENSE file. In addition, please read REQUEST file and understand the content.  
  
I read the following interesitng posts and grateful to the writers:  
[code - Is there an open-source license that enforces citations? - Academia Stack Exchange](https://academia.stackexchange.com/questions/97480/is-there-an-open-source-license-that-enforces-citations)  
  
  
# Acknowledgement （謝辞）

I am grateful to the creators of GNU Scientific Library for the benefit we could obtain, in particular to the followings:

Mark Galassi, Jim Davies, James Theiler, Brian Gough, Reid Priedhorsky, Gerard Jungman, Michael Booth, Fabrice Rossi, Simone Piccardi, Carlo Perassi, Ho-Jin Dan, Szymon Jaroszewicz, Nicolas Darnis, Tuomo Keskitalo, Ivo Alxneit, Jason H. Stover, Patrick Alken, Rhys Ulerich, Pavel Holoborodko, Pedro Gonnet, and Free Software Foundation, Inc.

I am also grateful to TOMINAGA Daisuke, who has made considerable efforts to translate the GSL reference manual into Japanese.
