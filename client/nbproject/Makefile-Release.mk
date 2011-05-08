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
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/worker_pool.o \
	${OBJECTDIR}/job.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/yadfs_client.o \
	${OBJECTDIR}/worker.o


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
LDLIBSOPTIONS=-lfuse ../commons/dist/Debug/GNU-Linux-x86/libcommons.a -lpthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Release.mk dist/Release/GNU-Linux-x86/client

dist/Release/GNU-Linux-x86/client: ../commons/dist/Debug/GNU-Linux-x86/libcommons.a

dist/Release/GNU-Linux-x86/client: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/client ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/worker_pool.o: worker_pool.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -D_FILE_OFFSET_BITS=64 -MMD -MP -MF $@.d -o ${OBJECTDIR}/worker_pool.o worker_pool.cpp

${OBJECTDIR}/job.o: job.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -D_FILE_OFFSET_BITS=64 -MMD -MP -MF $@.d -o ${OBJECTDIR}/job.o job.cpp

${OBJECTDIR}/main.o: main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -D_FILE_OFFSET_BITS=64 -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/yadfs_client.o: yadfs_client.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -D_FILE_OFFSET_BITS=64 -MMD -MP -MF $@.d -o ${OBJECTDIR}/yadfs_client.o yadfs_client.cpp

${OBJECTDIR}/worker.o: worker.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -D_FILE_OFFSET_BITS=64 -MMD -MP -MF $@.d -o ${OBJECTDIR}/worker.o worker.cpp

# Subprojects
.build-subprojects:
	cd ../commons && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/GNU-Linux-x86/client

# Subprojects
.clean-subprojects:
	cd ../commons && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
