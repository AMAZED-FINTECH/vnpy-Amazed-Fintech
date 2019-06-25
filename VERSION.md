#版本说明：

此版本为Amazed-Fintech基于vnpy原生版本的改动。

&nbsp;

##20190618：

- 目标：以测试策略vnpy.app.cta_strategy.test_strategy_crypto为基础，跑通Okex交易所的测试。
- 修改：成交数量，以最小单位0.0000001为单位进行测试
- 修改：将所有write_log形式写入MongoDB数据库

&nbsp;

## 20190620:
- 备忘：Okex的websocket每隔一段时间会强制的断开所有用户的连接，所以，要及时的重连

当前要测试Okex的websocket，断开重连的机制，是否合适
测试的脚本：gateway 目录下 gateway_test 
okey_gateway_test.py + websocket_client_test.py + test.py

- bug:: websocket_client里面的的问题是最终调试出来的问题

OKEX把连接踢掉之后，根本不存在self._ws
但是这里的重连机制是，只有_ws的时候，才会重连
因此，对Vnpy原生版本的最小化改动为 修改是否存在_ws,重连的时候，无论是否存在_ws，都要设置为None

```
def _disconnect(self):
   
    with self._ws_lock:
        if self._ws:
            self._ws.close()    
            self._ws = None
        # 修改为
        if self._ws:
            self._ws.close()    
        self._ws = None
```

再进行测试

- 小问题，Okex接口，vnpy返回的是世界时间，而Okexf返回的是世界时间

咨询 ZP 之后，选择中国时间作为可视化的返回结果

对okex_gateway内容进行ticker和depth的修改

注意：vnpy内认为的tick是depth内的tick，而不是ticker的tick，修改depth内tick的时间推送，才生效

&nbsp;

## 20190621:
已调试好OKEX币币交易gateway所有逻辑，以及自动订阅
原问题出现在自动重连之后，并不自动订阅，导致行情不更新
现已修正，使其能够在半分钟之内自动重连

需要增加的内容：query_history 主引擎中添加了访问历史数据，这个历史数据，只能从restful api里面获得

&nbsp;

## 20190622:
OKEX OKEXM OKEXF OKEXS
分别为币币交易 杠杆交易 合约交易 永续交易
四个接口当前能够实现获取行情并自动重连，重连时间不超过30秒，使用ping的方式
由于OKEX的Ping的时间就是30秒，因此，不会有更短时间的解决办法

对合约部分修改较大的地方就是原来的设计是查询合约之后，开启websocket
但是，由于OKEX断线之后，可能出现原来合约不存在的情况，需要重新获取所有合约
在查询合约之后开启websocket线程，会导致不断的重新开启线程
因此，将开启线程放在查询账户之后

其他如币币交易，杠杆交易，永续交易，不存在交割合约的概念，因此，不用改


接下来，需要调试四个接口的下单，撤单，账户记录，等情况。
需要对CTA Engine部分进行增加下单账户等数据实时插入数据库的，进而可以完成账户与策略实时监控

&nbsp;

## 20190625
添加以下功能，预计可实现第一版本：
- 1 OKEX的历史KBar调取与推送
- 2 需要添加EVENT_BAR与gateway父类内添加之后的推送
- 3 OKEX的自动订阅,添加1分钟K线的自动订阅与合成
- 4 本地数据库记录,在CtaEngine中process order trade position account 与本地数据库的交互

原因一是对于数字货币,交易所本身就实时推送K线数据,而不像商品期货,没有交易所K线数据
原因二是对于数字货币,活跃合约的tick数据量太大,不像商品期货,半秒推送一个,数据量这么大,
   本地合成K线容易出错,造成实盘和回测信号对不上
因此,综合考虑,添加上述功能

- 这样设计之后,由gateway可以发出Bar,推送至事件处理器,推送至register EVENT_BAR的函数，
    原作者设计的一些原理
    

