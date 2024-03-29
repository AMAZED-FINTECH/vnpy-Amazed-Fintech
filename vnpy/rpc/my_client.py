from __future__ import print_function
from __future__ import absolute_import
from time import sleep, time

from vnpy.rpc import RpcClient


class MyClient(RpcClient):
    """
    Test RpcClient   
    """

    def __init__(self, req_address, sub_address):
        """
        Constructor
        """
        super(MyClient, self).__init__(req_address, sub_address)

    def callback(self, topic, data):
        """
        Realize callable function
        """
        print('client received topic:', topic, ', data:', data, " ", time())


if __name__ == '__main__':
    req_address = 'tcp://localhost:2014'
    sub_address = 'tcp://localhost:0602'

    tc = MyClient(req_address, sub_address)
    tc.subscribeTopic('')
    tc.start()

    while 1:
        print(tc.add(1, 3))
        sleep(2)
