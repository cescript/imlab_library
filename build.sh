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



################# RUN LOCAL TEST ################
# no test for now

#################################################



############### INSTALL THE LIBRRAY ##############
# copy all the header and libs
make install
# copy all the test sources
cp -R ../test ../library/


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