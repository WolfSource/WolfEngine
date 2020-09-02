#include <wolf.h>
#include <io/w_io.h>
#include <net/w_net.h>

#define MAX_BUFFER_SIZE 65535
static w_mem_pool s_mem_pool = NULL;
static w_buffer_t s_buffer;
void s_quic_debug_log_callback(const char* pLine, void* pArgp)
{
    printf("%s", pLine);
}

W_RESULT s_quic_receiving_stream_callback(
    uint8_t* pConnectionID,
    uint64_t pStreamIndex,
    const uint8_t* pProtocol,
    const size_t pProtocolLen)
{
    printf("\nstream %llu is readable\n", pStreamIndex);


    s_buffer.len = MAX_BUFFER_SIZE;
    if (!s_buffer.data)
    {
        s_buffer.data = (uint8_t*)w_malloc(s_mem_pool, sizeof(uint8_t));
    }

    bool _stream_finished = false;

    size_t _len = w_net_receive_msg_quic(
        pConnectionID,
        pStreamIndex,
        &s_buffer,
        &_stream_finished);

    printf("\n client data: %s \n", (char*)s_buffer.data);

    static const char* _resp = "success\n";
    s_buffer.len = strlen(_resp);
    memcpy(s_buffer.data, _resp, s_buffer.len);
    w_net_send_msg_quic(
        pConnectionID,
        pStreamIndex,
        &s_buffer,
        _stream_finished);

    if (_stream_finished)
    {
        static const char* _resp = "success\n";
        s_buffer.len = strlen(_resp);
        memcpy(s_buffer.data, _resp, s_buffer.len);
        w_net_send_msg_quic(
            pConnectionID,
            pStreamIndex,
            &s_buffer,
            _stream_finished);
    }

    return W_SUCCESS;
}

W_RESULT s_quic_sending_stream_callback(
    uint8_t* pConnectionID,
    uint64_t pStreamIndex,
    const uint8_t* pProtocol,
    const size_t pProtocolLen)
{
    return W_SUCCESS;
}

int main()
{
    wolf_init();

    char* _current_dir = NULL;

    w_mem_pool_init(&s_mem_pool, W_MEM_POOL_FAST_EXTEND);
    if (s_mem_pool)
    {
        _current_dir = w_io_dir_get_current(s_mem_pool);

        char* _crt = w_strcat(s_mem_pool, _current_dir, "/cert.crt", NULL);
        char* _key = w_strcat(s_mem_pool, _current_dir, "/cert.key", NULL);

        w_net_open_quic_socket(
            "localhost",
            5555,
            quic_listener,
            _crt,
            _key,
            s_quic_debug_log_callback,
            s_quic_receiving_stream_callback,
            s_quic_sending_stream_callback);
    }
    w_mem_pool_fini(&s_mem_pool);
    wolf_fini();

    return 0;
}
