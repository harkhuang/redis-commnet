project(redis-test)
cmake_minimum_required(VERSION 2.6)
set( CMAKE_BUILD_TYPE Debug )
include_directories(${CMAKE_SOURCE_DIR} )
aux_source_directory(src ${CMAKE_SOURCE_DIR} )
#add_executable(test adlist.c  ae.c  anet.c  dict.c  main.c  picol.c	redis.c  sds.c ${src})
add_executable(redis-test adlist.c  ae.c picol.c  dict.c  anet.c  redis.c sds.c  ${src})

