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
	${OBJECTDIR}/_ext/a83248dc/w_media_core.o \
	${OBJECTDIR}/_ext/a83248dc/w_media_core_pch.o


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
LDLIBSOPTIONS=-L../../../dependencies/tbb/oss/linux/lib/intel64/gcc4.7

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/libwolf.media_core.${CND_DLIB_EXT}

${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/libwolf.media_core.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}
	clang++ -o ${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/libwolf.media_core.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/_ext/a83248dc/w_media_core.o: ../../../src/wolf.media_core/w_media_core.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/a83248dc
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__720p__ -D__WOLF_MEDIA_CORE__ -I../../../src/wolf.system -I../../../src/wolf.media_core -I../../../dependencies/tbb/oss/linux/include -I/usr/include/x86_64-linux-gnu -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a83248dc/w_media_core.o ../../../src/wolf.media_core/w_media_core.cpp

${OBJECTDIR}/_ext/a83248dc/w_media_core_pch.o: ../../../src/wolf.media_core/w_media_core_pch.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/a83248dc
	${RM} "$@.d"
	$(COMPILE.cc) -g -D__720p__ -D__WOLF_MEDIA_CORE__ -I../../../src/wolf.system -I../../../src/wolf.media_core -I../../../dependencies/tbb/oss/linux/include -I/usr/include/x86_64-linux-gnu -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a83248dc/w_media_core_pch.o ../../../src/wolf.media_core/w_media_core_pch.cpp

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
