#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/a293c7f6/glm.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_inputs_manager.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_logger.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_lua.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_network.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_object.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_system_pch.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_task.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_tcp_client.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_thread.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_thread_pool.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_time_span.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_window.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_xml.o \
	${OBJECTDIR}/_ext/26f1a4f1/wolf.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../../dependencies/tbb/oss/linux/lib/intel64/gcc4.7/ -L../../../dependencies/luaJIT/lib/linux/x64/debug -L../../../dependencies/msgpack/lib/linux/x64/debug -L../../../dependencies/nanomsg/lib/linux/x64/debug -lxcb -ltbb_debug -ltbbmalloc_debug -lluajit-static -lmsgpackc -lnanomsg -lanl -lrt -lpthread -lnsl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/libwolf.system.${CND_DLIB_EXT}

${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/libwolf.system.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}
	gcc -o ${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/libwolf.system.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/_ext/a293c7f6/glm.o: ../../../src/wolf.system/glm/detail/glm.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/a293c7f6
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__linux -D_DEBUG -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D__LUA__ -D__WOLF_SYSTEM__ -DNN_HAVE_ACCEPT4=1 -DNN_HAVE_BACKTRACE=1 -DNN_HAVE_CLOCK_GETTIME=1 -DNN_HAVE_CLOCK_MONOTONIC=1 -DNN_HAVE_EPOLL=1 -DNN_HAVE_EVENTFD=1 -DNN_HAVE_GCC_ATOMIC_BUILTINS -DNN_HAVE_GETADDRINFO_A=1 -DNN_HAVE_LIBNSL=1 -DNN_HAVE_LINUX -DNN_HAVE_MSG_CONTROL=1 -DNN_HAVE_PIPE2=1 -DNN_HAVE_PIPE=1 -DNN_HAVE_POLL=1 -DNN_HAVE_SEMAPHORE -DNN_HAVE_SEMAPHORE_PTHREAD=1 -DNN_HAVE_SOCKETPAIR=1 -DNN_HAVE_UNIX_SOCKETS=1 -DNN_MAX_SOCKETS=512 -DNN_STATIC_LIB -D_GNU_SOURCE -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -D_THREAD_SAFE -I../../../src/wolf.system -I../../../dependencies/luaJIT/include -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/nanomsg/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a293c7f6/glm.o ../../../src/wolf.system/glm/detail/glm.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_inputs_manager.o: ../../../src/wolf.system/w_inputs_manager.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__linux -D_DEBUG -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D__LUA__ -D__WOLF_SYSTEM__ -DNN_HAVE_ACCEPT4=1 -DNN_HAVE_BACKTRACE=1 -DNN_HAVE_CLOCK_GETTIME=1 -DNN_HAVE_CLOCK_MONOTONIC=1 -DNN_HAVE_EPOLL=1 -DNN_HAVE_EVENTFD=1 -DNN_HAVE_GCC_ATOMIC_BUILTINS -DNN_HAVE_GETADDRINFO_A=1 -DNN_HAVE_LIBNSL=1 -DNN_HAVE_LINUX -DNN_HAVE_MSG_CONTROL=1 -DNN_HAVE_PIPE2=1 -DNN_HAVE_PIPE=1 -DNN_HAVE_POLL=1 -DNN_HAVE_SEMAPHORE -DNN_HAVE_SEMAPHORE_PTHREAD=1 -DNN_HAVE_SOCKETPAIR=1 -DNN_HAVE_UNIX_SOCKETS=1 -DNN_MAX_SOCKETS=512 -DNN_STATIC_LIB -D_GNU_SOURCE -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -D_THREAD_SAFE -I../../../src/wolf.system -I../../../dependencies/luaJIT/include -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/nanomsg/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_inputs_manager.o ../../../src/wolf.system/w_inputs_manager.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_logger.o: ../../../src/wolf.system/w_logger.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__linux -D_DEBUG -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D__LUA__ -D__WOLF_SYSTEM__ -DNN_HAVE_ACCEPT4=1 -DNN_HAVE_BACKTRACE=1 -DNN_HAVE_CLOCK_GETTIME=1 -DNN_HAVE_CLOCK_MONOTONIC=1 -DNN_HAVE_EPOLL=1 -DNN_HAVE_EVENTFD=1 -DNN_HAVE_GCC_ATOMIC_BUILTINS -DNN_HAVE_GETADDRINFO_A=1 -DNN_HAVE_LIBNSL=1 -DNN_HAVE_LINUX -DNN_HAVE_MSG_CONTROL=1 -DNN_HAVE_PIPE2=1 -DNN_HAVE_PIPE=1 -DNN_HAVE_POLL=1 -DNN_HAVE_SEMAPHORE -DNN_HAVE_SEMAPHORE_PTHREAD=1 -DNN_HAVE_SOCKETPAIR=1 -DNN_HAVE_UNIX_SOCKETS=1 -DNN_MAX_SOCKETS=512 -DNN_STATIC_LIB -D_GNU_SOURCE -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -D_THREAD_SAFE -I../../../src/wolf.system -I../../../dependencies/luaJIT/include -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/nanomsg/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_logger.o ../../../src/wolf.system/w_logger.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_lua.o: ../../../src/wolf.system/w_lua.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__linux -D_DEBUG -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D__LUA__ -D__WOLF_SYSTEM__ -DNN_HAVE_ACCEPT4=1 -DNN_HAVE_BACKTRACE=1 -DNN_HAVE_CLOCK_GETTIME=1 -DNN_HAVE_CLOCK_MONOTONIC=1 -DNN_HAVE_EPOLL=1 -DNN_HAVE_EVENTFD=1 -DNN_HAVE_GCC_ATOMIC_BUILTINS -DNN_HAVE_GETADDRINFO_A=1 -DNN_HAVE_LIBNSL=1 -DNN_HAVE_LINUX -DNN_HAVE_MSG_CONTROL=1 -DNN_HAVE_PIPE2=1 -DNN_HAVE_PIPE=1 -DNN_HAVE_POLL=1 -DNN_HAVE_SEMAPHORE -DNN_HAVE_SEMAPHORE_PTHREAD=1 -DNN_HAVE_SOCKETPAIR=1 -DNN_HAVE_UNIX_SOCKETS=1 -DNN_MAX_SOCKETS=512 -DNN_STATIC_LIB -D_GNU_SOURCE -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -D_THREAD_SAFE -I../../../src/wolf.system -I../../../dependencies/luaJIT/include -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/nanomsg/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_lua.o ../../../src/wolf.system/w_lua.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_network.o: ../../../src/wolf.system/w_network.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__linux -D_DEBUG -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D__LUA__ -D__WOLF_SYSTEM__ -DNN_HAVE_ACCEPT4=1 -DNN_HAVE_BACKTRACE=1 -DNN_HAVE_CLOCK_GETTIME=1 -DNN_HAVE_CLOCK_MONOTONIC=1 -DNN_HAVE_EPOLL=1 -DNN_HAVE_EVENTFD=1 -DNN_HAVE_GCC_ATOMIC_BUILTINS -DNN_HAVE_GETADDRINFO_A=1 -DNN_HAVE_LIBNSL=1 -DNN_HAVE_LINUX -DNN_HAVE_MSG_CONTROL=1 -DNN_HAVE_PIPE2=1 -DNN_HAVE_PIPE=1 -DNN_HAVE_POLL=1 -DNN_HAVE_SEMAPHORE -DNN_HAVE_SEMAPHORE_PTHREAD=1 -DNN_HAVE_SOCKETPAIR=1 -DNN_HAVE_UNIX_SOCKETS=1 -DNN_MAX_SOCKETS=512 -DNN_STATIC_LIB -D_GNU_SOURCE -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -D_THREAD_SAFE -I../../../src/wolf.system -I../../../dependencies/luaJIT/include -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/nanomsg/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_network.o ../../../src/wolf.system/w_network.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_object.o: ../../../src/wolf.system/w_object.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__linux -D_DEBUG -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D__LUA__ -D__WOLF_SYSTEM__ -DNN_HAVE_ACCEPT4=1 -DNN_HAVE_BACKTRACE=1 -DNN_HAVE_CLOCK_GETTIME=1 -DNN_HAVE_CLOCK_MONOTONIC=1 -DNN_HAVE_EPOLL=1 -DNN_HAVE_EVENTFD=1 -DNN_HAVE_GCC_ATOMIC_BUILTINS -DNN_HAVE_GETADDRINFO_A=1 -DNN_HAVE_LIBNSL=1 -DNN_HAVE_LINUX -DNN_HAVE_MSG_CONTROL=1 -DNN_HAVE_PIPE2=1 -DNN_HAVE_PIPE=1 -DNN_HAVE_POLL=1 -DNN_HAVE_SEMAPHORE -DNN_HAVE_SEMAPHORE_PTHREAD=1 -DNN_HAVE_SOCKETPAIR=1 -DNN_HAVE_UNIX_SOCKETS=1 -DNN_MAX_SOCKETS=512 -DNN_STATIC_LIB -D_GNU_SOURCE -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -D_THREAD_SAFE -I../../../src/wolf.system -I../../../dependencies/luaJIT/include -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/nanomsg/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_object.o ../../../src/wolf.system/w_object.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_system_pch.o: ../../../src/wolf.system/w_system_pch.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__linux -D_DEBUG -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D__LUA__ -D__WOLF_SYSTEM__ -DNN_HAVE_ACCEPT4=1 -DNN_HAVE_BACKTRACE=1 -DNN_HAVE_CLOCK_GETTIME=1 -DNN_HAVE_CLOCK_MONOTONIC=1 -DNN_HAVE_EPOLL=1 -DNN_HAVE_EVENTFD=1 -DNN_HAVE_GCC_ATOMIC_BUILTINS -DNN_HAVE_GETADDRINFO_A=1 -DNN_HAVE_LIBNSL=1 -DNN_HAVE_LINUX -DNN_HAVE_MSG_CONTROL=1 -DNN_HAVE_PIPE2=1 -DNN_HAVE_PIPE=1 -DNN_HAVE_POLL=1 -DNN_HAVE_SEMAPHORE -DNN_HAVE_SEMAPHORE_PTHREAD=1 -DNN_HAVE_SOCKETPAIR=1 -DNN_HAVE_UNIX_SOCKETS=1 -DNN_MAX_SOCKETS=512 -DNN_STATIC_LIB -D_GNU_SOURCE -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -D_THREAD_SAFE -I../../../src/wolf.system -I../../../dependencies/luaJIT/include -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/nanomsg/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_system_pch.o ../../../src/wolf.system/w_system_pch.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_task.o: ../../../src/wolf.system/w_task.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__linux -D_DEBUG -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D__LUA__ -D__WOLF_SYSTEM__ -DNN_HAVE_ACCEPT4=1 -DNN_HAVE_BACKTRACE=1 -DNN_HAVE_CLOCK_GETTIME=1 -DNN_HAVE_CLOCK_MONOTONIC=1 -DNN_HAVE_EPOLL=1 -DNN_HAVE_EVENTFD=1 -DNN_HAVE_GCC_ATOMIC_BUILTINS -DNN_HAVE_GETADDRINFO_A=1 -DNN_HAVE_LIBNSL=1 -DNN_HAVE_LINUX -DNN_HAVE_MSG_CONTROL=1 -DNN_HAVE_PIPE2=1 -DNN_HAVE_PIPE=1 -DNN_HAVE_POLL=1 -DNN_HAVE_SEMAPHORE -DNN_HAVE_SEMAPHORE_PTHREAD=1 -DNN_HAVE_SOCKETPAIR=1 -DNN_HAVE_UNIX_SOCKETS=1 -DNN_MAX_SOCKETS=512 -DNN_STATIC_LIB -D_GNU_SOURCE -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -D_THREAD_SAFE -I../../../src/wolf.system -I../../../dependencies/luaJIT/include -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/nanomsg/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_task.o ../../../src/wolf.system/w_task.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_tcp_client.o: ../../../src/wolf.system/w_tcp_client.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__linux -D_DEBUG -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D__LUA__ -D__WOLF_SYSTEM__ -DNN_HAVE_ACCEPT4=1 -DNN_HAVE_BACKTRACE=1 -DNN_HAVE_CLOCK_GETTIME=1 -DNN_HAVE_CLOCK_MONOTONIC=1 -DNN_HAVE_EPOLL=1 -DNN_HAVE_EVENTFD=1 -DNN_HAVE_GCC_ATOMIC_BUILTINS -DNN_HAVE_GETADDRINFO_A=1 -DNN_HAVE_LIBNSL=1 -DNN_HAVE_LINUX -DNN_HAVE_MSG_CONTROL=1 -DNN_HAVE_PIPE2=1 -DNN_HAVE_PIPE=1 -DNN_HAVE_POLL=1 -DNN_HAVE_SEMAPHORE -DNN_HAVE_SEMAPHORE_PTHREAD=1 -DNN_HAVE_SOCKETPAIR=1 -DNN_HAVE_UNIX_SOCKETS=1 -DNN_MAX_SOCKETS=512 -DNN_STATIC_LIB -D_GNU_SOURCE -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -D_THREAD_SAFE -I../../../src/wolf.system -I../../../dependencies/luaJIT/include -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/nanomsg/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_tcp_client.o ../../../src/wolf.system/w_tcp_client.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_thread.o: ../../../src/wolf.system/w_thread.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__linux -D_DEBUG -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D__LUA__ -D__WOLF_SYSTEM__ -DNN_HAVE_ACCEPT4=1 -DNN_HAVE_BACKTRACE=1 -DNN_HAVE_CLOCK_GETTIME=1 -DNN_HAVE_CLOCK_MONOTONIC=1 -DNN_HAVE_EPOLL=1 -DNN_HAVE_EVENTFD=1 -DNN_HAVE_GCC_ATOMIC_BUILTINS -DNN_HAVE_GETADDRINFO_A=1 -DNN_HAVE_LIBNSL=1 -DNN_HAVE_LINUX -DNN_HAVE_MSG_CONTROL=1 -DNN_HAVE_PIPE2=1 -DNN_HAVE_PIPE=1 -DNN_HAVE_POLL=1 -DNN_HAVE_SEMAPHORE -DNN_HAVE_SEMAPHORE_PTHREAD=1 -DNN_HAVE_SOCKETPAIR=1 -DNN_HAVE_UNIX_SOCKETS=1 -DNN_MAX_SOCKETS=512 -DNN_STATIC_LIB -D_GNU_SOURCE -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -D_THREAD_SAFE -I../../../src/wolf.system -I../../../dependencies/luaJIT/include -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/nanomsg/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_thread.o ../../../src/wolf.system/w_thread.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_thread_pool.o: ../../../src/wolf.system/w_thread_pool.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__linux -D_DEBUG -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D__LUA__ -D__WOLF_SYSTEM__ -DNN_HAVE_ACCEPT4=1 -DNN_HAVE_BACKTRACE=1 -DNN_HAVE_CLOCK_GETTIME=1 -DNN_HAVE_CLOCK_MONOTONIC=1 -DNN_HAVE_EPOLL=1 -DNN_HAVE_EVENTFD=1 -DNN_HAVE_GCC_ATOMIC_BUILTINS -DNN_HAVE_GETADDRINFO_A=1 -DNN_HAVE_LIBNSL=1 -DNN_HAVE_LINUX -DNN_HAVE_MSG_CONTROL=1 -DNN_HAVE_PIPE2=1 -DNN_HAVE_PIPE=1 -DNN_HAVE_POLL=1 -DNN_HAVE_SEMAPHORE -DNN_HAVE_SEMAPHORE_PTHREAD=1 -DNN_HAVE_SOCKETPAIR=1 -DNN_HAVE_UNIX_SOCKETS=1 -DNN_MAX_SOCKETS=512 -DNN_STATIC_LIB -D_GNU_SOURCE -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -D_THREAD_SAFE -I../../../src/wolf.system -I../../../dependencies/luaJIT/include -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/nanomsg/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_thread_pool.o ../../../src/wolf.system/w_thread_pool.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_time_span.o: ../../../src/wolf.system/w_time_span.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__linux -D_DEBUG -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D__LUA__ -D__WOLF_SYSTEM__ -DNN_HAVE_ACCEPT4=1 -DNN_HAVE_BACKTRACE=1 -DNN_HAVE_CLOCK_GETTIME=1 -DNN_HAVE_CLOCK_MONOTONIC=1 -DNN_HAVE_EPOLL=1 -DNN_HAVE_EVENTFD=1 -DNN_HAVE_GCC_ATOMIC_BUILTINS -DNN_HAVE_GETADDRINFO_A=1 -DNN_HAVE_LIBNSL=1 -DNN_HAVE_LINUX -DNN_HAVE_MSG_CONTROL=1 -DNN_HAVE_PIPE2=1 -DNN_HAVE_PIPE=1 -DNN_HAVE_POLL=1 -DNN_HAVE_SEMAPHORE -DNN_HAVE_SEMAPHORE_PTHREAD=1 -DNN_HAVE_SOCKETPAIR=1 -DNN_HAVE_UNIX_SOCKETS=1 -DNN_MAX_SOCKETS=512 -DNN_STATIC_LIB -D_GNU_SOURCE -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -D_THREAD_SAFE -I../../../src/wolf.system -I../../../dependencies/luaJIT/include -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/nanomsg/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_time_span.o ../../../src/wolf.system/w_time_span.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_window.o: ../../../src/wolf.system/w_window.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__linux -D_DEBUG -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D__LUA__ -D__WOLF_SYSTEM__ -DNN_HAVE_ACCEPT4=1 -DNN_HAVE_BACKTRACE=1 -DNN_HAVE_CLOCK_GETTIME=1 -DNN_HAVE_CLOCK_MONOTONIC=1 -DNN_HAVE_EPOLL=1 -DNN_HAVE_EVENTFD=1 -DNN_HAVE_GCC_ATOMIC_BUILTINS -DNN_HAVE_GETADDRINFO_A=1 -DNN_HAVE_LIBNSL=1 -DNN_HAVE_LINUX -DNN_HAVE_MSG_CONTROL=1 -DNN_HAVE_PIPE2=1 -DNN_HAVE_PIPE=1 -DNN_HAVE_POLL=1 -DNN_HAVE_SEMAPHORE -DNN_HAVE_SEMAPHORE_PTHREAD=1 -DNN_HAVE_SOCKETPAIR=1 -DNN_HAVE_UNIX_SOCKETS=1 -DNN_MAX_SOCKETS=512 -DNN_STATIC_LIB -D_GNU_SOURCE -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -D_THREAD_SAFE -I../../../src/wolf.system -I../../../dependencies/luaJIT/include -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/nanomsg/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_window.o ../../../src/wolf.system/w_window.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_xml.o: ../../../src/wolf.system/w_xml.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__linux -D_DEBUG -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D__LUA__ -D__WOLF_SYSTEM__ -DNN_HAVE_ACCEPT4=1 -DNN_HAVE_BACKTRACE=1 -DNN_HAVE_CLOCK_GETTIME=1 -DNN_HAVE_CLOCK_MONOTONIC=1 -DNN_HAVE_EPOLL=1 -DNN_HAVE_EVENTFD=1 -DNN_HAVE_GCC_ATOMIC_BUILTINS -DNN_HAVE_GETADDRINFO_A=1 -DNN_HAVE_LIBNSL=1 -DNN_HAVE_LINUX -DNN_HAVE_MSG_CONTROL=1 -DNN_HAVE_PIPE2=1 -DNN_HAVE_PIPE=1 -DNN_HAVE_POLL=1 -DNN_HAVE_SEMAPHORE -DNN_HAVE_SEMAPHORE_PTHREAD=1 -DNN_HAVE_SOCKETPAIR=1 -DNN_HAVE_UNIX_SOCKETS=1 -DNN_MAX_SOCKETS=512 -DNN_STATIC_LIB -D_GNU_SOURCE -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -D_THREAD_SAFE -I../../../src/wolf.system -I../../../dependencies/luaJIT/include -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/nanomsg/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_xml.o ../../../src/wolf.system/w_xml.cpp

${OBJECTDIR}/_ext/26f1a4f1/wolf.o: ../../../src/wolf.system/wolf.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__linux -D_DEBUG -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D__LUA__ -D__WOLF_SYSTEM__ -DNN_HAVE_ACCEPT4=1 -DNN_HAVE_BACKTRACE=1 -DNN_HAVE_CLOCK_GETTIME=1 -DNN_HAVE_CLOCK_MONOTONIC=1 -DNN_HAVE_EPOLL=1 -DNN_HAVE_EVENTFD=1 -DNN_HAVE_GCC_ATOMIC_BUILTINS -DNN_HAVE_GETADDRINFO_A=1 -DNN_HAVE_LIBNSL=1 -DNN_HAVE_LINUX -DNN_HAVE_MSG_CONTROL=1 -DNN_HAVE_PIPE2=1 -DNN_HAVE_PIPE=1 -DNN_HAVE_POLL=1 -DNN_HAVE_SEMAPHORE -DNN_HAVE_SEMAPHORE_PTHREAD=1 -DNN_HAVE_SOCKETPAIR=1 -DNN_HAVE_UNIX_SOCKETS=1 -DNN_MAX_SOCKETS=512 -DNN_STATIC_LIB -D_GNU_SOURCE -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT -D_THREAD_SAFE -I../../../src/wolf.system -I../../../dependencies/luaJIT/include -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/nanomsg/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/wolf.o ../../../src/wolf.system/wolf.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
