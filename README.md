(Like a)Interpreter  
===================  

build
------

	make 
でビルドできます.

	make clean

で生成したオブジェクトファイルを削除します.  



使い方
------
インタープリタを起動後

	load ./test/filename.aaa

でファイルを読み込み

	run

で読み込んだソースコードを実行します.  
読み込んだソースコードは実行前に

	list

で確認できます。  
プログラムを終了するときは

	exit(or quit)


関数定義
--------

	def 関数名
		処理
	enddef

で関数を定義できます.

関数呼び出し
-----------

	call 関数名(引数）

"でサブルーチンを呼び出します.引数も指定できます.  

条件分岐
--------

	if 条件式 then 
		処理 
	else
		処理
	endif

で条件分岐(elseは省略可）  

繰り返し処理
------------

	for 初期化(ex.i=0) to　終了条件 
		処理 
	next i

ヘルプコマンド
-------------
コマンドライン上で

	help

でヘルプを見ることができます.

コメントアウト  
--------------

	// hogehoge


+ オプション  
	+ ```printvar``` or ``` pv```     - 定義した変数を表示.  
	+ ```printstack``` or ```ps```    - 現在のスタックを表示.   
	+ ```print x```                   - x(任意の式)を評価し出力(改行なし).   
	+ ```println x```                 - x(任意の式)を評価し出力(改行あり)     
	+ ```pritvar``` or ```pv```       - 変数表を出力します。(pvでも可)  
	+ ```printstack``` or ```pv```    - スタックの内容を表示します。(psでも可)  
	+ ```print x```                   - xを表示します.  
	+ ```println x```                 - xを表示したあと改行します.  
	+ ```tokenstat``` or ```ts```     - トークンの状態を出力しながら処理する。(tsでも可).  
	+ ```displine```  or ```dl```     - 実行中の行を出力しながら処理する。(dlでも可).  
	+ ```stackstat``` or ```ss```     - スタックの状態を出力しながら処理する。(ssでも可).  


+ 補足  
	+ プログラムの一行目から```end```までがmainモジュールとなるので、関数定義はその後に記述します.  
	+ ソースコードをファイルから読み込ませなくても、四則計算やオプションの利用であればインタープリタのコマンドライン上で直接実行することもできます.
	+ 演算子の優先順位も定義しているので,```2 + 5 * 8``` と``` 2 * 5 + 8``` は異なる結果となります.
	+ プログラムの一行目から順番にコードを追いつつメモリ上に保存しているため,ネストしていたり再帰的なコードは実行できません.  
