
#include <wolf.h>
#include"net/w_amq.h"
#include"concurrency/w_concurrent_queue.h"
#include"concurrency/w_thread.h"
#include"log/w_log.h"
int main()
{
    wolf_init();

    w_mem_pool _mem_pool = NULL;
    int x = w_mem_pool_init(&_mem_pool);
    w_amq_init(_mem_pool);
    const char* _trace_info = "test";
    if (!_mem_pool)
    {
        W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        return W_FAILURE;
    }
    w_thread _thread = NULL;
    w_net_init_producer(_mem_pool);
    int i;
    char* message = "majidi";
    int count = 0;

    w_net_open_producer();

    for (i = 0; i <= 2; i++) {
        w_net_send_producer(message, 0);

    }



    w_net_producer_fini();
    w_amq_fini(_mem_pool);

    w_mem_pool_fini(&_mem_pool);
    return 0;
}
