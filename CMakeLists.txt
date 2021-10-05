cmake_minimum_required(VERSION 3.5)
project(imlab_library C)

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
    # see bug about Zc:preprocessor and wd5105: https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/c5105?view=msvc-160
    set(COMPILER_SPECIFIC_DEFINITIONS_DEBUG ${COMPILER_SPECIFIC_DEFINITIONS_DEBUG} "/Od /TC")
    set(COMPILER_SPECIFIC_DEFINITIONS_RELEASE ${COMPILER_SPECIFIC_DEFINITIONS_RELEASE} "/O2 /TC")
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

# set build type to release
set(CMAKE_BUILD_TYPE Release)

# set the core libraries
set(IMLAB_FOLDERS "core;alcore;cvcore;imcore;iocore;lacore;mlcore;prcore")

# clear variables and fill them
set(IMLAB_HEADER_DIRECTORIES "")
set(IMLAB_PUBLIC_HEADER_FILES "")
set(IMLAB_SOURCE_FILES "")
set(IMLAB_TEST_FILES, "")

#for each c file in the samples directory
foreach(imlab_folder ${IMLAB_FOLDERS})

    # create the public header directory and add it to the HEADER_INCLUDES list
    set(flib_directory   "source_code/${imlab_folder}")
    set(IMLAB_HEADER_DIRECTORIES "${IMLAB_HEADER_DIRECTORIES};${flib_directory}")

    # find the public headers in directory and add it to the IMLAB_PUBLIC_HEADER_FILES list
    file(GLOB fpublic_headers   "${flib_directory}/*.h")
    set(IMLAB_PUBLIC_HEADER_FILES "${IMLAB_PUBLIC_HEADER_FILES};${fpublic_headers}")

    # find and add the source files
    file(GLOB fsource_files   "${flib_directory}/sources/*.c")
    set(IMLAB_SOURCE_FILES "${IMLAB_SOURCE_FILES};${fsource_files}")

    # find and add the test files
    file(GLOB ftest_files   "${flib_directory}/tests/*.c")
    set(IMLAB_TEST_FILES "${IMLAB_TEST_FILES};${ftest_files}")

endforeach()

# print the files and folders for debug
# message(STATUS ${IMLAB_HEADER_DIRECTORIES})
# message(STATUS ${IMLAB_PUBLIC_HEADER_FILES})
# message(STATUS ${IMLAB_SOURCE_FILES})
# message(STATUS ${IMLAB_TEST_FILES})

# create imlab library
add_library(imlab STATIC ${IMLAB_SOURCE_FILES})

# add the include directories
target_include_directories(imlab PRIVATE ${IMLAB_HEADER_DIRECTORIES})

# set target properties of the library
set_target_properties(imlab PROPERTIES LINKER_LANGUAGE C)
set_target_properties(imlab PROPERTIES PUBLIC_HEADER "${IMLAB_PUBLIC_HEADER_FILES}")

# install the imlab library
install(TARGETS imlab 
    ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/library/lib/
    PUBLIC_HEADER DESTINATION ${PROJECT_SOURCE_DIR}/library/include/
)

# test files using the output library
enable_testing()

#for each c file in the samples directory
foreach(test_target ${IMLAB_TEST_FILES})

    #get the filename without the path
    get_filename_component(test_source_name ${test_target} NAME_WE)

    # print the status of the test file
    message(STATUS ${test_source_name})

    # create an executable_name via omitting the posix
    string(REPLACE ".c" "" executable_name ${test_source_name})
    add_executable(${executable_name} ${test_target})
    add_test(NAME ${executable_name} COMMAND ${executable_name})
    
    #we created the lib, now test it with samples
    target_include_directories(${executable_name} PRIVATE ${IMLAB_HEADER_DIRECTORIES})

    # change the output dir of the target
    set_target_properties( ${executable_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bins/ )

    # Make sure YourLib is linked to each app
    target_link_libraries(${executable_name} imlab)

    if(UNIX)
        target_link_libraries(${executable_name} m)
    endif(UNIX)
endforeach(test_target ${IMLAB_TEST_FILES})