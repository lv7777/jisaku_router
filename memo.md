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

htonsのhはhost。nはnetwork。間違えやすいので注意。host to network short.

逆にntohsもある。


* inet\_ntop,inet\_pton

nはnetwork。pはplain text（多分）

パックされた人間に読めないネットワークアドレスをstringにする。

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

socket関数の第２引数にはSOCK\_STREAMとSOCK\_RAWの２つがある。

* AF\_INET,PF\_INET

address family inet,protocol family inet.

実態としてはPF\_INETはAF\_INETのエイリアス。

* INADDR\_ANY

0.0.0.0

* struct ether\_arp,ether\_header

netinet/if\_ether.hに存在している。

* struct iphdr

netinet/ip.hに存在している。

iphdr.tot\_lenはトータルlengthのこと。「バージョン、ヘッダ長、サービスタイプ、全長」の全長。

iphdr.ihlはipヘッダ長のこと。分かり辛い・・・・

その他のプロトコルの構造体もnetinet以下に存在してるっぽい。

* struct ip6\_hdr

netinet/ip6.hに存在している。

ip6の場合はipとはずれている。
例えばipのヘッダ長がprotocolに対して、ip6\_hdrだと、ip6\_hdr->ip6_nxtみたいな感じになる。

ip6\_hdr->ip6\_flowはフロータイプ、ip4のqos的な奴（すっかり忘れてた)

* sockaddr_ll

linklayer sock addr。

netpacket/packet.hに定義されてる。

タダのsockaddrのほうはsys/socket.hにある。

* arpa/inet.h

ntohsやhtonsなどが定義されている。

* ifreqとioctl

ioctlはsys/ioctl.hに存在しておりifreq構造体はlinux/if.h、もしくはnet/if.hに存在している。

## ネットワークにおけるチェックサム

ネットワークに置けるチェックサムは全て同じ。16bitごとの１の補数をとってそれを和してそいつの１の補数をとる感じ？

0001,1000,0010=FFFE,EFFF,FFEF=EFEC

EFECの１の補数は0x1012

### 単位元

X+0x0000=の和はXの一の補数と同じになるはず。

こんな感じでXにyを足してもXの補数と同じになる的なものを単位元という。この他には0xFFFFがある。

e.g.

0x1010の１の補数・・・0x0101

0x1010+0x0000の1の補数0x0101
0x1010+0xFFFFの1の補数=1100F,

0x0000は+0,0xFFFFは-0

つまりFFFEに+1すればFFFF、FFFFは-0(0)なのでそのつぎは1になる。FFFF->0001

~(0x1010)=EFEF,
0x1010+0xFFFF=1(100F)
~(0x1010+0xFFFF)=EFF1

(0xFFFF+(~0x1010)).toString(16).toUpperCase()

# register修飾子

レジスタに当てはめるやつ