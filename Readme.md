# 7-Zip

Copyright (C) 1999-2016 Igor Pavlov.

Licenses for files are:

1. 7z.dll: GNU LGPL + unRAR restriction
2. All other files:  GNU LGPL

The GNU LGPL + unRAR restriction means that you must follow both 
GNU LGPL rules and unRAR restriction rules.

## Customization

Copyright (C) 2010 CubeSoft, Inc.

このプロジェクトは、CubeICE 用に 7-Zip をカスタマイズしたものです。
具体的には、文字コードの判別および変換処理を独自のものに置き換えています。
7-Zip/CubeICE では、文字コードの判別および変換処理を以下のライブラリを用いて実装しています。

* バベル -babel-
  URL: http://tricklib.com/cxx/ex/babel/
  License: http://tricklib.com/license.htm

## How to use

7-Zip/CubeICE は、以下のようなブランチ構造となっています。

* master ... 7-Zip オリジナルのソースコードを管理するブランチ
* cubeice ... CubeICE 用のカスタマイズを反映したブランチ

ビルドする際には、以下の手順で実行して下さい。

1. babel をビルドする (/MT, _fastcall (/Gr))
2. babel.h と babel.lib を適当なディレクトリに配置する
3. インクルードディレクトリ、ライブラリディレクトリに 2. で設置したディレクトリのパスを追加する