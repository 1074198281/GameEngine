add_executable(AssetLoaderTest AssetLoaderTest.cpp)
target_link_libraries(AssetLoaderTest Common)
add_test(NAME TEST_AssetLoader COMMAND AssetLoaderTest)

#add_executable(TextureLoadTest TextureLoadTest.cpp)
#target_link_libraries(TextureLoadTest Common)
#add_test(NAME TEST_TextureLoad COMMAND TextureLoadTest)

#没有使用并行计算
#add_executable(GeomMathTest GeomMathTest.cpp)
#target_link_libraries(GeomMathTest GeomMath)
#add_test(NAME Test_GeomMath COMMAND GeomMathTest)

link_directories(${PROJECT_SOURCE_DIR}/External/Windows/lib)

add_executable(OgexParserTest OgexParserTest.cpp)
target_link_libraries(OgexParserTest Common OpenGEX.lib OpenDDL.lib xg.lib)
#__add_xg_platform_dependencies(OgexParserTest)
add_test(NAME TEST_OgexParser COMMAND OgexParserTest)

add_executable(SceneObjectTest SceneObjectTest.cpp)
target_link_libraries(SceneObjectTest Common xg.lib)
#__add_xg_platform_dependencies(SceneObjectTest)
add_test(NAME TEST_SceneObject COMMAND SceneObjectTest)