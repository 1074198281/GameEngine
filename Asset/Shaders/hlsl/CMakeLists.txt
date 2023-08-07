#add Shaders source

#MESSAGE("HLSL PROPERTIES CONFIG")

#find_program(HLSL_COMPILER_PATH fxc.exe PATHS "C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64")
#if(NOT HLSL_COMPILER_PATH)
#    message(FATAL_ERROR "Could not find HLSL compiler (fxc.exe)")
#endif()
#set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/mainVS.hlsl PROPERTIES VS_SHADER_TYPE PixelShader)
#set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/mainVS.hlsl PROPERTIES VS_SHADER_MODEL 4.0)
#set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/mainVS.hlsl PROPERTIES VS_SHADER_ENTRYPOINT main)

#编译头文件
#set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/mainVS.hlsl PROPERTIES VS_SHADER_OUTPUT_HEADER_FILE_DIRECTORY "${PROJECT_SOURCE_DIR}/Asset/Shaders/CompiledShaders")

#不需要二进制文件，直接使用头文件
#set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/mainVS.hlsl PROPERTIES VS_SHADER_OUTPUT_OBJECT_FILE_DIRECTORY "${PROJECT_SOURCE_DIR}/Asset/Shaders/CompiledShaders")
#set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/mainVS.hlsl PROPERTIES VS_SHADER_VARIABLE_NAME "g_mainVS")
#set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/mainVS.hlsl PROPERTIES VS_SHADER_DISABLE_OPTIMIZATIONS ON)
#set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/mainVS.hlsl PROPERTIES VS_SHADER_ENABLE_DEBUG ON)



#add extra hlsl project
cmake_minimum_required(VERSION 3.18)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

project(Shader_Compiler VERSION 1.0 LANGUAGES CXX)

file(GLOB SHADER_COMPILER
${CMAKE_CURRENT_SOURCE_DIR}/*.hlsl)

set(SOURCE_FILE ${SHADER_COMPILER})

set(MAIN main.cpp)

add_executable(COMPILER ${MAIN} ${SOURCE_FILE})
