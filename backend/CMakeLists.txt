cmake_minimum_required(VERSION 3.14)
project(tomtom_backend C)

#set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#set(BUILD_SHARED_LIBS OFF)
#set(CMAKE_EXE_LINKER_FLAGS "-static")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb -fno-plt -pipe -fasynchronous-unwind-tables")
set(CMAKE_C_FLAGS_RELEASE "-O3 -fomit-frame-pointer -fno-plt -pipe -s")

set(CMAKE_C_FLAGS_TRACE "-O0 -ggdb -fno-plt -pipe -fasynchronous-unwind-tables -fno-omit-frame-pointer -fsanitize=thread -fsanitize=undefined")
set(CMAKE_EXE_LINKER_FLAGS_TRACE "-fsanitize=thread -fsanitize=undefined")
set(CMAKE_MODULE_LINKER_FLAGS_TRACE "-fsanitize=thread -fsanitize=undefined")

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "Trace")
	add_compile_definitions(DEBUG)
endif ()

include_directories("third-party/libwebsockets/include")
link_directories("third-party/libwebsockets/lib")
include_directories("third-party/flatcc/include")
link_directories("third-party/flatcc/lib")

include_directories("api-schema/request/gen")
include_directories("api-schema/response/gen")

add_executable(tomtom_backend
		src/cert.h
		src/globals.h
		src/main.c src/main.h
		src/processing.c src/processing.h
		src/utils.c src/utils.h
		src/api/register.c src/api/register.h
		src/api/login.c src/api/login.h
		src/api/get_me.c src/api/get_me.h
		src/api/edit_me.c src/api/edit_me.h
		src/api/request_contract.c src/api/request_contract.h
		src/api/list_contracts.c src/api/list_contracts.h
		src/api/check_contract.c src/api/check_contract.h
		)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(OpenSSL REQUIRED)

target_link_libraries(tomtom_backend
		websockets
		flatccrt
		Threads::Threads
		PostgreSQL::PostgreSQL
		OpenSSL::Crypto)
