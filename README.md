make コマンドでbuildできます。
make clean で生成したオブジェクトファイルを削除します。

インタープリタ的なものです。

'( ) + * / % 'などが使えます。演算子の優先順位も定義 。()を含んだ式も評価可能（したつもりだったけどうまくいってない。そのうち直す。)

グローバル環境に変数を束縛できます。(ex. hoge = 100)。if,else,などの構文っぽいものは予約語なので使えません。)

適当なソースコードを外部から読み込んで構文解析する機能は未完成。





<<コマンド>>


quit:プログラムを終了

printvar(pv): 変数を表示

printstack(ps): スタックを表示     

print x: x(任意の式)を評価し出力(改行なし)      

println x:  x(任意の式)を評価し出力(改行あり)

