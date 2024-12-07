cmake_minimum_required(VERSION 2.5)

project(
	mymuduo
	)

# mymuduo 最终编译成 so 动态库  设置动态库路径  放在 根目录的 lib 文件夹下
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置 调试 信息 启动 c++ 11 语言标准
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -fPIC")

aux_source_directory(./ SRC_LIST)

# 编译动态库    SHARED 生成动态库 mymuduo 
add_library(mymuduo SHARED ${SRC_LIST})


