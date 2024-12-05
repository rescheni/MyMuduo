cmake_minimum_required(VERSION 2.5)

project(mymuduo VERSION 0.1)

# mymuduo 最终编译成 so 动态库  设置动态库路径  放在 根目录的 lib 文件夹下
set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
# 设置 调试 信息 启动 c++ 11 语言标准
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

aux_source_directory(./ SRC_LIST)

# 编译动态库    SHARED 生成动态库 mymuduo 
add_library(mymuduo SHARED ${SRC_LIST})


