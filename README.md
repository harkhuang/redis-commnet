[toc]


# redis-from-0.1-to-3.0
source code for reading and comment

# source code before 2.6.14 
[download](https://code.google.com/archive/p/redis/downloads?page=1)

# source code before 2.6.14 
[download](http://download.redis.io/releases/)







# 版本0.6beta测试版本中不具备稳定性,建议使用1.0版本,不仅添加了使用文档,同时添加了性能测试工具
redis数据库与传统数据库在思想上有很大区别






# 压力测试  对比mysql

##　环境
主频2.7GHz，
cpu:i5-5257U
memory:8g
硬盘:stat
# redis单机性能 
qps: 5w~6w
# mysql单机
qps: 5w~6w

# 分析原因
1.表比较小,cache命中几率很大问题,测试数据不具有代表性
2.redis的性能是稳定的,不论内容有多少.哪怕满载情况下,寻址也是很快的
3.mysql优化有几个重点的分析,cache存在大大提升了数据库运行的效率
4.mysql在执行sql中有一种优化机制,查询预判,在做了某个查询之后,mysql服务会为客户端准备好下次预查到数据,即利用空闲时间,负载cpu,让cpu和磁盘利用率达到最高



# 压力测试  对比memcache (https://timyang.net/data/mcdb-tt-redis/)
1.version

```
db-4.7.25.tar.gz
libevent-1.4.11-stable.tar.gz
memcached-1.2.8.tar.gz
memcachedb-1.2.1-beta.tar.gz
redis-0.900_2.tar.gz
tokyocabinet-1.4.9.tar.gz
tokyotyrant-1.1.9.tar.gz
```
2.Small data size test result
```
Test 1, 1-5,000,000 as key, 100 bytes string value, do set, then get test, all get test has result.
Request per second(mean)key-value-performance-1(Update)
```

| Store | Write | Read |
|--------|--------|--------|
Memcached  |	55,989  |		50,974
Memcachedb  |		25,583  |		35,260
Tokyo Tyrant  |		42,988	  |	46,238
Redis	  |	85,765  |		71,708



Server Load Average
Store	|Write	|Read
|--------|--------|--------|
Memcached	|1.80, 1.53, 0.87	|1.17, 1.16, 0.83
MemcacheDB|	1.44, 0.93, 0.64	|4.35, 1.94, 1.05
Tokyo Tyrant|	3.70, 1.71, 1.14|	2.98, 1.81, 1.26
Redis	|1.06, 0.32, 0.18	|1.56, 1.00, 0.54






3. Larger data size test result
```
Test 2, 1-500,000 as key, 20k bytes string value, do set, then get test, all get test has result.
Request per second(mean)
(Aug 13 Update: fixed a bug on get() that read non-exist key)
````



| column | column | column |
|--------|--------|--------|
| Store  |Write   |Read|
|Memcachedb	|357|	327
|Tokyo Tyrant|	3,501|	257
|Redis	|1,542|	957




# 关于思考
单进程的redis相对多进程的优势在哪里?
常用的服务模型
1.多进程处理模型 
消耗资源占用空间,同时并发处理性能非常差
2.单进程线程池处理模型
线程池,控制力生产消费处理,使得cpu利用率达到最佳,但同时带来的问题是,大量并发处理也存在超时.(那么问题来超时的问题应该如何处理呢?)
3.select处理模型
4.epoll处理模型




# 对比aliredis


1. 单进程单线程, 无法充分发挥服务器多核cpu的性能.
2. 大流量下造成IO阻塞. 同样是由于单进程单线程, cpu在处理业务逻辑的时候,网络IO被阻塞住, 造成无法处理更多的请求.
3. 维护成本高, 如果想要充分发挥服务器的所有资源包括cpu, 网络io等, 就必须建立多个instance, 但此时不可避免会增加维护成本.  拿24核服务器举例来讲, 如果部署24个单机版的instance,理论上可以实现10w*24core= 240wQPS的总体性能.但是每个 instance 有各自独立的数据,占用资源如内存也会同比上升,反过来制约一台服务器又未必能支持这么多的 instance.  如果部署24个Instance来构成单机集群, 虽然可以共享数据，但是因为节点增加, redis的状态通讯更加频繁和费时,性能也下会降很多.  并且两种方式都意味着要维护24个Instance，运维成本都会成倍增加. 

4. 持久化. redis提供了两种save方式 1)save触发. 2)bgsave. 当然也可以使用3)aof来实现持久化, 但是这3点都有弊端.
         1)save:  由于是单进程单线程, redis会阻塞住所有请求, 来遍历所有redisDB, 把key-val写入dump.rdb. 如果内存数据量过大, 会造成短时间几秒到几十秒甚至更长的时间停止服务, 这种方案对于twitter, taobao等大流量的网站, 显然是不可取的.  
         2)bgsave: 在触发bgsave时, redis会fork自身, child进程会进入1)的处理方式,这意味着服务器内存要有一半的冗余才可以, 如今内存已变得越来越廉价, 但是对于存储海量数据的情况,内存以及服务器的成本还是不容忽视的. 
         3)aof:  说到持久化, redis提供的aof算是最完美的方案了, 但是有得必有失, 严重影响性能! 因为redis每接收到一条请求, 就要把命令内容完整的写到磁盘文件, 且不说频繁读写会影响磁盘寿命,写磁盘的时间足以拖垮redis整体性能 . 当然熟悉redis的开发者会想到用appendfsync等参数来调整, 但都不是完美.即使使用 SSD，性能也只是略有提升，并且性价比不高。
 
针对以上几种情况, 阿里技术保障团队做了如下优化手段, 其实这不仅仅只是优化, 而更是一种对redis的改造.
1. 多线程master + N*work 工作模式.master线程负责监听网络事件, 在接收到一个新的连接后, master会把新的fd注册到worker的epoll事件中, 交由worker处理这个fd的所有读写事件, 这样master线程就可以完全被释放出来接收更多的连接, 同时又不妨碍worker处理业务逻辑和IO读写.

采用这种master + N*worker的网络层事件模型,可以实现redis性能的平行扩展. 真正的让redis在面临高并发请求时可以丛容面对.
2. 抛弃save, bgsave, aof等三种模式.采用redisDB lock模式. AliRedis在数据存储层把多DB存储模式转换成HashDb存储, 将key hash到所有RedisDB上, 这样做有一个弊端就是抛弃了select命令, 但与此同时会带来一个更大的好处, 可以逐个DB持久化而不会影响整个系统, 在做持久化的时候AliRedis只需lock住1/N个redisDb, 占用1/m个线程. 在不需要内存冗余的情况下进行持久化, 相比之前提到的弊端, 这种方式可以带来更大的收益, 更丰厚的回报.
3. 重构hiredis客户端, 支持redis-cluster工作模式, 目前hiredis并不支持redis-cluster模式, 阿里技术保障团队对hiredis进行重构,使之支持redis-cluster.
4. 优化jemalloc, 采用大内存页.  Redis在使用内存方面可谓苛刻至极, 压缩, string转number等, 能省就省, 但是在实际生产环境中, 为了追求性能, 对于内存的使用可以适度（不至于如bgsave般浪费）通融处理, 因此AliRedis对jemalloc做了微调, 通过调整pagesize来让一次je_malloc分配更多run空间来储备更多的用户态可用内存, 同时可以减轻换页表的负载, 降低user sys的切换频率, 来提高申请内存的性能, 对jemalloc有兴趣的开发者可以参考jemalloc源码中的bin, run, chunk数据结构进行分析.


通过如上改造, redis可以充分发挥服务器多核的优势, 以及网络IO复用, 特别是节省运维成本, 每台服务器只需维护一个AliRedis实例.

#　总结
