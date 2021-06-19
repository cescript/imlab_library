# IMLAB Image Processing Library [![Build Status](https://github.com/cescript/imlab_library/actions/workflows/cmake.yml/badge.svg)](https://github.com/cescript/imlab_library/actions/workflows/cmake.yml)

IMLAB is an easy to use computer vision library written purely in C language.
It is designed to be tiny, simple and brief yet self contained. No addtitional libraries is needed. Most of the fundemental operations on images, vectors, matrice and files are written pureley in C as header files which can be used sepeartely depending on your project specifics.
Basically, IMLAB defines just a few very well known structures in order to keep the library readable and work comfortable. It has 7 core project all under the imlab and all of them can be included or excluded from the main projet depending on your task.
Although the library is written in C, it is designed to be look as object oriented manner so it is easy to read and it is easy to remember neccesarry functions for each operation. Each struct (will menteioned as class later) has its own operations and all operation belong to a Class is started with a class_name prefix. This type of access makes it possiple to use auto complete features in the editors and helps to find the realted functions
easily.

IMLAB defines the following structure:
- vector_t is a vector container just like standart std vector in C++. vector_t structure is a generic c-like array that can hold any type of variable in dynamically allocated memory.
Just like the arrays, vector data is contigous in memory and can be accessed externally.
Using a vector object is easy. Just decide the data type you need and call #vector_create to create a new vector object.
The data type can be any C data types, additional names declared in stdint.h or structs created by the user.
```C
     vector_t *out = vector_create(uint32_t);
```
The above code will create a vector object and set the initial length to zero and capacity to one.
In order to add/remove element to vector #vector_push/#vector_pop can be used.
```C
     vector_t *out = vector_create(float);
     float values[12] = {0.0 0.1, 0.3, 0.5, 0.7, 0.9, 0.8, 0.6, 0.5, 0.4, 0.2, 0.0};
     // in this loop the length of the vector will automatically increase up to 12.
     for(size_t i = 0; i < 12; i++) {
      vector_push(out, &values[i]);
     }
```
We can get the values from the vector with various methods. In the simplest case if we know the data type of the vector, we can obtain the data pointer and access the values via pointer just like standart c arrays.
```C
     float *vector_data = data(float, out);
     for(size_t i = 0; i < length(out); i++) {
        float val = vector_data[i];
     }
```
- matrix_t structure is a generic c-like array that can hold any type of variable in dynamically allocated memory. Just like the arrays and vectors, matrix data is contigous in memory and can be accessed externally. Similar to vector objects, matrix object is also resizable but you cannot increase the size of the matrix by pushing elements.

The main advantage of the matrix object onto the standart arrays is that the matrix objects can hold two dimesional data in a simple way. Using matrix object the user inetarctions with the 2D array becomes easier and the code would be understandable.

Similar to the vector_t structure matrix_t structure can be created via matrix_create with any C data types, additional names declared in stdint.h or structs created by the user.
```C
     matrix_t *out = matrix_create(float);
```
The above code will create a matrix object and set the initial rows,cols and channels to zero.
If you want to specify size to matrix just call:
```C
     matrix_t *out = matrix_create(float, 1024, 1024, 1);
```
This call of the #matrix_create will create a floating point data pointer and allocate 1024*1024*1*sizeof(float) byte memory on the memory. In order to access an element of a matrix #at or #data functions could be used.
```C
     for(size_t i = 0; i < rows(out); i++) {
        for(size_t j = 0; j < cols(out); j++) {
            float val = at(float, out, i,j);
        }
     }
```

## How To Build
All the public library and header files are distributed under the [library](library) folder. This files are compiled with [GNU Compiler Collection](https://gcc.gnu.org) in [Alpine Linux](https://alpinelinux.org).

If these files are not suitable for your project, you can rebuild the library using the following steps: 

1. Download the library source files: `git clone https://github.com/cescript/imlab_library.git`
2. Change directory to source folder: `cd imlab_library`
3. Create a temporary build directory and step into: `mkdir build && cd build`
4. Configure the build process with default options: `cmake ..`
5. Build the library: `make -j8`
6. Update the files under [library](library) folders: `make install`
7. All done, you can use [library](library) folder to link imlab to your project
8. You can find some example projects in [imlab.io](https://imlab.io)