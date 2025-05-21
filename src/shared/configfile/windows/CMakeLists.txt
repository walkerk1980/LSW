add_library(configfile STATIC ${SOURCES} ${HEADERS})
add_dependencies(configfile lswserviceidl localization common)
set_target_properties(configfile PROPERTIES FOLDER common)