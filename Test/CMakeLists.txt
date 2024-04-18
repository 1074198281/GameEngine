add_executable(AssetLoaderTest AssetLoaderTest.cpp)
target_link_libraries(AssetLoaderTest Framework)
add_test(NAME TEST_AssetLoader COMMAND AssetLoaderTest)

#add_executable(TextureLoadTest TextureLoadTest.cpp)
#target_link_libraries(TextureLoadTest Framework)
#add_test(NAME TEST_TextureLoad COMMAND TextureLoadTest)

#没有使用并行计算
#add_executable(GeomMathTest GeomMathTest.cpp)
#target_link_libraries(GeomMathTest GeomMath)
#add_test(NAME Test_GeomMath COMMAND GeomMathTest)

link_directories(${PROJECT_SOURCE_DIR}/External/Windows/lib)

add_executable(OgexParserTest OgexParserTest.cpp)
target_link_libraries(OgexParserTest Framework OpenGEX.lib OpenDDL.lib xg.lib)
#__add_xg_platform_dependencies(OgexParserTest)
add_test(NAME TEST_OgexParser COMMAND OgexParserTest)

add_executable(SceneObjectTest SceneObjectTest.cpp)
target_link_libraries(SceneObjectTest Framework xg.lib)
#__add_xg_platform_dependencies(SceneObjectTest)
add_test(NAME TEST_SceneObject COMMAND SceneObjectTest)

add_executable(PhysXTest PhysXTest.cpp)
#MESSAGE(STATUS "PhysX_DEBUG_LIBRARY: " ${PHYSX_DEBUG_LIBRARY} )
#MESSAGE(STATUS "PhysXCommon_DEBUG_LIBRARY: " ${PHYSXCOMMON_DEBUG_LIBRARY} )
#MESSAGE(STATUS "PhysXFoundation_DEBUG_LIBRARY: " ${PHYSXFOUNDATION_DEBUG_LIBRARY} )
target_link_libraries(PhysXTest ${PHYSX_DEBUG_LIBRARY} ${PHYSXCOMMON_DEBUG_LIBRARY} ${PHYSXFOUNDATION_DEBUG_LIBRARY} ${PHYSXPVDSDK_DEBUG_LIBRARY}  ${PHYSXEXTENSIONS_DEBUG_LIBRARY})
#__add_xg_platform_dependencies(PhysXTest)
add_test(NAME TEST_PhysXTest COMMAND PhysXTest)

add_executable(WAVTest WAVTest.cpp)
target_link_libraries(WAVTest Framework)
add_test(NAME TEST_WAV COMMAND WAVTest)
