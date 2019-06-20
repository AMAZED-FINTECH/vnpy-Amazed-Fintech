版本说明：
此版本为Amazed-Fintech基于vnpy原生版本的改动。

20190618版本：
目标：以测试策略vnpy.app.cta_strategy.test_strategy_crypto为基础，跑通Okex交易所的测试。
修改：成交数量，以最小单位0.0000001为单位进行测试
修改：将所有write_log形式写入MongoDB数据库

20190620:
- 备忘：Okex的websocket每隔一段时间会强制的断开所有用户的连接，所以，要及时的重连

当前要测试Okex的websocket，断开重连的机制，是否合适
测试的脚本：gateway 目录下 gateway_test 
okey_gateway_test.py + websocket_client_test.py + test.py

- bug websocket_client里面的的问题是最终调试出来的问题

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

