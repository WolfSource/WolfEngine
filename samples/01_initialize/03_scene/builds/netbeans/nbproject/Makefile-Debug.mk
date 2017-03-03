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
	${OBJECTDIR}/_ext/56252444/main.o \
	${OBJECTDIR}/_ext/56252444/pch.o \
	${OBJECTDIR}/_ext/56252444/scene.o


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
LDLIBSOPTIONS=-L../../../../../bin/x64/Debug/GNU-Linux -L../../../../../engine/dependencies/tbb/tbb2017_20161128oss/linux/lib/intel64/gcc4.7 -L../../../../../engine/dependencies/vulkan/linux/lib -lwolf.system -ltbb_debug -ltbbmalloc_debug -lwolf.vulkan -lvulkan

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/../../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/03_scene

${CND_DISTDIR}/../../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/03_scene: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/../../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/../../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/03_scene ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/56252444/main.o: ../../src/main.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../../../engine/src/wolf.system -I../../../../../engine/dependencies/tbb/tbb2017_20161128oss/linux/include -I../../../../../engine/src/wolf.render/wolf.vulkan -I../../../../../engine/dependencies/vulkan/linux/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/main.o ../../src/main.cpp

${OBJECTDIR}/_ext/56252444/pch.o: ../../src/pch.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../../../engine/src/wolf.system -I../../../../../engine/dependencies/tbb/tbb2017_20161128oss/linux/include -I../../../../../engine/src/wolf.render/wolf.vulkan -I../../../../../engine/dependencies/vulkan/linux/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/pch.o ../../src/pch.cpp

${OBJECTDIR}/_ext/56252444/scene.o: ../../src/scene.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56252444
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../../../engine/src/wolf.system -I../../../../../engine/dependencies/tbb/tbb2017_20161128oss/linux/include -I../../../../../engine/src/wolf.render/wolf.vulkan -I../../../../../engine/dependencies/vulkan/linux/include -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56252444/scene.o ../../src/scene.cpp

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
