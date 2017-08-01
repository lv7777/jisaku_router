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

* inet_aton

address(192.168.114.51)をint型のアレ(全部16になるやつ)に変換する。
ntopとの違いはなんだろ。

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

# Makefileの=と:=の違い

=は遅延評価

:=は即時評価

基本的には=の方が安心だね

# proc/sys以下

proc/sys以下はシステムに関する情報を表示するだけでなく、カーネルの状態を即時に切り替えることもできる。
sysctlのラッパーの内部ではここいじってるんだろうなきっと

# struct poll

selectと同じ効果。fdのイベントを監視。recvはブロッキングな動作をする（jsでいう同期的な）ため、2つのfdを扱うときに使う。

受信可能か、痩身可能か、エラーがあるかなども扱える。

select自身はrecvと同じようにblockする。selectは受信用、送信用、エラー用、等の構造体を受け取ってそれらの構造体に含まれるディスクリプタが受信、送信、エラー可能になるとブロックが解除される。

selectのメインの機能としてFD_ISSET()があってこいつをifで使うことで始めてselectをちゃんとしようしたことになる。

FD_ISSETなかったらただのrecvと同じだからね。

pollの場合はstruct pollfdの配列を作ってその配列の要素ごとに1つのソケットディスクリプタを入れる。  

pollはイベントが発生していたら1以上の数値(イベントが発生してるソケットディスクリプタの数)をだしてそれ以外は-1を出す。

# arpプロトコル

hwtype,upper protocol,hlen,upper protocol length,operation type,smac,sip,dmac,dip

# icmp protocol

icmptype,icmpcode,checksum, depend with icmptype

* icmpcode...depend with icmp type.

main user is icmp unreachabletype(write to reason),time exceed(if ttl0 time exceed , write 00)

# pthread\_attr\_t

スレッドの属性設定に使う。

こいつをpthread\_create関数の第２引数に渡すと属性設定済みのスレッドが作成できるっぽい。

* inaddr

sockaddr\_in構造体はソケットの包括的な情報でその中にin\_addr構造体を含む

in\_addrはin\_addr\_tを持つ構造体

in\_addr\_tはuint32みたいな感じの型。

sockaddr\_llはsockaddr\_inのlink layer版。

* リアルタイムに

tcpdump -i wlan0 -U -n -w /tmp/test.pcap tail -f /tmp/test.pcap| wireshark -k -S -i -
