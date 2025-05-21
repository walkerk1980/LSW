set(SOURCES
    ServiceMain.cpp
    WslInstallerFactory.cpp
    WslInstaller.cpp
    main.rc)

set (HEADERS
     WslInstallerFactory.h
     WslInstaller.h)

include_directories(${CMAKE_BINARY_DIR}/src/windows/lswinstaller/inc/${TARGET_PLATFORM}/${CMAKE_BUILD_TYPE})

add_executable(lswinstaller ${SOURCES} ${HEADERS})
add_dependencies(lswinstaller lswinstalleridl)

set_target_properties(lswinstaller PROPERTIES LINK_FLAGS "/merge:minATL=.rdata /include:__minATLObjMap_WslInstaller_COM")
target_link_libraries(lswinstaller
                      ${COMMON_LINK_LIBRARIES}
                      common
                      legacy_stdio_definitions)

target_precompile_headers(lswinstaller REUSE_FROM common)
set_target_properties(lswinstaller PROPERTIES FOLDER windows)