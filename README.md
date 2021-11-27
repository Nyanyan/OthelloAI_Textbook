# OthelloAI_Textbook
記事集「オセロAIの教科書」のサンプルプログラム



## 概要

このレポジトリは記事集[オセロAIの教科書](https://note.com/nyanyan_cubetech/m/m54104c8d2f12)のサンプルプログラムです。



## 使用言語

オセロAIの教科書では、AIとインターフェースを分離していて、都合によりAI自体はC++で、AIで遊ぶインターフェース部分はPythonで書いています。Pythonの方は実行さえできれば良いです。



## 使用ツール

Pythonにsubprocessというライブラリを用いています。ない場合は

```
$ pip3 install subprocess
```

を実行してください。

また、C++のコンパイルをする必要があります。コンパイラにはg++を推奨していますが、何でも動くと思います。



## 使用方法

各ディレクトリ内で、C++で書かれたAIをコンパイルします。

```
$ g++ ai.cpp -o ai.out -O3
```

### GUI(わかりやすい画面を表示する場合)

tkinterというPythonのライブラリが必要です。ない場合は以下のコマンドでインストールしてください。

```
$ pip3 install tkinter
```

以下のコマンドのどちらかでPythonで書かれたインターフェースを実行します。

```
$ python3 main.py
```

または

```
$ python3 main.py gui
```

### CUI(コンソール画面で遊ぶ場合)

以下のコマンドを実行します。

```
$ python3 main.py cui
```



## 目次

記事集全体はこちら: https://note.com/nyanyan_cubetech/m/m54104c8d2f12

1. 概観
2. ボードの実装
3. 1手読みAI
4. minimax
5. alphabeta
6. negascout
7. 評価関数
8. 定石
9. 終盤探索
10. 枝刈り
11. 高速化

