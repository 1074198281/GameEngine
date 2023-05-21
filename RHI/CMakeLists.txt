# Empty
add_subdirectory(Empty)
IF(${WIN32})
    # Windows specific code
    add_subdirectory(D3d)
    add_subdirectory(D2d)
ENDIF(${WIN32})