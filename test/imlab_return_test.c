#include "core.h"
#include <stdlib.h>


int main() {

    return_t aa = ERROR_NULL_TYPE;
    message(aa, "null type error is called %d", 1025);

    return_t bb = ERROR_TYPE_MISMATCH;
    message(bb, "this is a %s with %d and %3.2f", "test", 10, 11.02f);

    return_t cc = ERROR_DIMENSION_MISMATCH;
    message(cc, NULL);

    return_t dd = ERROR_OUT_OF_MEMORY;
    message(dd, NULL);

    return_t ee = ERROR_UNABLE_TO_OPEN;
    message(ee, NULL);

    return_t ef = ERROR;
    message(ef, NULL);

    return_t ff = SUCCESS;
    message(ff, NULL);

    return_t gg = WARNING_NOT_SUPPORTED;
    message(gg, NULL);

    return_t hh = WARNING_NOTHING_DONE;
    message(hh, NULL);

    return_t ii = WARNING;
    message(ii, NULL);

    message(SUCCESS, "all operations are done successfully!");

    return 0;
}
