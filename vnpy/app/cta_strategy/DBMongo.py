#!/usr/bin/python
# -*- coding: utf-8 -*-

from pymongo import MongoClient, ASCENDING
from pymongo.errors import ConnectionFailure, DuplicateKeyError

MONGOHOST = "localhost"
MONGOPORT = 27017


class dbMongo(object):

    def __init__(self):
        self.connect()

    # ----------------------------------------------------------------------
    def connect(self):
        """连接MongoDB数据库"""

        # 读取MongoDB的设置
        try:
            # 设置MongoDB操作的超时时间为0.5秒
            self.dbClient = MongoClient(MONGOHOST, MONGOPORT, connectTimeoutMS=500)

            # 调用server_info查询服务器状态，防止服务器异常并未连接成功
            self.dbClient.server_info()

        except ConnectionFailure:
            print('连接失败')

    # ----------------------------------------------------------------------
    def dbInsert(self, dbName, collectionName, d):
        """向MongoDB中插入数据，d是具体数据"""
        try:
            db = self.dbClient[dbName]
            collection = db[collectionName]
            collection.insert_one(d)
        except DuplicateKeyError:
            print('重复插入，插入失败，请使用更新')

    # ----------------------------------------------------------------------
    def dbQuery(self, dbName, collectionName, d, sortKey='', sortDirection=ASCENDING):
        """从MongoDB中读取数据，d是查询要求，返回的是数据库查询的指针"""
        try:
            db = self.dbClient[dbName]
            collection = db[collectionName]

            if sortKey:
                cursor = collection.find(d).sort(sortKey, sortDirection)  # 对查询出来的数据进行排序
            else:
                # cursor = collection.find(d, projection={"_id": False})
                cursor = collection.find(d)

            if cursor:
                return list(cursor)
            else:
                return []
        except:
            print('查询失败')
            return []

    # ----------------------------------------------------------------------
    def dbUpdate(self, dbName, collectionName, d, flt, upsert=False):
        """向MongoDB中替换数据，d是具体数据，flt是过滤条件，upsert代表若无是否要插入"""
        try:
            db = self.dbClient[dbName]
            collection = db[collectionName]
            # collection.ensure_index([('trade_date', 1)], unique=True)
            # collection.update_one(flt, d, upsert=True)
            collection.replace_one(flt, d, upsert)
        except Exception as e:
            print('更新失败: ' + str(e))

    # ----------------------------------------------------------------------
    def dbUpdate_one(self, dbName, collectionName, old_d, new_d, upsert=False):
        """向MongoDB中更新数据，d是具体数据，flt是过滤条件，upsert代表若无是否要插入"""
        try:
            db = self.dbClient[dbName]
            collection = db[collectionName]
            # collection.ensure_index([('trade_date', 1)], unique=True)
            collection.update_one(old_d, new_d, upsert)
        except Exception as e:
            print('更新失败: ' + str(e))

    # ----------------------------------------------------------------------
    def dbDelete(self, dbName, collectionName, flt):
        """从数据库中删除数据，flt是过滤条件"""
        try:
            db = self.dbClient[dbName]
            collection = db[collectionName]
            collection.delete_one(flt)
        except:
            print('删除数据')


