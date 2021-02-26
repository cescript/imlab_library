############## BUILD IMLAB LIBRARY #############
# remove build directory
rm -rf build
# create new buil directory
mkdir build
# change directory to build
cd build
# generate cache and run cmake
cmake .. && cmake --build . --target all
# is build successfull, or exit
[ $? -eq 0 ]  || exit 1

#################################################

############### INSTALL THE LIBRRAY ##############
# copy all the header and libs
make install

############## BUILD LIBRARY TEST FILES #########
# change to test directory
cd ../library/
# remove build directory
rm -rf build
# create new buil directory
mkdir build
# change directory to build
cd build
# generate cache
cmake ..
# run cmake command
cmake --build . --target all
#################################################

############### RUN ALL THE TESTS ###############
cd ../bins/
find . -type f -executable -not -path './results/*' | while read test
do
    $test
    if [ $? -ne 0 ]
    then
        exit 1
    fi
done
#################################################