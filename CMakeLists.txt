cmake_minimum_required(VERSION 3.5)
project(imlab C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set copiler flags for debug and release
set(COMPILER_SPECIFIC_DEFINITIONS_DEBUG "")
set(COMPILER_SPECIFIC_DEFINITIONS_RELEASE "")

# add OS definitions
if (WIN32)
    add_definitions(-D_IMLAB_PLATFORM_WINDOWS)
elseif (UNIX)
    add_definitions(-D_IMLAB_PLATFORM_UNIX)
endif (WIN32)

# add COMPILER specific flags
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    # using visual studio
    message(STATUS "Found compiler Visual Studio")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(COMPILER_SPECIFIC_DEFINITIONS_DEBUG ${COMPILER_SPECIFIC_DEFINITIONS_DEBUG} "/Od /TC /Zc:preprocessor")
    set(COMPILER_SPECIFIC_DEFINITIONS_RELEASE ${COMPILER_SPECIFIC_DEFINITIONS_RELEASE} "/O2 /TC /Zc:preprocessor")
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    # using GCC
    message(STATUS "Found compiler GCC")
    set(COMPILER_SPECIFIC_DEFINITIONS_DEBUG ${COMPILER_SPECIFIC_DEFINITIONS_DEBUG} "-std=c99 -fgnu89-inline -pg -D_DEBUG -Wall -Wno-int-conversion -Wno-parentheses -Wno-unused-variable -Wno-unknown-pragmas -Wno-unused-result -Wno-format -Wno-format-extra-args")
    set(COMPILER_SPECIFIC_DEFINITIONS_RELEASE ${COMPILER_SPECIFIC_DEFINITIONS_RELEASE} "-std=c99 -O2 -fgnu89-inline -Wno-unused-result")
else ()
    message(FATAL "Unsupported compiler")
endif()

set(CMAKE_C_FLAGS_DEBUG ${COMPILER_SPECIFIC_DEFINITIONS_DEBUG})
set(CMAKE_C_FLAGS_RELEASE ${COMPILER_SPECIFIC_DEFINITIONS_RELEASE})

# set the include directory
set(IMLAB_DIRECTORY ${PROJECT_SOURCE_DIR}/library/)
set(IMLAB_INCLUDE_DIRECTORY ${IMLAB_DIRECTORY}/include/)
set(IMLAB_LIBRARY_DIRECTORY ${IMLAB_DIRECTORY}/lib/)
set(IMLAB_TEST_BINARIES_DIRECTORY ${PROJECT_SOURCE_DIR}/bins/)

# set build type to release
set(CMAKE_BUILD_TYPE Release)

# find the source header of the library
set(inc_core   "source_code/core/")
set(inc_alcore "source_code/alcore/")
set(inc_cvcore "source_code/cvcore/")
set(inc_imcore "source_code/imcore/")
set(inc_iocore "source_code/iocore/")
set(inc_lacore "source_code/lacore/")
set(inc_mlcore "source_code/mlcore/")
set(inc_prcore "source_code/prcore/")

# concat private headers
set(HEADER_INCLUDES ${inc_core} ${inc_alcore} ${inc_cvcore} ${inc_imcore} ${inc_iocore} ${inc_lacore} ${inc_mlcore} ${inc_prcore})

# concat public headers
file(GLOB header_main   "source_code/*.h")
file(GLOB header_core   "${inc_core}/*.h")
file(GLOB header_alcore "${inc_alcore}/*.h")
file(GLOB header_cvcore "${inc_cvcore}/*.h")
file(GLOB header_imcore "${inc_imcore}/*.h")
file(GLOB header_iocore "${inc_iocore}/*.h")
file(GLOB header_lacore "${inc_lacore}/*.h")
file(GLOB header_mlcore "${inc_mlcore}/*.h")
file(GLOB header_prcore "${inc_prcore}/*.h")

set(IMLAB_PUBLIC_HEADER_FILES ${header_main};${header_core};${header_alcore};${header_cvcore};${header_imcore};${header_iocore};${header_lacore};${header_mlcore};${header_prcore})

# find the source files of the library
file(GLOB src_core   "source_code/core/sources/*.c")
file(GLOB src_alcore "source_code/alcore/sources/*.c")
file(GLOB src_cvcore "source_code/cvcore/sources/*.c")
file(GLOB src_imcore "source_code/imcore/sources/*.c")
file(GLOB src_iocore "source_code/iocore/sources/*.c")
file(GLOB src_lacore "source_code/lacore/sources/*.c")
file(GLOB src_mlcore "source_code/mlcore/sources/*.c")
file(GLOB src_prcore "source_code/prcore/sources/*.c")

# set the source files and header files for the imlab library
set(IMLAB_SOURCE_FILES ${src_core} ${src_alcore} ${src_cvcore} ${src_imcore} ${src_iocore} ${src_lacore} ${src_mlcore} ${src_prcore})
# message(STATUS ${IMLAB_SOURCE_FILES})

# create imlab library
add_library(imlab STATIC ${IMLAB_SOURCE_FILES})

# add all the private headers into the include directory of the project
include_directories(${HEADER_INCLUDES})

# set the source files and header files for the imlab library
message(STATUS ${IMLAB_PUBLIC_HEADER_FILES})

# set target properties of the library
set_target_properties(imlab PROPERTIES LINKER_LANGUAGE C)
set_target_properties(imlab PROPERTIES PUBLIC_HEADER "${IMLAB_PUBLIC_HEADER_FILES}")

# install the imlab library
INSTALL(TARGETS imlab 
    ARCHIVE DESTINATION ${IMLAB_LIBRARY_DIRECTORY}
    PUBLIC_HEADER DESTINATION ${IMLAB_INCLUDE_DIRECTORY}
)

# get all .c files under each sub folder
file(GLOB test_files "test/*.c")

#for each c file in the samples directory
foreach(test_target ${test_files})

    #get the filename without the path
    get_filename_component(test_source_name ${test_target} NAME_WE)

    # print the status of the test file
    message(STATUS ${test_source_name})

    # create an executable_name via omitting the posix
    string(REPLACE ".c" "" executable_name ${test_source_name})
    add_executable(${executable_name} ${test_target})

    # change the output dir of the target
    set_target_properties( ${executable_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${IMLAB_TEST_BINARIES_DIRECTORY} )

    # Make sure YourLib is linked to each app
    target_link_libraries(${executable_name} imlab)

    if(UNIX)
        target_link_libraries(${executable_name} m)
    endif(UNIX)
endforeach(test_target ${test_files})