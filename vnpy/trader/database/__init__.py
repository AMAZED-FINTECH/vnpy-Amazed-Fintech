import os
from typing import TYPE_CHECKING

# TYPE_CHECKING默认False，如果为True，就引入数据库管理
if TYPE_CHECKING:
    from vnpy.trader.database.database import BaseDatabaseManager

# 如果在环境变量中没有发现VNPY_TESTING，也就是测试
if "VNPY_TESTING" not in os.environ:
    # 引入配置
    from vnpy.trader.setting import get_settings
    # 引入初始化
    from .initialize import init

    # 配置，要修改全局setting，使之变成MongoDB
    settings = get_settings("database.")
    database_manager: "BaseDatabaseManager" = init(settings=settings)
