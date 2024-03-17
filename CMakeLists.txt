cmake_minimum_required(VERSION 3.18)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

project(GameEngine VERSION 1.0 LANGUAGES CXX)

set(DEBUG_FLAG 1) 

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  # Debug 配置
  #添加可编辑并继续的程序数据库用于调试
  if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "-Od ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "/ZI")
    set(CMAKE_SHARED_LINKER_FLAGS "/SAFESEH:NO")
    set(CMAKE_EXE_LINKER_FLAGS "/SAFESEH:NO")
  endif()
  set(DEBUG_FLAG 1)
  MESSAGE("DEBUG_FLAG value change to 1")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  # Release 配置
  set(DEBUG_FLAG 0)
  MESSAGE("DEBUG_FLAG value change to 0")
endif ()


#if USE DX12
add_compile_definitions(USE_DIRECTX_TOOLKIT)

# 添加预处理器定义屏蔽vs的全局max和min宏
add_compile_definitions(NOMINMAX)

# 添加工作目录
set(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
MESSAGE("working directory: " ${PROJECT_SOURCE_DIR})
ADD_DEFINITIONS(-D_WORKING_DIRECTORY=\"${WORKING_DIRECTORY}\")
# 添加IBL资源目录
set(IBL_RESOURCE_DIRECTORY ${PROJECT_SOURCE_DIR}/Asset/IBLTextures)
MESSAGE("IBL Resource directory: " ${IBL_RESOURCE_DIRECTORY})
ADD_DEFINITIONS(-D_IBL_RESOURCE_DIRECTORY=\"${IBL_RESOURCE_DIRECTORY}\")

# 添加库
set(MYGE_TARGET_PLATFORM "Windows")
set(MYGE_EXTERNAL_ROOT ${PROJECT_SOURCE_DIR}/External/${MYGE_TARGET_PLATFORM}/)
include_directories("${MYGE_EXTERNAL_ROOT}include")
set(MYGE_EXTERNAL_LIBRARY_PATH ${MYGE_EXTERNAL_ROOT}lib/)

find_library(XG_LIBRARY xg PATHS ${MYGE_EXTERNAL_LIBRARY_PATH} NO_CMAKE_FIND_ROOT_PATH)
find_library(OPENDDL_LIBRARY OpenDDL PATHS ${MYGE_EXTERNAL_LIBRARY_PATH} NO_CMAKE_FIND_ROOT_PATH)
find_library(OPENGEX_LIBRARY OpenGEX PATHS ${MYGE_EXTERNAL_LIBRARY_PATH} NO_CMAKE_FIND_ROOT_PATH)
find_library(ZLIB_LIBRARY zlib PATHS ${MYGE_EXTERNAL_LIBRARY_PATH} NO_CMAKE_FIND_ROOT_PATH)
find_library(ZLIB_DEBUG_LIBRARY zlibd PATHS ${MYGE_EXTERNAL_LIBRARY_PATH} NO_CMAKE_FIND_ROOT_PATH)

MESSAGE(STATUS "(1 for debug and 0 for release) DEBUG_FLAG Value : " ${DEBUG_FLAG})

if(DEBUG_FLAG)
  #find bullet3
  MESSAGE("----------- find bullet3 -----------")
  find_library(BULLETCOLLISION_DEBUG_LIBRARY BulletCollision_Debug PATHS ${MYGE_EXTERNAL_LIBRARY_PATH}/bullet3/Debug NO_CMAKE_FIND_ROOT_PATH)
  find_library(BULLETDYNAMICS_DEBUG_LIBRARY BulletDynamics_Debug PATHS ${MYGE_EXTERNAL_LIBRARY_PATH}/bullet3/Debug NO_CMAKE_FIND_ROOT_PATH)
  find_library(BULLETLINEARMATH_DEBUG_LIBRARY LinearMath_Debug PATHS ${MYGE_EXTERNAL_LIBRARY_PATH}/bullet3/Debug NO_CMAKE_FIND_ROOT_PATH)
  MESSAGE(STATUS "BULLETCOLLISION_DEBUG_LIBRARY: " ${BULLETCOLLISION_DEBUG_LIBRARY} )
  MESSAGE(STATUS "BULLETDYNAMICS_DEBUG_LIBRARY: " ${BULLETDYNAMICS_DEBUG_LIBRARY} )
  MESSAGE(STATUS "BULLETLINEARMATH_DEBUG_LIBRARY: " ${BULLETLINEARMATH_DEBUG_LIBRARY} )
  MESSAGE("----------- find bullet3 end -----------")

  #find ImGUI
  MESSAGE("----------- find ImGUI -----------")
  find_library(IMGUI_DEBUG_LIBRARY ImGUI_Debug PATHS ${MYGE_EXTERNAL_LIBRARY_PATH}/ImGUI/Debug NO_CMAKE_FIND_ROOT_PATH)
  MESSAGE(STATUS "IMGUI_DEBUG_LIBRARY: " ${IMGUI_DEBUG_LIBRARY} )
  MESSAGE("----------- find ImGUI end -----------")
else()
  #find bullet3
  MESSAGE("----------- find bullet3 -----------")
  find_library(BULLETCOLLISION_RELEASE_LIBRARY BulletCollision PATHS ${MYGE_EXTERNAL_LIBRARY_PATH}/bullet3/Debug NO_CMAKE_FIND_ROOT_PATH)
  find_library(BULLETDYNAMICS_RELEASE_LIBRARY BulletDynamics PATHS ${MYGE_EXTERNAL_LIBRARY_PATH}/bullet3/Debug NO_CMAKE_FIND_ROOT_PATH)
  find_library(BULLETLINEARMATH_RELEASE_LIBRARY LinearMath PATHS ${MYGE_EXTERNAL_LIBRARY_PATH}/bullet3/Debug NO_CMAKE_FIND_ROOT_PATH)
  MESSAGE(STATUS "BULLETLINEARMATH_RELEASE_LIBRARY: " ${BULLETLINEARMATH_RELEASE_LIBRARY} )
  MESSAGE(STATUS "BULLETDYNAMICS_RELEASE_LIBRARY: " ${BULLETDYNAMICS_RELEASE_LIBRARY} )
  MESSAGE(STATUS "BULLETCOLLISION_RELEASE_LIBRARY: " ${BULLETCOLLISION_RELEASE_LIBRARY} )
  MESSAGE("----------- find bullet3 end -----------")

  #find ImGUI
  MESSAGE("----------- find ImGUI -----------")
  find_library(IMGUI_RELEASE_LIBRARY ImGUI_Release PATHS ${MYGE_EXTERNAL_LIBRARY_PATH}/ImGUI/Release NO_CMAKE_FIND_ROOT_PATH)
  MESSAGE(STATUS "IMGUI_RELEASE_LIBRARY: " ${IMGUI_RELEASE_LIBRARY} )
  MESSAGE("----------- find ImGUI end -----------")
endif()

# 设置附加库目录
link_directories(${MYGE_EXTERNAL_LIBRARY_PATH})

# third_parties
include_directories("${PROJECT_SOURCE_DIR}/External/Windows/include")
# Bullet
include_directories("${PROJECT_SOURCE_DIR}/External/Windows/include/bullet3")
# zlib
include_directories("${PROJECT_SOURCE_DIR}/External/Windows/include/zlib")
include_directories("${PROJECT_SOURCE_DIR}/External/Windows/include/WinPix")
# ImGUI
include_directories("${PROJECT_SOURCE_DIR}/External/Windows/include/ImGUI")

# Framework files
include_directories("${PROJECT_SOURCE_DIR}/Framework/Algorithm")
include_directories("${PROJECT_SOURCE_DIR}/Framework/Common")
include_directories("${PROJECT_SOURCE_DIR}/Framework/GeomMath")
include_directories("${PROJECT_SOURCE_DIR}/Framework/Interface")
include_directories("${PROJECT_SOURCE_DIR}/Framework/Manager")
include_directories("${PROJECT_SOURCE_DIR}/Framework/Math")
include_directories("${PROJECT_SOURCE_DIR}/Framework/Parser")
include_directories("${PROJECT_SOURCE_DIR}/Framework/SceneGraph")

# add windows includes
IF(${WIN32})
	include_directories("${PROJECT_SOURCE_DIR}/Platform/Windows")
ENDIF(${WIN32})

# add RHI includes
include_directories("${PROJECT_SOURCE_DIR}/RHI")


# -----------------------------------------------------
MESSAGE("---------- Starting RHI Make ----------")
add_subdirectory(RHI)
MESSAGE("---------- RHI Make End ----------")

MESSAGE("---------- Starting Framework Make ----------")
add_subdirectory(Framework)
MESSAGE("---------- Framework Make End ----------")

MESSAGE("---------- Starting Platform Make ----------")
add_subdirectory(Platform)
MESSAGE("---------- Platform Make End ----------")

MESSAGE("---------- Starting Viewer Make ----------")
add_subdirectory(Viewer)
MESSAGE("---------- Platform Viewer End ----------")

MESSAGE("---------- Starting Test Make ----------")
add_subdirectory(Test)
MESSAGE("---------- Test Make End ----------")