#add Shaders source

MESSAGE("HLSL PROPERTIES CONFIG")
MESSAGE("CMAKE CURRENT DIRECTORY : " ${CMAKE_CURRENT_SOURCE_DIR})

#add extra hlsl project
#cmake_minimum_required(VERSION 3.18)
#set(CMAKE_C_STANDARD 11)
#set(CMAKE_CXX_STANDARD 20)

#project(Shader_Compiler VERSION 1.0 LANGUAGES CXX)

file(GLOB ALGORITHM_FILES
${CMAKE_CURRENT_SOURCE_DIR}/Algorithm/*.hlsl)

file(GLOB CUBE_FILES
${CMAKE_CURRENT_SOURCE_DIR}/Cube/*.hlsl)

file(GLOB TOYCAR_FILES
${CMAKE_CURRENT_SOURCE_DIR}/ToyCar/*.hlsl)

file(GLOB ABEAUTIFULGAME_FILES
${CMAKE_CURRENT_SOURCE_DIR}/ABeautifulGame/*.hlsl)

file(GLOB ENVIRONMENTTEST_FILES
${CMAKE_CURRENT_SOURCE_DIR}/EnvironmentTest/*.hlsl)

file(GLOB SKYBOX_FILES
${CMAKE_CURRENT_SOURCE_DIR}/SkyBox/*.hlsl)

file(GLOB PRESENT_FILES
${CMAKE_CURRENT_SOURCE_DIR}/Present/*.hlsl)

file(GLOB MAIN_FILE
${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

source_group(Algorithm FILES ${ALGORITHM_FILES})
source_group(Cube FILES ${CUBE_FILES})
source_group(ToyCar FILES ${TOYCAR_FILES})
source_group(ABeautifulGame FILES ${ABEAUTIFULGAME_FILES})
source_group(EnvironmentTest FILES ${ENVIRONMENTTEST_FILES})
source_group(SkyBox FILES ${SKYBOX_FILES})
source_group(Present FILES ${PRESENT_FILES})

set(SHADER_FILES 
${ALGORITHM_FILES}
${CUBE_FILES}
${TOYCAR_FILES}
${ABEAUTIFULGAME_FILES}
${ENVIRONMENTTEST_FILES}
${SKYBOX_FILES}
${PRESENT_FILES}
)

# 设置 HLSL 文件的属性
foreach(PER_SHADER_FILE ${SHADER_FILES})
    get_filename_component(FILE_NAME ${PER_SHADER_FILE} NAME)
    
    # 判断文件名是否以 "_VS" 结尾
    if(FILE_NAME MATCHES "_VS.hlsl$")
        set_source_files_properties(${PER_SHADER_FILE}
            PROPERTIES
            VS_SHADER_DISABLE_OPTIMIZATIONS "true"
            VS_SHADER_ENABLE_DEBUG "true"
            VS_SHADER_ENTRYPOINT "main"
            VS_SHADER_TYPE "Vertex"
            VS_SHADER_MODEL "5.1"
            VS_SHADER_VARIABLE_NAME "g_p%(Filename)"
            VS_SHADER_OUTPUT_HEADER_FILE "$(SolutionDir)../Asset/Shaders/CompiledShaders/g_%(Filename).h"
            VS_SHADER_OBJECT_FILE_NAME ""
        )
    # 判断文件名是否以 "_PS" 结尾
    elseif(FILE_NAME MATCHES "_PS.hlsl$")
        set_source_files_properties(${PER_SHADER_FILE}
            PROPERTIES
            VS_SHADER_DISABLE_OPTIMIZATIONS "true"
            VS_SHADER_ENABLE_DEBUG "true"
            VS_SHADER_ENTRYPOINT "main"
            VS_SHADER_TYPE "Pixel"
            VS_SHADER_MODEL "5.1"
            VS_SHADER_VARIABLE_NAME "g_p%(Filename)"
            VS_SHADER_OUTPUT_HEADER_FILE "$(SolutionDir)../Asset/Shaders/CompiledShaders/g_%(Filename).h"
            VS_SHADER_OBJECT_FILE_NAME ""
        )
    elseif(FILE_NAME MATCHES "_CS.hlsl$")
        set_source_files_properties(${PER_SHADER_FILE}
            PROPERTIES
            VS_SHADER_DISABLE_OPTIMIZATIONS "true"
            VS_SHADER_ENABLE_DEBUG "true"
            VS_SHADER_ENTRYPOINT "main"
            VS_SHADER_TYPE "Compute"
            VS_SHADER_MODEL "5.1"
            VS_SHADER_VARIABLE_NAME "g_p%(Filename)"
            VS_SHADER_OUTPUT_HEADER_FILE "$(SolutionDir)../Asset/Shaders/CompiledShaders/g_%(Filename).h"
            VS_SHADER_OBJECT_FILE_NAME ""
        )
    endif()
endforeach()


add_executable(SHADER_COMPILER 
${MAIN_FILE}
${SHADER_FILES}
)
