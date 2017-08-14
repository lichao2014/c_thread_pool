cmake_minimum_required(VERSION 2.8)

project(c_thread_pool)

aux_source_directory(src SRC)
file(GLOB INC src/*.h)

if (NOT WIN32)
	list(REMOVE_ITEM INC src/pthread_win32.h)
	list(REMOVE_ITEM SRC src/pthread_win32.c)
endif()

add_library(c_thread_pool ${SRC} ${INC})

include_directories( src)

if (UNIX)
	link_libraries(pthread)
endif()

link_directories(D:/third_party/boost_1_64_0/vc14.1/lib)

add_executable(test tests/test.cpp)
target_include_directories(test PRIVATE src)
target_link_libraries(test PRIVATE c_thread_pool)

add_executable(ppl_test tests/ppl_test.cpp)

add_executable(boost_test tests/boost_test.cpp)
target_include_directories(boost_test PRIVATE D:/third_party/boost_1_64_0)

add_executable(cpp_test tests/cpp_test.cpp)
target_include_directories(cpp_test PRIVATE cpp)