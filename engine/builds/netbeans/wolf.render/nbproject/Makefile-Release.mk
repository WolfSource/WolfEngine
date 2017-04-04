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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/24207458/w_game.o \
	${OBJECTDIR}/_ext/de18f501/w_shader.o \
	${OBJECTDIR}/_ext/24207458/w_graphics_device_manager.o \
	${OBJECTDIR}/_ext/24207458/w_render_pch.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libwolf.render.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libwolf.render.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libwolf.render.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/_ext/24207458/w_game.o: ../../../src/wolf.render/w_game.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/24207458
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/24207458/w_game.o ../../../src/wolf.render/w_game.cpp

${OBJECTDIR}/_ext/de18f501/w_shader.o: ../../../src/wolf.render/w_graphics/w_shaders/w_shader.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/de18f501
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/de18f501/w_shader.o ../../../src/wolf.render/w_graphics/w_shaders/w_shader.cpp

${OBJECTDIR}/_ext/24207458/w_graphics_device_manager.o: ../../../src/wolf.render/w_graphics_device_manager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/24207458
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/24207458/w_graphics_device_manager.o ../../../src/wolf.render/w_graphics_device_manager.cpp

${OBJECTDIR}/_ext/24207458/w_render_pch.o: ../../../src/wolf.render/w_render_pch.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/24207458
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/24207458/w_render_pch.o ../../../src/wolf.render/w_render_pch.cpp

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
