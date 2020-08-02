#include <wolf.h>
#include <net/w_net.h>

#define MAX_BUFFER_SIZE 65535

void s_quic_debug_log_callback(const char* pLine, void* pArgp)
{
    printf("%s", pLine);
}

void s_quic_stream_callback(uint8_t* pConnectionID, uint64_t pStreamIndex)
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
        strcpy(_b.data, _resp);
        _b.len = strlen(_resp);
        w_net_send_msg_quic(
            pConnectionID,
            pStreamIndex,
            &_b,
            _stream_finished);

    }
}

int main()
{
    wolf_initialize();

    w_net_open_quic_socket(
        "localhost",
        5555,
        quic_listener,
        "D:/SourceCodes/PlayPod/playpod.quiche/samples/client_server_quiche/server_quiche/cert.crt",
        "D:/SourceCodes/PlayPod/playpod.quiche/samples/client_server_quiche/server_quiche/cert.key",
        s_quic_debug_log_callback,
        s_quic_stream_callback);

    wolf_terminate();

    return 0;
}
