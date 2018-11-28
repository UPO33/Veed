cmake_minimum_required(VERSION 3.8)

project(Veed)



#option(UCORE_BUILD_SHARED "dll or static" ON)
#option(UENGINE_BUILD_SHARED "dll or static?" ON)
#option(UGFXDIRECTX_BUILD_SHARED "" ON)
#option(UGFXVULKAN_BUILD_SHARED "" ON)

#set(PhysXSDKDir "D:/Lib/PhysX-3.3-master/PhysXSDK"  CACHE PATH "")
#set(QT5Dir "E:/Qt5/5.6.3/msvc2015_64"  CACHE PATH "")

#this add a definition to target as ${Target}_BUILD 
#macro(SetTargetExport Target)
#	set_target_properties(${Target} PROPERTIES COMPILE_FLAGS "-D${Target}_BUILD")
#endmacro()
#
#macro(AdjustTargetDebugWorkingDir Target)
#	set_target_properties(${Target} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/../${UPLATFORM_NAME}/Debug")
#endmacro()


#message(${CMAKE_BINARY_DIR})
#message(${CMAKE_SOURCE_DIR})

#if(UCORE_BUILD_SHARED)
#	#all projects will have UCORE_SHARED preprocessor
#	add_definitions(-DUCORE_SHARED)
#	set(UCORE_BUILD_TYPE SHARED)
#else()
#	set(UCORE_BUILD_TYPE STATIC)
#endif()



add_definitions(-DUNICODE -D_UNICODE)



#SET(CMAKE_BUILD_TYPE Debug CACHE STRING "default to debug" FORCE)

if(WIN32)
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(VPLATFORM_NAME "Win32")
	else()
		set(VPLATFORM_NAME "Win64")
	endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${VPLATFORM_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${VPLATFORM_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${VPLATFORM_NAME})


#wall == enable all warning

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++latest")



#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)

#However, the file(GLOB...) allows for wildcard additions:

file(GLOB_RECURSE SOURCES_Core "Src/Core/*.cpp")
file(GLOB_RECURSE HEADERS_Core "Src/Core/*.h")

file(GLOB_RECURSE SOURCES_CoreTest "Src/CoreTest/*.cpp")
file(GLOB_RECURSE HEADERS_CoreTest "Src/CoreTest/*.h")

file(GLOB_RECURSE SOURCES_NetTest "Src/NetTest/*.cpp")
file(GLOB_RECURSE HEADERS_NetTest "Src/NetTest/*.h")

file(GLOB_RECURSE HEADERS_ImGui "Src/ImGui/*.h")
file(GLOB_RECURSE SOURCES_ImGui "Src/ImGui/*.cpp")


add_library(VCore STATIC
${SOURCES_Core} 
${HEADERS_Core} )



add_executable(VCoreTest 
${SOURCES_CoreTest} 
${HEADERS_CoreTest}
)
target_link_libraries(VCoreTest VCore)
#AdjustTargetDebugWorkingDir(VCoreTest)

add_executable(VNetTest ${SOURCES_NetTest} ${HEADERS_NetTest} ${HEADERS_ImGui} ${SOURCES_ImGui})
target_link_libraries(VNetTest VCore)
