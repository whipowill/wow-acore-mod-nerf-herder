CU_SET_PATH("CMAKE_MOD_NERFHERDER_DIR" "${CMAKE_CURRENT_LIST_DIR}")

AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/ModNerfHerder.cpp")

AC_ADD_SCRIPT_LOADER("NerfHerder" "${CMAKE_CURRENT_LIST_DIR}/src/ModNerfHerder.h")

AC_ADD_CONFIG_FILE("${CMAKE_CURRENT_LIST_DIR}/conf/NerfHerder.conf.dist")