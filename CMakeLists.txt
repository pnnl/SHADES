# See README.md for instruction on how to build SHAD with CMake
cmake_minimum_required(VERSION 3.8)

project(SHAD ${cmake_3_0_PROJ_VERSION} ${cmake_3_0_LANGUAGES} CXX)
set(CMAKE_CXX_STANDARD 14)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Debug")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (default Debug)" FORCE)
endif()


# No in-tree build allowed.
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR
    "In-source build are not allowed.
Please create a directory directory and run cmake from there, passing the path
to this source directory as the last argumente.
This process created the file `CMakeCache.txt' and the directory `CMakeFiles'.
Please delete them.")
endif()


string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)

if (CMAKE_BUILD_TYPE AND
    NOT uppercase_CMAKE_BUILD_TYPE MATCHES "^(RELEASE|DEBUG|RELWITHDEBINFO|MINSIZEREL)$")
  message(FATAL_ERROR "Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()



# Add path for custom CMake modules
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake") 
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH} "${SHAD_ROOT}/cmake")
set(SHAD_INCLUDE_DIRS "${SHAD_ROOT}/include")
set(ZMQ_PATH "/share/apps/zeromq/4.3.2/lib")

include_directories(include)
if(USE_GMT)
  list(APPEND backends "gmt")
#   temp
  include(shad_gmt_runtime_export)
  get_target_property(gmt_includes gmt_runtime INTERFACE_INCLUDE_DIRECTORIES)
  get_target_property(gmt_libs gmt_runtime INTERFACE_LINK_LIBRARIES)
  get_target_property(gmt_defs gmt_runtime INTERFACE_COMPILE_DEFINITIONS)
  list(APPEND RT ${gmt_libs})
  list(APPEND RT "${SHAD_ROOT}/lib/libgmt_runtime.a")
  list(APPEND SHAD_INCLUDE_DIRS ${gmt_includes})
  list(APPEND SHAD_INCLUDE_DIRS include)
message("--------> includes: ${SHAD_INCLUDE_DIRS}")
   add_executable(gmt_shad_server src/shad_server.cc)
   target_include_directories(gmt_shad_server PRIVATE ${SHAD_INCLUDE_DIRS})
   target_compile_definitions(gmt_shad_server PRIVATE ${gmt_defs})
   target_link_libraries(gmt_shad_server "${ZMQ_PATH}/libzmq.so" ${RT})
endif()
if(USE_TBB)
  list(APPEND backends "tbb")
  include(shad_tbb_runtime_export)
  get_target_property(tbb_includes tbb_runtime INTERFACE_INCLUDE_DIRECTORIES)
  get_target_property(tbb_libs tbb_runtime INTERFACE_LINK_LIBRARIES)
  get_target_property(tbb_defs tbb_runtime INTERFACE_COMPILE_DEFINITIONS)
  list(APPEND TBBRT ${tbb_libs})
  list(APPEND TBBRT "${SHAD_ROOT}/lib/libtbb_runtime.a")
  list(APPEND TBBSHAD_INCLUDE_DIRS ${tbb_includes})
  list(APPEND TBBSHAD_INCLUDE_DIRS include)
message("--------> includes: ${TBBSHAD_INCLUDE_DIRS}")
  add_executable(tbb_shad_server src/shad_server.cc)
  target_include_directories(tbb_shad_server PRIVATE ${TBBSHAD_INCLUDE_DIRS})
  target_compile_definitions(tbb_shad_server PRIVATE ${tbb_defs})
  target_link_libraries(tbb_shad_server "${ZMQ_PATH}/libzmq.so" ${TBBRT})
endif()
if(USE_CPP_SIMPLE)
  list(APPEND backends "cpp_simple")
  #   temp
  include(shad_cpp_simple_runtime_export)
  get_target_property(cpp_simple_includes cpp_simple_runtime INTERFACE_INCLUDE_DIRECTORIES)
  get_target_property(cpp_simple_libs cpp_simple_runtime INTERFACE_LINK_LIBRARIES)
  list(APPEND RT ${cpp_simple_libs})
  list(APPEND RT "${SHAD_ROOT}/lib/libcpp_simple_runtime.a")
  list(APPEND SHAD_INCLUDE_DIRS ${cpp_simple_includes})
  include_directories(${SHAD_INCLUDE_DIRS})
  add_executable(cpp_shad_server src/shad_server.cc)
  target_link_libraries(cpp_shad_server "${ZMQ_PATH}/libzmq.so" ${RT})
#   end
endif()

# include_directories(${SHAD_INCLUDE_DIRS})
# include_directories(include)

# add_executable(hello-shad src/hello-shad.cc)
#  target_link_libraries(hello-shad ${RT})
# 
# add_executable(async-hello-shad src/async-hello-shad.cc)
# target_link_libraries(async-hello-shad ${RT})
# 
# add_executable(tc src/reader.cc src/tc.cc)
# target_link_libraries(tc ${RT})
