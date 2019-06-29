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
- 3 OKEX的自动订阅,添加1分钟K线的自动订阅
- 4 本地数据库记录,在CtaEngine中process order trade position account on Bar与本地数据库的交互

原因一是对于数字货币,交易所本身就实时推送K线数据,而不像商品期货,没有交易所K线数据
原因二是对于数字货币,活跃合约的tick数据量太大,不像商品期货,半秒推送一个,数据量这么大,
   本地合成K线容易出错,造成实盘和回测信号对不上
因此,综合考虑,添加上述功能

- 这样设计之后,由gateway可以发出Bar,推送至事件处理器,推送至register EVENT_BAR的函数，
    原作者设计的一些原理
    
## 20190626
- 1 OKEXF的历史KBAR调取与推送，后续可修改为每次都推送2000条bar,当前不需要
    四个接口都已经修改！
    这里学习一下:如果要写到路径下的,用params的参数,如果要写到{}的内容中,用data参数
```buildoutcfg
        def query_history(self, req: HistoryRequest):
        """TODO 因为Okex推送2000条数据，但是这里默认只推送200条"""

        params = {
            "granularity": INTERVAL_VT2OKEX[req.interval],
            "start": "",
            "end": "",
        }

        path = "/api/futures/v3/instruments/" + req.symbol + "/candles"
        self.add_request(
            "GET",
            path,
            callback=self.on_query_history,
            data=data,
            params=params,
            on_error=self.on_error,
            on_failed=self.on_failed,
            extra=req
        )

    def on_query_history(self, data, req):
        """调取历史数据"""
        # OKEX返回的是倒叙的K线,需要重新排列
        for i in range(len(data)):
            d = data[-i - 1]
            dt = utc_to_local(d[0])
            bar = BarData(
                symbol=req.extra.symbol,
                exchange=req.extra.exchange,
                datetime=dt,
                interval=req.extra.interval,
                volume=float(d[5]),
                open_price=float(d[1]),
                high_price=float(d[2]),
                low_price=float(d[3]),
                close_price=float(d[4]),
                gateway_name=self.gateway_name
            )
            self.gateway.on_bar(bar)

```

- 2 添加EVENT_BAR与gateway推送
```buildoutcfg
    def on_bar(self, bar: BarData):
        """
        Bar event push.
        Bar event of a specific vt_symbol is also pushed.
        """
        self.on_event(EVENT_BAR, bar)
        self.on_event(EVENT_BAR + bar.vt_symbol, bar)

```
- 3 OKEX的自动订阅,添加1分钟K线的自动订阅
###### 修改subscribe_topic
```buildoutcfg
    def subscribe_topic(self):
        """
        Subscribe to all private topics.
        """
        self.callbacks["futures/ticker"] = self.on_ticker
        self.callbacks["futures/depth5"] = self.on_depth
        self.callbacks["futures/account"] = self.on_account
        self.callbacks["futures/order"] = self.on_order
        self.callbacks["futures/position"] = self.on_position
        self.callbacks["futures/candle60s"] = self.on_candle

        # Subscribe to order update
        channels = []
        for instrument_id in instruments:
            channel = f"futures/order:{instrument_id}"
            channels.append(channel)

        req = {
            "op": "subscribe",
            "args": channels
        }
        self.send_packet(req)

        # Subscribe to account update
        channels = []
        for currency in currencies:
            if currency != "USD":
                channel = f"futures/account:{currency}"
                channels.append(channel)

        req = {
            "op": "subscribe",
            "args": channels
        }
        self.send_packet(req)

        # Subscribe to position update
        channels = []
        for instrument_id in instruments:
            channel = f"futures/position:{instrument_id}"
            channels.append(channel)

        req = {
            "op": "subscribe",
            "args": channels
        }
        self.send_packet(req)

        # 这里添加gateway的文件，以保证能够一开始就订阅，这里算是改动比较大的了
        # 为了减少服务器压力，这里只需要订阅需要的合约，也就是季度交割合约
        for futures in sub_instruments:
            req = SubscribeRequest(symbol=futures, exchange=Exchange.OKEX)
            self.subscribe(req)

```
###### 修改subscribe
```buildoutcfg
    def subscribe(self, req: SubscribeRequest):
        """
        Subscribe to tick data upate.
        订阅ticker和bar
        """
        tick = TickData(
            symbol=req.symbol,
            exchange=req.exchange,
            name=req.symbol,
            datetime=datetime.now(),
            gateway_name=self.gateway_name,
        )
        self.ticks[req.symbol] = tick

        bar = BarData(
            symbol=req.symbol,
            exchange=req.exchange,
            datetime=copy(datetime.now()),
            interval=Interval.MINUTE,
            gateway_name=self.gateway_name,
        )
        self.bars[req.symbol] = bar
        self.sub_flags[req.symbol] = True

        channel_ticker = f"futures/ticker:{req.symbol}"
        channel_depth = f"futures/depth5:{req.symbol}"
        channel_candle = f"futures/candle60s:{req.symbol}"

        self.callbacks[channel_ticker] = self.on_ticker
        self.callbacks[channel_depth] = self.on_depth
        self.callbacks[channel_candle] = self.on_candle

        req = {
            "op": "subscribe",
            "args": [channel_ticker, channel_depth, channel_candle]
        }
        self.send_packet(req)
```
###### 添加on_candle,细节,考虑第一次连接推送的Bar.
```buildoutcfg
    def on_candle(self, d):
        """"""
        symbol = d["instrument_id"]
        bar = self.bars.get(symbol, None)
        if not bar:
            return

        # 由于OKEX每根Bar会推送很多次,不只一次,这里设置成,只有当这根Bar走完之后才推送
        candle = d["candle"]
        dt = utc_to_local(candle[0])

        # 如果时间戳和本地不等,推送Bar
        if dt != bar.datetime:
            if not self.sub_flags[symbol]:
                self.gateway.on_bar(copy(bar))
                print(bar.__dict__)
            else:
                self.sub_flags[symbol] = False
            bar.volume = float(candle[5])
            bar.open_price = float(candle[1])
            bar.high_price = float(candle[2])
            bar.low_price = float(candle[3])
            bar.close_price = float(candle[4])
            bar.datetime = dt
        else:
            bar.volume = float(candle[5])
            bar.open_price = float(candle[1])
            bar.high_price = float(candle[2])
            bar.low_price = float(candle[3])
            bar.close_price = float(candle[4])
            bar.datetime = dt

```
###### 修改cta_engine
- 1 添加EVENT_BAR
- 2 将原load_bar废除,完全重写,每个gateway有其自己的query_history的方法
- 3 注册事件中多了一个process_bar,要仿照on_tick来写
- 4 注册事件中多了一个process_account,插入数据库用
- 5 向CtaEngine中加入MongoDB的插入,每个插入的
- init_engine 中不启动米匡,没有账号启动干什么
- CtaEngine里面添加accountid,由于数字货币市场的机制,使得其并没有传统的accountid的概念,
因此,很有必要在其中添加一个accountid作为识别（当前vnpy设计均为单账户）
- 添加accountid是因为在插入数据库的时候,数据库的名称就是accountid的名称,documents包括：
order trade position account 在测试策略的时候,还需要记录Bar或Tick数据

## 20190627
- 备忘 不同的gateway->不同的交易所名称->最好不要出现同一个交易所名称这种情况（增加OKEXF的交易所）
- 主要原因就是vnpy里面的bar的vt_symbol包括tick的vt_symbol是用Exchange.value来命名
- 备忘 OKEX的合成K线的方式不明,但是确定的是: OKEX的分钟KBar和本地tick合成的分钟KBar不一样
- 因此要想实盘和回测更贴近,必须用交易所的分钟K线,由于交易所的分钟K线合成5分钟,30分钟,1小时等采取左闭右开的模式，可以本地合成
- tick数据用来做风控与本地停止单,不做其他功能


## 20190628
- 检测中官方的CtaEngine出现逻辑问题,在sync_strategy_data的时候,采用的是w+的形式写入json
- 如果同名策略,在两次写入的时候,后面的参数是最新的,但是在第二次载入的时候,就会出现问题
- CtaEngine每次初始化的时候,用的都是strategy_name作为策略的唯一标识符
- 因此如果不将原来的同名策略配置删除,策略不会读取到最新的策略配置文件
- 因此,为了统一,将载入策略与存储策略参数都统一到数据库内
- 改： CtaEngine里面的load_strategy_setting和save_strategy_setting,从数据库中拿

- 1 修改CtaEngine里面的load_strategy_setting
- 2 修改CtaEngine里面的update_strategy_setting
- 3 修改CtaEngine里面的remove_strategy_setting
- 4 修改CtaEngine里面的load_strategy_data
- 5 修改CtaEngine里面的sync_strategy_data







