# linuxネットワークプログラミング概要

windowsだとOS経由で触れるのは3層までだったけどlinuxでは2層も扱える.

linuxでIP,TCP,UDP,Ethを扱う方法はすべて同じ。socket関数を使う。でもそれぞれ引数に渡す定数が違う。

windowsだとwinsockでポートにバインドするけどlinuxの場合はソケットディスクリプタ使う。

# linuxのNW系のインクルードヘッダについて

* sys/ioctl

sys/ioctlの中のioctl関数はスペシャルファイルを扱うための関数。

システムコールとも関係あるっぽい。

___
2層を操るためにはいつものsys/socket.hとかに加えてlinux/if.hとかnetpacket/packet.hとかnetinet/if_ether.hとかが必要。

# 関数

* htons

バイトオーダー関連の奴。winsockでも使えるっぽい。

* ioctl

ioctlはいろんなコマンド（操作）を発行できる。
その中でもSIOCGIFINDEXはifの一覧を得ることができる

* perror

stdio.h内に定義されている。print error。stderrに出力するが通常はstderrはstdioなので問題ない。

ついでにエラーが起こったときのエラーコードも表示してくれる。

* u\_char,u\_int

unsigned charのエイリアス。sys/types.hで定義されてるっぽい。

* socket

socketは失敗してもエラーを出さない。そのためperrorで確認してやる必要がある。