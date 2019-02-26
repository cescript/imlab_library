///@file core_macros.h
//
// Created by cescript on 11.02.2018.
//
#ifndef IMLAB_CORE_MACROS_H
#define IMLAB_CORE_MACROS_H

/**@hideinitializer
 * Swap the given two variable using the third temporary variable
 \code{.c}
 double a = 1.1, b = 2.2;
 swap(double, a,b);   // a : 2.2, b : 1.1
 \endcode
 */
#define swap(_type, a,b) do { _type _imlab_tmp = (a); (a) = (b); (b) = _imlab_tmp; } while(0);

/**@hideinitializer
 * Clamp the given variable x, between the lower and higher limits
 \code{.c}
 double a = 1.4, b = 1.6, c = b-a;
 clamp(a, 0.5,1.5); // a : 1.4 (unchanged)
 clamp(b, 0.5,1.5); // b : 1.5 (clamp to higher limit)
 clamp(c, 0.5, 1.5) // c : 0.5 (clamp to lower limit)
 \endcode
 */
#define clamp(x, lower, higher) (((x) < (lower)) ? (lower) : (((x) > (higher)) ? (higher) : (x)))

/**@hideinitializer
 * Compare the given variable a and b, and return true if they are in eps neighbourhood
 \code{.c}
 double a = 1.4, b = 1.6;
 equal(a, b, 0.1); // false
 equal(a, b, 0.3); // true
 \endcode
 */
#define equal(a, b, eps) ((((a)-(b)) < eps) && (((b)-(a)) < eps))
//#define round(val) ((int)( val + (val < 0 ? -0.5:0.5)))
//#define floor(val) ((int)(val))

/**@hideinitializer
 * Return the positive closest multiple of m less than a
 \code{.c}
 roundup(16,5); // returns 15
 roundup(14,5); // returns 10
 roundup(19,5); // returns 15
 \endcode
 */
#define roundup(a,m)  (((int)((a)/(m)))*(m))

/**@hideinitializer
 * Return the remedy of a/m division
 \code{.c}
 remedy(16,5); // returns 1
 remedy(14,5); // returns 4
 remedy(14.1,5); // returns 4.1
 \endcode
 */
#define remedy(a,m)   ( (a) - roundup(a,m)) // return the remedy of a/m division

/**@hideinitializer
 * Map input value which in [_imin, _imax] range to value between [_omin, _omax]
 \code{.c}
 double angle = 90;
 map(angle, -180,180, -pi,pi); // returns pi/4
 \endcode
 */
#define map(_in, _imin, _imax, _omin, _omax) ( ((_omax) - (_omin))*((_in)-(_imin)) / (((_imax) - (_imin))) + (_omin) )

/**@hideinitializer
 * Convert degrees to radians
 */
#define deg2rad(angle) ( (angle)*0.01745329251f ) // (2.0 * imlab_pi) / 360.0)
/**@hideinitializer
 * Convert radians to degrees
 */
#define rad2deg(angle) ( (angle)*57.2957795131f ) // 360.0 / (2.0 * imlab_pi))

/**@hideinitializer
 * Compute the square of the input parameter
 */
#define square(x) ((x)*(x))

/**@hideinitializer
 * Stringify the given parameter or macro expression
 */
#define stringify(txt) xstringify(txt)
#ifndef DOXYGEN_SKIP_IMLAB_HIDDEN_MACRO_NAMES
#define xstringify(txt) #txt
#endif

/**@hideinitializer
 * Counts the number of arguments in variable length argument list. Argument count must be between 1-9.
 \code{.c}
 #define fun(...) printf("fun called with %d arguments\n", va_nargs(__VA_ARGS__));
 fun(1.2345);  // "fun called with 1 arguments"
 fun(1,2);     // "fun called with 2 arguments"
 fun(a,b,c,d); // "fun called with 4 arguments"
 \endcode
 */
#define va_nargs(...) va_nargs_(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#ifndef DOXYGEN_SKIP_IMLAB_HIDDEN_MACRO_NAMES
// simple argument counter for variable argument functions
#define va_nargs_(_1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N
#endif

//
/**@hideinitializer
 * Call the given function_(#number of arguments) with the given arguments
 */
#define call(func, ...) call_(func, va_nargs(__VA_ARGS__))(__VA_ARGS__)
#ifndef DOXYGEN_SKIP_IMLAB_HIDDEN_MACRO_NAMES
#define call_(func, nargs) cat_3(func, _, nargs)
#endif

/**@hideinitializer
 * Get the argument _i in a variable length argument list (up to 9 arguments)
 */
#define arg(_i, ...) cat_2(arg_, _i)(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0)
#ifndef DOXYGEN_SKIP_IMLAB_HIDDEN_MACRO_NAMES
// to get the argument at a specific position
#define arg_1(N, ...) N
#define arg_2(_1, N, ...) N
#define arg_3(_1, _2, N, ...) N
#define arg_4(_1, _2, _3, N, ...) N
#define arg_5(_1, _2, _3, _4, N, ...) N
#define arg_6(_1, _2, _3, _4, _5, N, ...) N
#define arg_7(_1, _2, _3, _4, _5, _6, N, ...) N
#define arg_8(_1, _2, _3, _4, _5, _6, _7, N, ...) N
#define arg_9(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#endif

/**@hideinitializer
 * Concanate the given arguments (up to 9 arguments)
 \code{.c}
 cat(1.2345);  // 1.2345
 cat(1,2);     // 12
 cat(a,b,c,d); // abcd
 \endcode
 */
#define cat(...) call(cat, __VA_ARGS__)
#ifndef DOXYGEN_SKIP_IMLAB_HIDDEN_MACRO_NAMES
// concanate the given different number of arguments
#define cat_1(a) a
#define cat_2(a, b) a ## b
#define cat_3(a, b, c) a ## b ## c
#define cat_4(a, b, c, d) a ## b ## c ## d
#define cat_5(a, b, c, d, e) a ## b ## c ## d ## e
#define cat_6(a, b, c, d, e, f) a ## b ## c ## d ## e ## f
#define cat_7(a, b, c, d, e, f, g) a ## b ## c ## d ## e ## f ## g
#define cat_8(a, b, c, d, e, f, g, h) a ## b ## c ## d ## e ## f ## g ## h
#define cat_9(a, b, c, d, e, f, g, h, i) a ## b ## c ## d ## e ## f ## g ## h ## i
#endif

/* MIN-MAX MACRO */
/**@hideinitializer
 * Max macro for different number of arguments (up to 9 arguments)
 \code{.c}
 max(1,2);     // 2
 max(min(7,8),11,12); // 12
 \endcode
 */
#define max(...) call(max, __VA_ARGS__)
/**@hideinitializer
 * Min macro for different number of arguments (up to 9 arguments)
 \code{.c}
 min(1.2345);  // 1.2345
 min(a,b,c,d); // compares the values in (a,b,c,d) and return the minimum
 \endcode
 */
#define min(...) call(min, __VA_ARGS__)

#ifndef DOXYGEN_SKIP_IMLAB_HIDDEN_MACRO_NAMES
// implementation of max/min macro for different number of argumnets
#define max_1(a) (a)
#define min_1(a) (a)
#define max_2(a,b) (((a) > (b)) ? (a):(b))
#define min_2(a,b) (((a) < (b)) ? (a):(b))
#define max_3(a,b,c) (max_2((a), max_2((b),(c))))
#define min_3(a,b,c) (min_2((a), min_2((b),(c))))
#define max_4(a,b,c,d) (max_2(max_2((a),(b)), max_2((c),(d))))
#define min_4(a,b,c,d) (min_2(min_2((a),(b)), min_2((c),(d))))
#define max_5(a,b,c,d,e) ( max_2(max_2(a,b), max_3(c,d,e)) )
#define min_5(a,b,c,d,e) ( min_2(min_2(a,b), min_3(c,d,e)) )
#define max_6(a,b,c,d,e,f) ( max_2(max_3(a,b,c), max_3(d,e,f)) )
#define min_6(a,b,c,d,e,f) ( min_2(min_3(a,b,c), min_3(d,e,f)) )
#define max_7(a,b,c,d,e,f,g) ( max_2(max_3(a,b,c), max_4(d,e,f,g)) )
#define min_7(a,b,c,d,e,f,g) ( min_2(min_3(a,b,c), min_4(d,e,f,g)) )
#define max_8(a,b,c,d,e,f,g,h) ( max_2(max_4(a,b,c,d), max_4(e,f,g,h)) )
#define min_8(a,b,c,d,e,f,g,h) ( min_2(min_4(a,b,c,d), min_4(e,f,g,h)) )
#define max_9(a,b,c,d,e,f,g,h,i) ( max_3(max_3(a,b,c), max_3(d,e,f), max_3(g,h,i)) )
#define min_9(a,b,c,d,e,f,g,h,i) ( min_3(min_3(a,b,c), min_3(d,e,f), min_3(g,h,i)) )
#endif

#define in_range(_var, _min, _max) ((_var) >= (_min) && (_var) <= (_max))



#endif //IMLAB_CORE_MACROS_H
