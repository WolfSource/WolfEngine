#include <wolf.h>
#include <io/w_io.h>
#include <net/w_net.h>

#define MAX_BUFFER_SIZE 65535

void s_quic_debug_log_callback(const char* pLine, void* pArgp)
{
    printf("%s", pLine);
}

void s_quic_receiving_stream_callback(
    uint8_t* pConnectionID, 
    uint64_t pStreamIndex, 
    const uint8_t* pProtocol,
    const size_t pProtocolLen)
{
    printf("\nstream index is %d", pStreamIndex);

    w_buffer _b;
    _b.len = MAX_BUFFER_SIZE;
    _b.data = (uint8_t*)w_malloc(sizeof(uint8_t), "s_quic_stream_callback");

    bool _stream_finished = false;
    size_t _len = w_net_receive_msg_quic(
        pConnectionID,
        pStreamIndex,
        &_b,
        &_stream_finished);

    if (_stream_finished)
    {
        static const char* _resp = "bye\n";
        _b.len = strlen(_resp);
        memcpy(_b.data, _resp, _b.len);
        w_net_send_msg_quic(
            pConnectionID,
            pStreamIndex,
            &_b,
            _stream_finished);
    }

    return W_SUCCESS;
}

int main()
{
    wolf_initialize();

    const char* _current_dir = w_io_dir_get_current();
    char* _crt = w_string_concat(2, _current_dir, "/cert.crt");
    char* _key = w_string_concat(2, _current_dir, "/cert.key");

    w_net_open_quic_socket(
        "localhost",
        5555,
        quic_listener,
        _crt,
        _key,
        s_quic_debug_log_callback,
        s_quic_receiving_stream_callback,
        NULL);

    wolf_terminate();

    return 0;
}
