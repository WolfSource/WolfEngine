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
CC=clang
CCC=clang++
CXX=clang++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=CLang-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/26f1a4f1/w_cpu.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_linear_allocator.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_logger.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_lua.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_memory.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_object.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_python.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_system_pch.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_task.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_tcp_client.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_time_span.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_window.o \
	${OBJECTDIR}/_ext/26f1a4f1/w_xml.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libwolf.system.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libwolf.system.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libwolf.system.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/_ext/26f1a4f1/w_cpu.o: ../../../src/wolf.system/w_cpu.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_cpu.o ../../../src/wolf.system/w_cpu.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_linear_allocator.o: ../../../src/wolf.system/w_linear_allocator.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_linear_allocator.o ../../../src/wolf.system/w_linear_allocator.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_logger.o: ../../../src/wolf.system/w_logger.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_logger.o ../../../src/wolf.system/w_logger.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_lua.o: ../../../src/wolf.system/w_lua.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_lua.o ../../../src/wolf.system/w_lua.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_memory.o: ../../../src/wolf.system/w_memory.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_memory.o ../../../src/wolf.system/w_memory.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_object.o: ../../../src/wolf.system/w_object.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_object.o ../../../src/wolf.system/w_object.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_python.o: ../../../src/wolf.system/w_python.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_python.o ../../../src/wolf.system/w_python.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_system_pch.o: ../../../src/wolf.system/w_system_pch.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_system_pch.o ../../../src/wolf.system/w_system_pch.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_task.o: ../../../src/wolf.system/w_task.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_task.o ../../../src/wolf.system/w_task.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_tcp_client.o: ../../../src/wolf.system/w_tcp_client.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_tcp_client.o ../../../src/wolf.system/w_tcp_client.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_time_span.o: ../../../src/wolf.system/w_time_span.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_time_span.o ../../../src/wolf.system/w_time_span.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_window.o: ../../../src/wolf.system/w_window.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_window.o ../../../src/wolf.system/w_window.cpp

${OBJECTDIR}/_ext/26f1a4f1/w_xml.o: ../../../src/wolf.system/w_xml.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/26f1a4f1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/26f1a4f1/w_xml.o ../../../src/wolf.system/w_xml.cpp

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
