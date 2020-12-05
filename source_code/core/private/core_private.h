// common structutes and variables for the core functions
// here we define the opaque types and useful macros fro the source files
// these definitions will not be listed in the core.h header

#ifndef IMLAB_CORE_PRIVATE_H
#define IMLAB_CORE_PRIVATE_H

#include "../core.h"
#include "../core_macros.h"

// CHECK THAT THE GIVEN ARGUMENT IS TRUE AND RETURN ERROR IF IT IS FALSE
// check the given memory pointer is allocated correctly
//#define check_memory(ptr) if(ptr == NULL) { print_error("error in %s: cannot allocate enough memory!\n",__func__); return ERROR_OUT_OF_MEMORY;}
// check the given file is opened succesfully
// #define check_file(fid) if((fid) == NULL) { print_error("error in %s: cannot open the file!\n", __func__); return ERROR_UNABLE_TO_OPEN; }
// check the given condition and return error if it is not satisified
#define check_condition(cond, ret, ...) do { if(!(cond)) { message(ERROR, __VA_ARGS__); return ret; } } while(0);
//#define check_return(cond) do { if(is_error((cond))) { print_error("error in %s: %s", __func__, stringify(cond)); return cond; } } while(0);

// to check the return of the functions
#define is_error(c)   ((c) <  0)
#define is_success(c) ((c) == 0)
#define is_warning(c) ((c) >  0)
/*
#define print_error(...)       (IM_VERBOSE_ERROR       ? (printf("\033[31merror @%s: ", __func__),       printf(__VA_ARGS__), printf("\n\033[0m", __VA_ARGS__), fflush(stdout)) : 0) // print it red
#define print_warning(...)     (IM_VERBOSE_WARNING     ? (printf("\033[33mwarning on line %d: ", __LINE__),     printf(__VA_ARGS__), printf("\n\033[0m", __VA_ARGS__), fflush(stdout)) : 0)  // print it yellow
*/
#define check_image(...) call(check_image, __VA_ARGS__)
#define check_image_1(ptr) if( is_image(ptr) ? 0:message(ERROR_NOT_IMAGE, NULL) )
#define check_image_2(ptr, ret) if(!is_image(ptr)) { message(ERROR_NOT_IMAGE, NULL); return ret; }
#define check_image_3(ptr, ret, msg) if(!is_image(ptr)) { message(ERROR_NOT_IMAGE, msg); return ret; }

#define check_null(...) call(check_null, __VA_ARGS__)
#define check_null_1(ptr) if(ptr == NULL ? message(ERROR_NULL_TYPE, NULL) : 0)
#define check_null_2(ptr, ret) if(ptr == NULL) { message(ERROR_NULL_TYPE, NULL); return ret; }
#define check_null_3(ptr, ret, msg) if(ptr == NULL) { message(ERROR_NULL_TYPE, msg); return ret; }

#define check_memory(...) call(check_memory, __VA_ARGS__)
#define check_memory_1(ptr) if(ptr == NULL ? message(ERROR_OUT_OF_MEMORY, NULL) : 0)
#define check_memory_2(ptr, ret) if(ptr == NULL) { message(ERROR_OUT_OF_MEMORY, NULL); return ret; }
#define check_memory_3(ptr, ret, msg) if(ptr == NULL) { message(ERROR_OUT_OF_MEMORY, msg); return ret; }


#define check_file(...) call(check_file, __VA_ARGS__)
#define check_file_1(fid) if(fid == NULL ? message(ERROR_UNABLE_TO_OPEN, NULL) : 0)
#define check_file_2(fid, ret) if(fid == NULL) { message(ERROR_UNABLE_TO_OPEN, NULL); return ret; }
#define check_file_3(fid, ret, msg) if(fid == NULL) { message(ERROR_UNABLE_TO_OPEN, msg); return ret; }

#define check_return(...) call(check_return, __VA_ARGS__)
#define check_return_1(ret_val) if(ret_val < 0 ? message(ret_val, NULL) : 0)
#define check_return_2(ret_val, ret) if(ret_val < 0) { message(ret_val, NULL); return ret; }
#define check_return_3(ret_val, ret, msg) if(ret_val < 0) { message(ret_val, msg); return ret; }


/*
#define check_pointer(ptr) check_return(ptr == NULL ? ERROR_TYPE_MISMATCH:SUCCESS)
#define check_memory(ptr) check_return(ptr == NULL ? ERROR_OUT_OF_MEMORY:SUCCESS)
#define check_file(fid) check_return(fid == NULL ? ERROR_UNABLE_TO_OPEN :SUCCESS)
#define check_condition(cond, msg, rtn)  check_return(cond ? SUCCESS :ERROR)
return_t check_return_func(return_t cond, int line, const char *func);
 */

#define imlab_pi 3.14159265358979323846
#define imlab_epsilon 0.0000001


// define the imlab type here
struct imlab_type_t {
    uint8_t id;
    uint16_t elemsize;
    char *name;
    // default destructor for the type
    void (*destructor)(void*, uint32_t);
};

return_t imlab_type_write(struct imlab_type_t* in, FILE *out);
struct imlab_type_t* imlab_type_read(FILE *in);

// strdup implementation
char *imlab_strdup(const char *cstr);

void imlab_type_set_destructor(struct imlab_type_t *_type, void (*func)(void*, uint32_t));



#endif //IMLAB_CORE_PRIVATE_H