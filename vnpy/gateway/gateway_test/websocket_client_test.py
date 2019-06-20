# encoding: UTF-8

import json
import ssl
import sys
import traceback
import socket
from datetime import datetime
from threading import Lock, Thread
from time import sleep

import websocket
from vnpy.gateway.gateway_test.my_log import MyLog

class WebsocketClient(object):
    """
    Websocket API

    After creating the client object, use start() to run worker and ping threads.
    The worker thread connects websocket automatically.

    Use stop to stop threads and disconnect websocket before destroying the client
    object (especially when exiting the programme).

    Default serialization format is json.

    Callbacks to overrides:
    * unpack_data
    * on_connected
    * on_disconnected
    * on_packet
    * on_error

    After start() is called, the ping thread will ping server every 60 seconds.

    If you want to send anything other than JSON, override send_packet.
    """

    def __init__(self):
        """Constructor"""
        self.host = None

        self._ws_lock = Lock()
        self._ws = None

        self._worker_thread = None
        self._ping_thread = None
        self._active = False

        self.proxy_host = None
        self.proxy_port = None
        self.ping_interval = 10     # seconds
        self.header = {}

        # For debugging
        self._last_sent_text = None
        self._last_received_text = None

        self.log = MyLog()

    def write_log(self, msg):
        self.log.logger.info(msg)

    def init(self, host: str, proxy_host: str = "", proxy_port: int = 0, ping_interval: int = 60, header: dict = None):
        """
        :param ping_interval: unit: seconds, type: int
        """
        self.host = host
        self.ping_interval = ping_interval  # seconds

        if header:
            self.header = header

        if proxy_host and proxy_port:
            self.proxy_host = proxy_host
            self.proxy_port = proxy_port

    def start(self):
        """
        Start the client and on_connected function is called after webscoket
        is connected succesfully.

        Please don't send packet untill on_connected fucntion is called.
        """

        self._active = True
        self._worker_thread = Thread(target=self._run)
        self._worker_thread.start()

        self._ping_thread = Thread(target=self._run_ping)
        self._ping_thread.start()

    def stop(self):
        """
        Stop the client.
        """
        self._active = False
        self._disconnect()

    def join(self):
        """
        Wait till all threads finish.

        This function cannot be called from worker thread or callback function.
        """
        self._ping_thread.join()
        self._worker_thread.join()

    def send_packet(self, packet: dict):
        """
        Send a packet (dict data) to server

        override this if you want to send non-json packet
        """
        text = json.dumps(packet)
        self._record_last_sent_text(text)
        return self._send_text(text)

    def _send_text(self, text: str):
        """
        Send a text string to server.
        """
        ws = self._ws
        if ws:
            ws.send(text, opcode=websocket.ABNF.OPCODE_TEXT)

    def _send_binary(self, data: bytes):
        """
        Send bytes data to server.
        """
        ws = self._ws
        if ws:
            ws._send_binary(data)

    def _reconnect(self):
        """"""
        if self._active:
            self.write_log("_reconnect: 断开连接")
            self._disconnect()
            self.write_log("_reconnect: 重新连接")
            self._connect()

    def _create_connection(self, *args, **kwargs):
        """"""
        return websocket.create_connection(*args, **kwargs)

    def _connect(self):
        """这里需要修改"""
        self._ws = self._create_connection(
            self.host,
            sslopt={"cert_reqs": ssl.CERT_NONE},
            http_proxy_host=self.proxy_host,
            http_proxy_port=self.proxy_port,
            header=self.header
        )
        self.on_connected()

    def _disconnect(self):
        """
        """
        with self._ws_lock:
            # TODO 这个地方是最终调试出来出问题的地方
            # OKEX把连接踢掉之后，根本不存在self._ws
            # 但是这里的重连机制是，只有_ws的时候，才会重连
            # 因此，对Vnpy原生版本的最小化改动为 修改是否存在_ws,重连的时候，都要设置为None
            #
            if self._ws:
                self._ws.close()
            self._ws = None

    def _run(self):
        """
        Keep running till stop is called.
        """
        try:
            self._connect()
            # todo: onDisconnect
            while self._active:
                try:
                    ws = self._ws
                    if ws:
                        text = ws.recv()

                        # ws object is closed when recv function is blocking

                        if not text:
                            self.write_log("_run: 收不到text")
                            self._reconnect()
                            continue
                        self._record_last_received_text(text)

                        try:
                            data = self.unpack_data(text)
                        except ValueError as e:
                            self.write_log("_run: 无法解压text")
                            print("websocket unable to parse data: " + text)
                            raise e
                        self.on_packet(data)
                        self.write_log("_run: 解压的data " + str(data))
                # ws is closed before recv function is called
                # For socket.error, see Issue #1608
                except (websocket.WebSocketConnectionClosedException, socket.error):
                    self.write_log("_run: WebsocketConnectionClosedException和socket.error")
                    self._reconnect()

                # other internal exception raised in on_packet
                except Exception:  # noqa
                    self.write_log("_run: socket.error other wrong")
                    et, ev, tb = sys.exc_info()
                    self.on_error(et, ev, tb)
                    self._reconnect()
        except:  # noqa
            self.write_log("_run: _connect() except")
            et, ev, tb = sys.exc_info()
            self.on_error(et, ev, tb)
            self._reconnect()

    @staticmethod
    def unpack_data(data: str):
        """
        Default serialization format is json.

        override this method if you want to use other serialization format.
        """
        return json.loads(data)

    def _run_ping(self):
        """"""
        while self._active:
            try:
                self.write_log("_run_ping: send ping")
                self._ping()
            except Exception:  # noqa
                self.write_log("_run_ping: wrong reconnect()")
                et, ev, tb = sys.exc_info()
                self.on_error(et, ev, tb)
                self._reconnect()
            for i in range(self.ping_interval):
                if not self._active:
                    break
                sleep(1)

    def _ping(self):
        """"""
        ws = self._ws
        if ws:
            self.write_log("_ping: send ping")
            ws.send("ping", websocket.ABNF.OPCODE_PING)

    @staticmethod
    def on_connected():
        """
        Callback when websocket is connected successfully.
        """
        pass

    @staticmethod
    def on_disconnected():
        """
        Callback when websocket connection is lost.
        """
        pass

    @staticmethod
    def on_packet(packet: dict):
        """
        Callback when receiving data from server.
        """
        pass

    def on_error(self, exception_type: type, exception_value: Exception, tb):
        """
        Callback when exception raised.
        """
        sys.stderr.write(
            self.exception_detail(exception_type, exception_value, tb)
        )
        return sys.excepthook(exception_type, exception_value, tb)

    def exception_detail(
        self, exception_type: type, exception_value: Exception, tb
    ):
        """
        Print detailed exception information.
        """
        text = "[{}]: Unhandled WebSocket Error:{}\n".format(
            datetime.now().isoformat(), exception_type
        )
        text += "LastSentText:\n{}\n".format(self._last_sent_text)
        text += "LastReceivedText:\n{}\n".format(self._last_received_text)
        text += "Exception trace: \n"
        text += "".join(
            traceback.format_exception(exception_type, exception_value, tb)
        )
        return text

    def _record_last_sent_text(self, text: str):
        """
        Record last sent text for debug purpose.
        """
        self._last_sent_text = text[:1000]

    def _record_last_received_text(self, text: str):
        """
        Record last received text for debug purpose.
        """
        self._last_received_text = text[:1000]
