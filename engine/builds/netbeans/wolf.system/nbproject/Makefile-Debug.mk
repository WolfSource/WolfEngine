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
	${OBJECTDIR}/_ext/26f1a4f1/dllmain.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_cpu.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_linear_allocator.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_logger.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_lua.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_memory.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_python.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_system_pch.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_task.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_tcp_client.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_time_span.o


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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/libwolf.system.${CND_DLIB_EXT}

${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/libwolf.system.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/libwolf.system.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/_ext/26f1a4f1/dllmain.o: ../../../src/wolf.system/dllmain.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../dependencies/glm -I../../../dependencies/rapidxml -I../../../dependencies/rapidjson -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/dllmain.o ../../../src/wolf.system/dllmain.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_cpu.o: ../../../src/wolf.system/w_cpu.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../dependencies/glm -I../../../dependencies/rapidxml -I../../../dependencies/rapidjson -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_cpu.o ../../../src/wolf.system/w_cpu.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_linear_allocator.o: ../../../src/wolf.system/w_linear_allocator.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../dependencies/glm -I../../../dependencies/rapidxml -I../../../dependencies/rapidjson -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_linear_allocator.o ../../../src/wolf.system/w_linear_allocator.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_logger.o: ../../../src/wolf.system/w_logger.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../dependencies/glm -I../../../dependencies/rapidxml -I../../../dependencies/rapidjson -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_logger.o ../../../src/wolf.system/w_logger.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_lua.o: ../../../src/wolf.system/w_lua.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../dependencies/glm -I../../../dependencies/rapidxml -I../../../dependencies/rapidjson -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_lua.o ../../../src/wolf.system/w_lua.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_memory.o: ../../../src/wolf.system/w_memory.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../dependencies/glm -I../../../dependencies/rapidxml -I../../../dependencies/rapidjson -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_memory.o ../../../src/wolf.system/w_memory.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_python.o: ../../../src/wolf.system/w_python.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../dependencies/glm -I../../../dependencies/rapidxml -I../../../dependencies/rapidjson -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_python.o ../../../src/wolf.system/w_python.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_system_pch.o: ../../../src/wolf.system/w_system_pch.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../dependencies/glm -I../../../dependencies/rapidxml -I../../../dependencies/rapidjson -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_system_pch.o ../../../src/wolf.system/w_system_pch.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_task.o: ../../../src/wolf.system/w_task.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../dependencies/glm -I../../../dependencies/rapidxml -I../../../dependencies/rapidjson -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_task.o ../../../src/wolf.system/w_task.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_tcp_client.o: ../../../src/wolf.system/w_tcp_client.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../dependencies/glm -I../../../dependencies/rapidxml -I../../../dependencies/rapidjson -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_tcp_client.o ../../../src/wolf.system/w_tcp_client.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_time_span.o: ../../../src/wolf.system/w_time_span.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../dependencies/glm -I../../../dependencies/rapidxml -I../../../dependencies/rapidjson -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_time_span.o ../../../src/wolf.system/w_time_span.cpp

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
