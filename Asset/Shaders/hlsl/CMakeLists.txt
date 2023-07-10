#add Shaders source

#find_program(HLSL_COMPILER_PATH fxc.exe PATHS "C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64")
#if(NOT HLSL_COMPILER_PATH)
#    message(FATAL_ERROR "Could not find HLSL compiler (fxc.exe)")
#endif()
#set_source_files_properties(mainVS.hlsl PROPERTIES VS_SHADER_TYPE PixelShader)
#set_source_files_properties(mainVS.hlsl PROPERTIES VS_SHADER_MODEL 4.0)
#set_source_files_properties(mainVS.hlsl PROPERTIES VS_SHADER_ENTRYPOINT main)
#编译头文件
#set_source_files_properties(mainVS.hlsl PROPERTIES VS_SHADER_OUTPUT_HEADER_FILE_DIRECTORY "${PROJECT_SOURCE_DIR}/Asset/Shaders/CompiledShaders")

#不需要二进制文件，直接使用头文件
#set_source_files_properties(mainVS.hlsl PROPERTIES VS_SHADER_OUTPUT_OBJECT_FILE_DIRECTORY "${PROJECT_SOURCE_DIR}/Asset/Shaders/CompiledShaders")

#set_source_files_properties(mainVS.hlsl PROPERTIES VS_SHADER_VARIABLE_NAME "g_mainVS")
#set_source_files_properties(mainVS.hlsl PROPERTIES VS_SHADER_DISABLE_OPTIMIZATIONS ON)
#set_source_files_properties(mainVS.hlsl PROPERTIES VS_SHADER_ENABLE_DEBUG ON)
