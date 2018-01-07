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
	${OBJECTDIR}/_ext/54df2aa1/JRTBoundingBox.o \
	${OBJECTDIR}/_ext/54df2aa1/JRTCamera.o \
	${OBJECTDIR}/_ext/54df2aa1/JRTCore.o \
	${OBJECTDIR}/_ext/54df2aa1/JRTCoreUtils.o \
	${OBJECTDIR}/_ext/54df2aa1/JRTH2KDTreeBuilder.o \
	${OBJECTDIR}/_ext/54df2aa1/JRTHeuristicKDTreeBuilder.o \
	${OBJECTDIR}/_ext/54df2aa1/JRTKDTree.o \
	${OBJECTDIR}/_ext/54df2aa1/JRTKDTreeBuilder.o \
	${OBJECTDIR}/_ext/54df2aa1/JRTMesh.o \
	${OBJECTDIR}/_ext/54df2aa1/JRTOrthoCamera.o \
	${OBJECTDIR}/_ext/54df2aa1/JRTPPMImage.o \
	${OBJECTDIR}/_ext/54df2aa1/JRTTriangleIntersection.o \
	${OBJECTDIR}/_ext/4707c333/JMLFuncs.o \
	${OBJECTDIR}/_ext/5654df24/TootleRaytracer.o \
	${OBJECTDIR}/_ext/592a70fc/Stripifier.o \
	${OBJECTDIR}/_ext/592a70fc/aligned_malloc.o \
	${OBJECTDIR}/_ext/592a70fc/clustering.o \
	${OBJECTDIR}/_ext/592a70fc/d3doverdrawwindow.o \
	${OBJECTDIR}/_ext/592a70fc/d3dwm.o \
	${OBJECTDIR}/_ext/592a70fc/feedback.o \
	${OBJECTDIR}/_ext/592a70fc/fit.o \
	${OBJECTDIR}/_ext/592a70fc/gdiwm.o \
	${OBJECTDIR}/_ext/592a70fc/heap.o \
	${OBJECTDIR}/_ext/592a70fc/overdraw.o \
	${OBJECTDIR}/_ext/592a70fc/soup.o \
	${OBJECTDIR}/_ext/592a70fc/souptomesh.o \
	${OBJECTDIR}/_ext/592a70fc/tootlelib.o \
	${OBJECTDIR}/_ext/592a70fc/triorder.o \
	${OBJECTDIR}/_ext/68cc1cc9/w_camera.o \
	${OBJECTDIR}/_ext/68cc1cc9/w_first_person_camera.o \
	${OBJECTDIR}/_ext/80a4cd0d/c_bone.o \
	${OBJECTDIR}/_ext/80a4cd0d/c_node.o \
	${OBJECTDIR}/_ext/80a4cd0d/c_parser.o \
	${OBJECTDIR}/_ext/cbdfc7ea/w_bounding.o \
	${OBJECTDIR}/_ext/cbdfc7ea/w_cpipeline_model.o \
	${OBJECTDIR}/_ext/cbdfc7ea/w_cpipeline_pch.o \
	${OBJECTDIR}/_ext/cbdfc7ea/w_cpipeline_scene.o


# C Compiler Flags
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../../dependencies/tbb/oss/linux/lib/intel64/gcc4.7/ -L../../../dependencies/msgpack/lib/linux/x64/debug -ltbb_debug -ltbbmalloc_debug -lmsgpackc

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/libwolfcontent_pipeline.${CND_DLIB_EXT}

${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/libwolfcontent_pipeline.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}
	clang++ -o ${CND_DISTDIR}/../../../../../bin/x64/${CND_CONF}/${CND_PLATFORM}/libwolfcontent_pipeline.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/_ext/54df2aa1/JRTBoundingBox.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTBoundingBox.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/54df2aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/54df2aa1/JRTBoundingBox.o ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTBoundingBox.cpp

${OBJECTDIR}/_ext/54df2aa1/JRTCamera.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTCamera.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/54df2aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/54df2aa1/JRTCamera.o ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTCamera.cpp

${OBJECTDIR}/_ext/54df2aa1/JRTCore.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTCore.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/54df2aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/54df2aa1/JRTCore.o ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTCore.cpp

${OBJECTDIR}/_ext/54df2aa1/JRTCoreUtils.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTCoreUtils.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/54df2aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/54df2aa1/JRTCoreUtils.o ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTCoreUtils.cpp

${OBJECTDIR}/_ext/54df2aa1/JRTH2KDTreeBuilder.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTH2KDTreeBuilder.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/54df2aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/54df2aa1/JRTH2KDTreeBuilder.o ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTH2KDTreeBuilder.cpp

${OBJECTDIR}/_ext/54df2aa1/JRTHeuristicKDTreeBuilder.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTHeuristicKDTreeBuilder.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/54df2aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/54df2aa1/JRTHeuristicKDTreeBuilder.o ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTHeuristicKDTreeBuilder.cpp

${OBJECTDIR}/_ext/54df2aa1/JRTKDTree.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTKDTree.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/54df2aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/54df2aa1/JRTKDTree.o ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTKDTree.cpp

${OBJECTDIR}/_ext/54df2aa1/JRTKDTreeBuilder.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTKDTreeBuilder.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/54df2aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/54df2aa1/JRTKDTreeBuilder.o ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTKDTreeBuilder.cpp

${OBJECTDIR}/_ext/54df2aa1/JRTMesh.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTMesh.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/54df2aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/54df2aa1/JRTMesh.o ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTMesh.cpp

${OBJECTDIR}/_ext/54df2aa1/JRTOrthoCamera.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTOrthoCamera.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/54df2aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/54df2aa1/JRTOrthoCamera.o ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTOrthoCamera.cpp

${OBJECTDIR}/_ext/54df2aa1/JRTPPMImage.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTPPMImage.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/54df2aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/54df2aa1/JRTPPMImage.o ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTPPMImage.cpp

${OBJECTDIR}/_ext/54df2aa1/JRTTriangleIntersection.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTTriangleIntersection.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/54df2aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/54df2aa1/JRTTriangleIntersection.o ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/JRT/JRTTriangleIntersection.cpp

${OBJECTDIR}/_ext/4707c333/JMLFuncs.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/Math/JMLFuncs.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/4707c333
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/4707c333/JMLFuncs.o ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/Math/JMLFuncs.cpp

${OBJECTDIR}/_ext/5654df24/TootleRaytracer.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/TootleRaytracer.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/5654df24
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5654df24/TootleRaytracer.o ../../../src/wolf.content_pipeline/amd/amd_tootle/RayTracer/TootleRaytracer.cpp

${OBJECTDIR}/_ext/592a70fc/Stripifier.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/Stripifier.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/592a70fc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/592a70fc/Stripifier.o ../../../src/wolf.content_pipeline/amd/amd_tootle/Stripifier.cpp

${OBJECTDIR}/_ext/592a70fc/aligned_malloc.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/aligned_malloc.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/592a70fc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/592a70fc/aligned_malloc.o ../../../src/wolf.content_pipeline/amd/amd_tootle/aligned_malloc.cpp

${OBJECTDIR}/_ext/592a70fc/clustering.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/clustering.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/592a70fc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/592a70fc/clustering.o ../../../src/wolf.content_pipeline/amd/amd_tootle/clustering.cpp

${OBJECTDIR}/_ext/592a70fc/d3doverdrawwindow.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/d3doverdrawwindow.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/592a70fc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/592a70fc/d3doverdrawwindow.o ../../../src/wolf.content_pipeline/amd/amd_tootle/d3doverdrawwindow.cpp

${OBJECTDIR}/_ext/592a70fc/d3dwm.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/d3dwm.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/592a70fc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/592a70fc/d3dwm.o ../../../src/wolf.content_pipeline/amd/amd_tootle/d3dwm.cpp

${OBJECTDIR}/_ext/592a70fc/feedback.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/feedback.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/592a70fc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/592a70fc/feedback.o ../../../src/wolf.content_pipeline/amd/amd_tootle/feedback.cpp

${OBJECTDIR}/_ext/592a70fc/fit.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/fit.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/592a70fc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/592a70fc/fit.o ../../../src/wolf.content_pipeline/amd/amd_tootle/fit.cpp

${OBJECTDIR}/_ext/592a70fc/gdiwm.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/gdiwm.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/592a70fc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/592a70fc/gdiwm.o ../../../src/wolf.content_pipeline/amd/amd_tootle/gdiwm.cpp

${OBJECTDIR}/_ext/592a70fc/heap.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/heap.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/592a70fc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/592a70fc/heap.o ../../../src/wolf.content_pipeline/amd/amd_tootle/heap.cpp

${OBJECTDIR}/_ext/592a70fc/overdraw.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/overdraw.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/592a70fc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/592a70fc/overdraw.o ../../../src/wolf.content_pipeline/amd/amd_tootle/overdraw.cpp

${OBJECTDIR}/_ext/592a70fc/soup.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/soup.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/592a70fc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/592a70fc/soup.o ../../../src/wolf.content_pipeline/amd/amd_tootle/soup.cpp

${OBJECTDIR}/_ext/592a70fc/souptomesh.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/souptomesh.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/592a70fc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/592a70fc/souptomesh.o ../../../src/wolf.content_pipeline/amd/amd_tootle/souptomesh.cpp

${OBJECTDIR}/_ext/592a70fc/tootlelib.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/tootlelib.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/592a70fc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/592a70fc/tootlelib.o ../../../src/wolf.content_pipeline/amd/amd_tootle/tootlelib.cpp

${OBJECTDIR}/_ext/592a70fc/triorder.o: ../../../src/wolf.content_pipeline/amd/amd_tootle/triorder.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/592a70fc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/592a70fc/triorder.o ../../../src/wolf.content_pipeline/amd/amd_tootle/triorder.cpp

${OBJECTDIR}/_ext/68cc1cc9/w_camera.o: ../../../src/wolf.content_pipeline/cameras/w_camera.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/68cc1cc9
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/68cc1cc9/w_camera.o ../../../src/wolf.content_pipeline/cameras/w_camera.cpp

${OBJECTDIR}/_ext/68cc1cc9/w_first_person_camera.o: ../../../src/wolf.content_pipeline/cameras/w_first_person_camera.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/68cc1cc9
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/68cc1cc9/w_first_person_camera.o ../../../src/wolf.content_pipeline/cameras/w_first_person_camera.cpp

${OBJECTDIR}/_ext/80a4cd0d/c_bone.o: ../../../src/wolf.content_pipeline/collada/c_bone.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/80a4cd0d
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/80a4cd0d/c_bone.o ../../../src/wolf.content_pipeline/collada/c_bone.cpp

${OBJECTDIR}/_ext/80a4cd0d/c_node.o: ../../../src/wolf.content_pipeline/collada/c_node.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/80a4cd0d
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/80a4cd0d/c_node.o ../../../src/wolf.content_pipeline/collada/c_node.cpp

${OBJECTDIR}/_ext/80a4cd0d/c_parser.o: ../../../src/wolf.content_pipeline/collada/c_parser.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/80a4cd0d
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/80a4cd0d/c_parser.o ../../../src/wolf.content_pipeline/collada/c_parser.cpp

${OBJECTDIR}/_ext/cbdfc7ea/w_bounding.o: ../../../src/wolf.content_pipeline/w_bounding.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/cbdfc7ea
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/cbdfc7ea/w_bounding.o ../../../src/wolf.content_pipeline/w_bounding.cpp

${OBJECTDIR}/_ext/cbdfc7ea/w_cpipeline_model.o: ../../../src/wolf.content_pipeline/w_cpipeline_model.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/cbdfc7ea
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/cbdfc7ea/w_cpipeline_model.o ../../../src/wolf.content_pipeline/w_cpipeline_model.cpp

${OBJECTDIR}/_ext/cbdfc7ea/w_cpipeline_pch.o: ../../../src/wolf.content_pipeline/w_cpipeline_pch.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/cbdfc7ea
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/cbdfc7ea/w_cpipeline_pch.o ../../../src/wolf.content_pipeline/w_cpipeline_pch.cpp

${OBJECTDIR}/_ext/cbdfc7ea/w_cpipeline_scene.o: ../../../src/wolf.content_pipeline/w_cpipeline_scene.cpp nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}/_ext/cbdfc7ea
	${RM} "$@.d"
	$(COMPILE.cc) -g -DGLM_FORCE_DEPTH_ZERO_TO_ONE -D_DEBUG -D__WOLF_CONTENT_PIPELINE__ -I../../../src/wolf.system -I../../../src/wolf.content_pipeline -I../../../dependencies/tbb/oss/linux/include -I../../../dependencies/msgpack/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/cbdfc7ea/w_cpipeline_scene.o ../../../src/wolf.content_pipeline/w_cpipeline_scene.cpp

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
