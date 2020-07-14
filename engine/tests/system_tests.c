#include "convey.h"
#include <wolf.h>
#include <chrono/w_timespan.h>

// SIMPLE EXAMPLE
//int main()
//{
//	wolf_initialize();
//	w_timespan* _t_days = w_timespan_init_from_days(1);
//	wolf_terminate();
//	return 0;
//}

Main({
    //initialize wolf
    wolf_initialize();

    //testing chrono
    printf("testing chrono");
    Convey("chrono", {

        //testing timespan
        printf("testing w_timespan");
        w_timespan* _t_0 = w_timespan_init_from_zero();
        So(_t_0->ticks == 0 && !_t_0->overflowed);

        w_timespan* _t_now = w_timespan_init_from_now();
        So(_t_now->ticks != 0);
        
        w_timespan* _t_days = w_timespan_init_from_days(7);
        So(_t_days->ticks == 6048000000000);
    });

    //terminate wolf
    wolf_terminate();
 })
