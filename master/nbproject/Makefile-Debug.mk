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
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=
AS=as.exe

# Macros
CND_PLATFORM=Cygwin-Windows
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/master_server.o \
	${OBJECTDIR}/fs.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/fs_entry.o


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
LDLIBSOPTIONS=-lpthread ../commons/dist/Debug/GNU-Linux-x86/libcommons.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/Cygwin-Windows/master.exe

dist/Debug/Cygwin-Windows/master.exe: ../commons/dist/Debug/GNU-Linux-x86/libcommons.a

dist/Debug/Cygwin-Windows/master.exe: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/Cygwin-Windows
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/master ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/master_server.o: master_server.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_FILE_OFFSET_BITS=64 -MMD -MP -MF $@.d -o ${OBJECTDIR}/master_server.o master_server.cpp

${OBJECTDIR}/fs.o: fs.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_FILE_OFFSET_BITS=64 -MMD -MP -MF $@.d -o ${OBJECTDIR}/fs.o fs.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_FILE_OFFSET_BITS=64 -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/fs_entry.o: fs_entry.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_FILE_OFFSET_BITS=64 -MMD -MP -MF $@.d -o ${OBJECTDIR}/fs_entry.o fs_entry.cpp

# Subprojects
.build-subprojects:
	cd ../commons && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/Cygwin-Windows/master.exe

# Subprojects
.clean-subprojects:
	cd ../commons && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
