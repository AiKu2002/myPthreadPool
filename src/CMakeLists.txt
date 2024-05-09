AUX_SOURCE_DIRECTORY(./ SRC_LIST)
## 使用pthread 需要的  
## 查找到对应的包
set (PTHREADS_LIBRARIES "/usr/lib64/libpthread.so") 
set (PTHREADS_INCLUDE_DURES "/usr/include")
## 包含头文件
include_directories(${PTHREADS_INCLUDE_DIRS})

ADD_EXECUTABLE(MythreadPool ${SRC_LIST})

target_link_libraries(MythreadPool ${PTHREADS_LIBRARIES})
