# linuxネットワークプログラミング概要

windowsだとOS経由で触れるのは3層までだったけどlinuxでは2層も扱える.

linuxでIP,TCP,UDP,Ethを扱う方法はすべて同じ。socket関数を使う。でもそれぞれ引数に渡す定数が違う。

windowsだとwinsockでポートにバインドするけどlinuxの場合はソケットディスクリプタ使う。

# linuxのNW系のインクルードヘッダについて

* sys/ioctl

sys/ioctlの中のioctl関数はスペシャルファイルを扱うための関数。

システムコールとも関係あるっぽい。

