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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES=


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
LDLIBSOPTIONS=-L../../../../../bin/x64/Debug/GNU-Linux -L../../../../../../../VulkanSDK/1.0.42.0/x86_64/lib -Wl,-rpath,'../../../../../bin/x64/Debug/GNU-Linux' -lwolf.system -lwolf.render

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/../../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/vulkan.linux

${CND_DISTDIR}/../../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/vulkan.linux: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/../../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/../../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/vulkan.linux ${OBJECTFILES} ${LDLIBSOPTIONS}

# Subprojects
.build-subprojects:
	cd ../../wolf.system && ${MAKE}  -f Makefile CONF=Debug
	cd ../../wolf.render && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:
	cd ../../wolf.system && ${MAKE}  -f Makefile CONF=Debug clean
	cd ../../wolf.render && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
