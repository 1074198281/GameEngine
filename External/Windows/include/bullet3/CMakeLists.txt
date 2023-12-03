

cmake_minimum_required(VERSION 2.4.3)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
cmake_policy(SET CMP0017 NEW)
#this line has to appear before 'PROJECT' in order to be able to disable incremental linking
SET(MSVC_INCREMENTAL_DEFAULT ON)

add_compile_definitions(BT_USE_DOUBLE_PRECISION)

PROJECT(MY_BULLET_PHYSICS)
FILE (STRINGS "VERSION" BULLET_VERSION)

#设置库文件的输出目录
SET(CMAKE_DEBUG_POSTFIX "_Debug")
SET(CMAKE_RELEASE_POSTFIX "_Release") 
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG  ${PROJECT_SOURCE_DIR}/../../lib/bullet3/Debug)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE  ${PROJECT_SOURCE_DIR}/../../lib/bullet3/Release)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Bullet3Collision)
add_subdirectory(Bullet3Common)
add_subdirectory(Bullet3Dynamics)
add_subdirectory(Bullet3Geometry)
add_subdirectory(Bullet3OpenCL)
add_subdirectory(Bullet3Serialize/Bullet2FileLoader)
add_subdirectory(BulletCollision)
add_subdirectory(BulletDynamics)
add_subdirectory(BulletInverseDynamics)
add_subdirectory(BulletSoftBody)
add_subdirectory(clew)
add_subdirectory(LinearMath)
