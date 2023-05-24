cmake_minimum_required(VERSION 3.18)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

project(GameEngine)

#添加可编辑并继续的程序数据库用于调试
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS "-Od ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "/ZI")
  set(CMAKE_SHARED_LINKER_FLAGS "/SAFESEH:NO")
  set(CMAKE_EXE_LINKER_FLAGS "/SAFESEH:NO")
endif()

#添加预处理器定义屏蔽vs的全局max和min宏
add_compile_definitions(NOMINMAX)

set(MYGE_TARGET_PLATFORM "Windows")
set(MYGE_EXTERNAL_ROOT ${PROJECT_SOURCE_DIR}/External/${MYGE_TARGET_PLATFORM}/)
include_directories("${MYGE_EXTERNAL_ROOT}include")
set(MYGE_EXTERNAL_LIBRARY_PATH ${MYGE_EXTERNAL_ROOT}lib/)
find_library(XG_LIBRARY xg PATHS ${MYGE_EXTERNAL_LIBRARY_PATH} NO_CMAKE_FIND_ROOT_PATH)
find_library(OPENDDL_LIBRARY OpenDDL PATHS ${MYGE_EXTERNAL_LIBRARY_PATH} NO_CMAKE_FIND_ROOT_PATH)
find_library(OPENGEX_LIBRARY OpenGEX PATHS ${MYGE_EXTERNAL_LIBRARY_PATH} NO_CMAKE_FIND_ROOT_PATH)

include_directories("${PROJECT_SOURCE_DIR}/External/Windows/include")
include_directories("${PROJECT_SOURCE_DIR}/Framework/Common")
include_directories("${PROJECT_SOURCE_DIR}/Framework/Interface")
IF(${WIN32})
	include_directories("${PROJECT_SOURCE_DIR}/Platform/Windows")
ENDIF(${WIN32})
include_directories("${PROJECT_SOURCE_DIR}/RHI")

add_subdirectory(Framework)
add_subdirectory(Platform)
add_subdirectory(RHI)
add_subdirectory(Test)