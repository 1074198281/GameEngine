cmake_minimum_required(VERSION 3.18)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

project(GameEngine VERSION 1.0 LANGUAGES CXX)

#添加可编辑并继续的程序数据库用于调试
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS "-Od ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "/ZI")
  set(CMAKE_SHARED_LINKER_FLAGS "/SAFESEH:NO")
  set(CMAKE_EXE_LINKER_FLAGS "/SAFESEH:NO")
endif()

#添加预处理器定义屏蔽vs的全局max和min宏
add_compile_definitions(NOMINMAX)

#添加工作目录
set(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ADD_DEFINITIONS(-D_WORKING_DIRECTORY=\"${WORKING_DIRECTORY}\")

#添加库
set(MYGE_TARGET_PLATFORM "Windows")
set(MYGE_EXTERNAL_ROOT ${PROJECT_SOURCE_DIR}/External/${MYGE_TARGET_PLATFORM}/)
include_directories("${MYGE_EXTERNAL_ROOT}include")
set(MYGE_EXTERNAL_LIBRARY_PATH ${MYGE_EXTERNAL_ROOT}lib/)
find_library(XG_LIBRARY xg PATHS ${MYGE_EXTERNAL_LIBRARY_PATH} NO_CMAKE_FIND_ROOT_PATH)
find_library(OPENDDL_LIBRARY OpenDDL PATHS ${MYGE_EXTERNAL_LIBRARY_PATH} NO_CMAKE_FIND_ROOT_PATH)
find_library(OPENGEX_LIBRARY OpenGEX PATHS ${MYGE_EXTERNAL_LIBRARY_PATH} NO_CMAKE_FIND_ROOT_PATH)
find_library(ZLIB_LIBRARY zlib PATHS ${MYGE_EXTERNAL_LIBRARY_PATH} NO_CMAKE_FIND_ROOT_PATH)
find_library(ZLIB_DEBUG_LIBRARY zlibd PATHS ${MYGE_EXTERNAL_LIBRARY_PATH} NO_CMAKE_FIND_ROOT_PATH)

#设置附加库目录
link_directories(${MYGE_EXTERNAL_LIBRARY_PATH})

include_directories("${PROJECT_SOURCE_DIR}/External/Windows/include")
include_directories("${PROJECT_SOURCE_DIR}/External/Windows/include/zlib")
include_directories("${PROJECT_SOURCE_DIR}/External/Windows/include/WinPix")
include_directories("${PROJECT_SOURCE_DIR}/Framework/Common")
include_directories("${PROJECT_SOURCE_DIR}/Framework/Interface")
include_directories("${PROJECT_SOURCE_DIR}/Framework/GeomMath")
IF(${WIN32})
	include_directories("${PROJECT_SOURCE_DIR}/Platform/Windows")
ENDIF(${WIN32})
include_directories("${PROJECT_SOURCE_DIR}/RHI")

#add shader files 还没搞明白shader如何通过shader添加编译
#暂时不将这些加入目录，直接将shader编译为.h文件，用于头文件

file(GLOB ASSET_SHADERS
${PROJECT_SOURCE_DIR}/Asset/Shaders/hlsl/*.hlsl)
add_subdirectory(${PROJECT_SOURCE_DIR}/Asset/Shaders/hlsl)



#add_subdirectory(${PROJECT_SOURCE_DIR}/Asset/Shaders/hlsl)
add_subdirectory(RHI)
add_subdirectory(Framework)
add_subdirectory(Platform)
add_subdirectory(Test)