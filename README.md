# kgslb
A tool to help build GNU Scientific Library

Copyright (C) 2013-2019 Koji Yamamoto a.k.a. utilcraft

The explanations are still imcomplete and mostly in Japanese only.
Sorry for inconvenience...
  
  
# What is kgslb? （kgslbとは？）

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
  
  
# How to Use kgslb? （kgslbの使用方法）
  
  
## Preparation （準備）

kgslbを実行する前に、以下の準備をします。

(1) GSLをダウンロードして展開します。  
展開後のGSLのトップディレクトリ内に、Makefile.amと、config.h.inが存在することを確認します。

(2) kgslbがビルド時に使用するワークスペースディレクトリを準備します。  
通常はkgslbの実行ファイル（.exeファイル）と同じディレクトリで構わないと思われます。  

(3)  ワークスペースディレクトリに、GSL内のconfig.h.inをコピーして、名称をconfig.hに変更する。  

(4)  ワークスペースディレクトリのconfig.hを、自分の環境に合わせて修正する。  
コンパイルにVCかGCCを使用する場合、コンパイラに以下の関数またはマクロが付属しているので、これをconfig.hに反映させるとよいでしょう。  
- isnan, isfinite, isinf
- GCCの場合には、さらに、finite
  
  
## Running kgslb.exe （kgslb.exeの実行）

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
さらに、GCCを使用する場合には、リンク用の一時ファイルが出力されますが、これはバッチファイル実行時に消去されます。この一時ファイルはバッチファイル実行に必要なので、その前にユーザが消去すると、バッチファイルで正常にビルドすることができません。  
  
(iii)　コンパイラの種別  
"-Cm"または"-Cg"で指定します。  
デフォルトでは、VC（"-Cm"）が指定されます。  
"-Cm"を指定すると、Microsoft Visual C++用のコマンドをバッチファイルに記述します。  
"-Cg"を指定すると、GCC用のコマンドをバッチファイルに記述します。  
  
  
## Running the batchfile "kgslb.bat" （バッチファイル"kgslb.bat"の実行）

kgslb.exeを実行すると、バッチファイル"kgslb.bat"がそのディレクトリに出力されます。  

このバッチファイルを実行すると、ビルドが始まります。この工程には通常かなりの時間がかかります。  

ビルドが成功すると、ワークスペースにライブラリファイルが出力されます。デフォルトのMakefile.amを用いている場合、libgsl.laとlibgslcblas.laが生成されることになります。また、GSL用のインクルードファイルがコピーされたgslというディレクトリも作成されます。  

ユーザがGSLを用いたプログラムを作成する場合、このgslにあるインクルードファイルをインクルードし、また、完成したライブラリファイルをリンクします。  
  
  
# Enjoy!! 

I hope you all enjoy hacking!
  
  
# Acknowledgement （謝辞）

I am grateful to the creators of GNU Scientific Library for the benefit we could obtain, in particular to the followings:

Mark Galassi, Jim Davies, James Theiler, Brian Gough, Reid Priedhorsky, Gerard Jungman, Michael Booth, Fabrice Rossi, Simone Piccardi, Carlo Perassi, Ho-Jin Dan, Szymon Jaroszewicz, Nicolas Darnis, Tuomo Keskitalo, Ivo Alxneit, Jason H. Stover, Patrick Alken, Rhys Ulerich, Pavel Holoborodko, Pedro Gonnet, and Free Software Foundation, Inc.

I am also grateful to Daisuke Tominaga, who has made considerable efforts to translate the GSL reference manual into Japanese.
