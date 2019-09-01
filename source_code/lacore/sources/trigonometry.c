#include "private/lacore_private.h"
#include <math.h>

inline float fast_atan2f(float x, float y) {

    if(y == 0) { return x == 0 ? 0:90; }
    float z = x/y;
    // if the |z| > 1 return 90-atan(1/z)
    if(z > 1 || z < -1) { return 90-fast_atan2f(y,x);   }
    return z*(45-(fabs(z)-1)*15.66);
}