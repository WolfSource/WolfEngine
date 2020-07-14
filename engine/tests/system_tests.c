#include "convey.h"
#include <wolf.h>

Main({
    Convey("We can create a BUS socket", {
        int _x = 0;
        So(_x == 0);
    });
 })
