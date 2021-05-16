cmake_minimum_required(VERSION 3.5)
project(imlab_test C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set copiler flags for debug and release
set(CMAKE_C_FLAGS_DEBUG "-std=c99 -fgnu89-inline -pg -D_DEBUG -Wall -Wno-int-conversion -Wno-parentheses -Wno-unused-variable -Wno-unknown-pragmas -Wno-unused-result -Wno-format -Wno-format-extra-args")
set(CMAKE_C_FLAGS_RELEASE "-std=c99 -fgnu89-inline -O2")

# set build type to release
set(CMAKE_BUILD_TYPE Release)

set(IMLAB_INCLUDE_DIRECTORY ${PROJECT_SOURCE_DIR}/include/)
set(IMLAB_LIBRARY_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/)
set(IMLAB_TEST_BINARIES_DIRECTORY ${PROJECT_SOURCE_DIR}/bins/)

message(STATUS "INCLUDE_DIRECTORY: ${IMLAB_INCLUDE_DIRECTORY}")
message(STATUS "LIBRARY_DIRECTORY: ${IMLAB_LIBRARY_DIRECTORY}")

# add imlab path to the link directories
link_directories(${IMLAB_LIBRARY_DIRECTORY})

# enable library testing
enable_testing()

# get the all folders in the samples directory
set(test_sub_folder "test")

# get all .c files under each sub folder
file(GLOB test_files ${test_sub_folder}/*.c)

#for each c file in the samples directory
foreach(test_target ${test_files})

    #get the filename without the path
    get_filename_component(test_source_name ${test_target} NAME_WE)

    # print the status of the test file
    message(STATUS ${test_source_name})

    # create an executable_name via omitting the posix
    string(REPLACE ".c" "" executable_name ${test_source_name})
    add_executable(${executable_name} ${test_target})
    add_test(NAME ${executable_name} COMMAND ${executable_name})
    
    #we created the lib, now test it with samples
    target_include_directories(${executable_name} PUBLIC ${IMLAB_INCLUDE_DIRECTORY})

    # change the output dir of the target
    set_target_properties( ${executable_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${IMLAB_TEST_BINARIES_DIRECTORY} )

    # Make sure YourLib is linked to each app
    target_link_libraries(${executable_name} imlab)

    if(UNIX)
        target_link_libraries(${executable_name} m)
    endif(UNIX)
endforeach(test_target ${test_files})
