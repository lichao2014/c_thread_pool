cmake_minimum_required(VERSION 2.8)

project(c_thread_pool)

aux_source_directory(src SRC)
file(GLOB INC src/*.h)

if (NOT WIN32)
	list(REMOVE_ITEM INC src/pthread_win32.h)
	list(REMOVE_ITEM SRC src/pthread_win32.c)
endif()

add_library(c_thread_pool ${SRC} ${INC})

include_directories(D:/third_party/boost_1_64_0 src)

link_libraries(c_thread_pool)
if (UNIX)
	link_libraries(pthread)
endif()

link_directories(D:/third_party/boost_1_64_0/vc14.1/lib)

add_executable(test samples/test.cpp)
add_executable(ppl_test samples/ppl_test.cpp)
add_executable(boost_test samples/boost_test.cpp)

