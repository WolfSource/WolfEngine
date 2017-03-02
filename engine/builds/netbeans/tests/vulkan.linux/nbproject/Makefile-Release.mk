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
	${OBJECTDIR}/_ext/46a988ea/main.o \
	${OBJECTDIR}/_ext/46a988ea/pch.o \
	${OBJECTDIR}/_ext/46a988ea/scene.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vulkan.linux

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vulkan.linux: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vulkan.linux ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/46a988ea/main.o: ../../../../src/tests/Vulkan.Linux/main.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/46a988ea
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/46a988ea/main.o ../../../../src/tests/Vulkan.Linux/main.cpp

${OBJECTDIR}/_ext/46a988ea/pch.o: ../../../../src/tests/Vulkan.Linux/pch.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/46a988ea
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/46a988ea/pch.o ../../../../src/tests/Vulkan.Linux/pch.cpp

${OBJECTDIR}/_ext/46a988ea/scene.o: ../../../../src/tests/Vulkan.Linux/scene.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/46a988ea
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/46a988ea/scene.o ../../../../src/tests/Vulkan.Linux/scene.cpp

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
