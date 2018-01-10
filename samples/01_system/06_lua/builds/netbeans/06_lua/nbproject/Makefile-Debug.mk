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
	${OBJECTDIR}/_ext/52a4beb5/main.o \
	${OBJECTDIR}/_ext/52a4beb5/pch.o


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
LDLIBSOPTIONS=-L../../../../../../bin/x64/Debug/GNU-Linux -Wl,-rpath,'../../../../../../bin/x64/Debug/GNU-Linux' -lwolf.system

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/../../../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/06_lua

${CND_DISTDIR}/../../../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/06_lua: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/../../../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}
	clang++ -o ${CND_DISTDIR}/../../../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/06_lua ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/52a4beb5/main.o: ../../../src/main.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/52a4beb5
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -I../../../../../../engine/src/wolf.system -I../../../../../../engine/dependencies/luaJIT/include -I../../../../../../engine/dependencies/tbb/oss/linux/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/52a4beb5/main.o ../../../src/main.cpp

${OBJECTDIR}/_ext/52a4beb5/pch.o: ../../../src/pch.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/52a4beb5
	${RM} "$@.d"
	$(COMPILE.cc) -g -w -I../../../../../../engine/src/wolf.system -I../../../../../../engine/dependencies/luaJIT/include -I../../../../../../engine/dependencies/tbb/oss/linux/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/52a4beb5/pch.o ../../../src/pch.cpp

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
