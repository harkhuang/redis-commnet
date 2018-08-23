project(redis-test)

cmake_minimum_required(VERSION 2.6)
set( CMAKE_BUILD_TYPE Debug )
include_directories(${CMAKE_SOURCE_DIR} )
aux_source_directory(src ${CMAKE_SOURCE_DIR} )
include_directories(.)
#add_executable(test adlist.c  ae.c  anet.c  dict.c  main.c  picol.c	redis.c  sds.c ${src})
add_compile_options("-g")
add_executable(redis-test adlist.c adlist.h ae.h ae.c picol.c dict.h dict.c  anet.h anet.c redis.c sds.h sds.c )


#可以同时添加多个目标
ADD_CUSTOM_TARGET(
    TestExample redis-test
)


ADD_CUSTOM_COMMAND(TARGET redis-test
        PRE_BUILD
        COMMAND  echo  you can  do  some thing before build...
        COMMENT "cmd  before build" 
)

ADD_CUSTOM_COMMAND(TARGET redis-test
        POST_BUILD  #在项目构建完成之后做的事情
        COMMAND cp redis-test ..
        COMMENT "cmd after  build" 
)



