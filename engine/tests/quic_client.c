#include <wolf.h>
#include <net/w_net.h>

#define MAX_BUFFER_SIZE 65535

void s_quic_debug_log_callback(const char* pLine, void* pArgp)
{
    printf("%s", pLine);
}

static bool _req_sent = false;
W_RESULT s_quic_sending_stream_callback(
    uint8_t* pConnectionID,
    uint64_t pStreamIndex,
    const uint8_t* pProtocol,
    const size_t pProtocolLen)
{
    if (!_req_sent)
    {
        static w_buffer _b;
        _b.data = "GET / index.html\r\n";
        _b.len = strlen(_b.data);

        size_t _len = w_net_send_msg_quic(
            pConnectionID,
            pStreamIndex,
            &_b,
            true);
        _req_sent = true;

        printf("\np is %d", pProtocol);
    }

    return W_SUCCESS;
}

W_RESULT s_quic_receiving_stream_callback(
    uint8_t* pConnectionID,
    uint64_t pStreamIndex,
    const uint8_t* pProtocol,
    const size_t pProtocolLen)
{
    bool _fin = false;
    w_buffer _b;
    _b.len = MAX_BUFFER_SIZE;
    _b.data = (uint8_t*)w_malloc(sizeof(uint8_t), "s_quic_r_stream_callback");

    size_t _recv_len = w_net_receive_msg_quic(
        pConnectionID,
        pStreamIndex,
        &_b,
        &_fin);
    if (_recv_len < 0)
    {
        return W_FAILURE;
    }

    //logger printf("%.*s", (int)recv_len, buf);
    W_RESULT _ret = W_SUCCESS;
    if (_fin)
    {
        _ret = w_net_close_quic_socket();
    }

    return _ret;
}

int main()
{
    wolf_initialize();

    w_net_open_quic_socket(
        "localhost",
        5555,
        quic_dialer,
        NULL,
        NULL,
        s_quic_debug_log_callback,
        s_quic_receiving_stream_callback,
        s_quic_sending_stream_callback);

    wolf_terminate();

    return 0;
}
