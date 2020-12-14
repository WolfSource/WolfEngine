#include <wolf.h>
#include"net/w_amq.h"
#include"concurrency/w_concurrent_queue.h"
#include"concurrency/w_thread.h"
#include"log/w_log.h"
void pCallback_tmp(char* message)
{
    printf("%s", message);
}
/*main for consumer*/
int main()
{
    wolf_init();

    w_mem_pool _mem_pool = NULL;
    w_mem_pool_init(&_mem_pool);
    w_amq_init(_mem_pool);
    const char* _trace_info = "test";
    if (!_mem_pool)
    {
        W_ASSERT_P(false, "bad args! trace info:%s", _trace_info);
        return W_FAILURE;
    }
    w_net_receive_consumer(_mem_pool, pCallback_tmp);

    while (true)
    {
        Sleep(50);

    }

    w_net_consumer_fini();
    w_amq_fini(_mem_pool);
    w_mem_pool_fini(&_mem_pool);

    return 0;
}
