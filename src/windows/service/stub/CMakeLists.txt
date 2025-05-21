set(SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/../inc/${TARGET_PLATFORM}/${CMAKE_BUILD_TYPE}/lswservice_i_${TARGET_PLATFORM}.c
    ${CMAKE_CURRENT_BINARY_DIR}/../inc/${TARGET_PLATFORM}/${CMAKE_BUILD_TYPE}/lswservice_p_${TARGET_PLATFORM}.c
    ${CMAKE_CURRENT_BINARY_DIR}/../inc/${TARGET_PLATFORM}/${CMAKE_BUILD_TYPE}/dlldata_${TARGET_PLATFORM}.c
    ${CMAKE_CURRENT_LIST_DIR}/WslServiceProxyStub.def
    ${CMAKE_CURRENT_LIST_DIR}/WslServiceProxyStub.rc)

set_source_files_properties(${SOURCES} PROPERTIES GENERATED TRUE)

add_library(lswserviceproxystub SHARED ${SOURCES})
add_dependencies(lswserviceproxystub lswserviceidl)
target_link_libraries(lswserviceproxystub ${COMMON_LINK_LIBRARIES})
set_target_properties(lswserviceproxystub PROPERTIES FOLDER windows)