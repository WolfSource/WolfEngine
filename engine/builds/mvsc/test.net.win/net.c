#include <wolf.h>
#include<net/w_net.h>
int main()
{
    wolf_initialize();

    static const char cert[] =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIBzDCCAXkCCQCNJMf8eYUHxTAKBggqhkjOPQQDAjB2MQswCQYDVQQGEwJVUzEL\n"
        "MAkGA1UECAwCQ0ExEjAQBgNVBAcMCVNhbiBEaWVnbzEUMBIGA1UECgwLbmFub21z\n"
        "Zy5vcmcxHDAaBgNVBAsME1NhbXBsZSBDZXJ0aWZpY2F0ZXMxEjAQBgNVBAMMCWxv\n"
        "Y2FsaG9zdDAgFw0yMDAyMjMxODMwMDZaGA8yMTIwMDEzMDE4MzAwNlowdjELMAkG\n"
        "A1UEBhMCVVMxCzAJBgNVBAgMAkNBMRIwEAYDVQQHDAlTYW4gRGllZ28xFDASBgNV\n"
        "BAoMC25hbm9tc2cub3JnMRwwGgYDVQQLDBNTYW1wbGUgQ2VydGlmaWNhdGVzMRIw\n"
        "EAYDVQQDDAlsb2NhbGhvc3QwTjAQBgcqhkjOPQIBBgUrgQQAIQM6AAS9hA5gYo10\n"
        "jx+gzJdzYbxHzigJYXawdHtyoAud/TT/dUCt0ycpOzTMiO3CoDNxep+/mkmgxjfp\n"
        "ujAKBggqhkjOPQQDAgNBADA+Ah0A9b+GcfbhzzmI2NcYb4auE6XTYJPkPzHt6Adi\n"
        "fwIdAMJO2LEr6WHH6JGLlishVqjF78TtkuB5t+kzneQ=\n"
        "-----END CERTIFICATE-----\n";

    static const char key[] =
        "-----BEGIN EC PARAMETERS-----\n"
        "gUrgQQAIQ==\n"
        "-----END EC PARAMETERS-----\n"
        "-----BEGIN EC PRIVATE KEY-----\n"
        "MGgCAQEEHChK068x8MWcBzhpO7qANvW4iTo7E0yzMYFXGn+gBwYFK4EEACGhPAM6\n"
        "AAS9hA5gYo10jx+gzJdzYbxHzigJYXawdHtyoAud/TT/dUCt0ycpOzTMiO3CoDNx\n"
        "ep+/mkmgxjfpug==\n"
        "-----END EC PRIVATE KEY-----\n";

    W_RESULT _net_init = w_net_init();


    const char* pUrlAddress ="http://127.0.0.1:5555";
    w_url pURL = w_malloc(sizeof(w_url),"main::w_url");
    W_RESULT net_url_parse =w_net_url_parse( pUrlAddress, pURL);

    const char* _net_url_encoded = w_net_url_encoded( pUrlAddress);

   // w_net_url_free(pURL);

    /*char* pPort = "5555";
    w_tcp_socket_address_family pSocketAddressFamily= IPV6;
    int pBindOrConnect=1;
    w_socket_address pSocketAddress=w_malloc(sizeof(w_socket_address),"w_socket_address");
    W_RESULT _net_resolve =w_net_resolve(pUrlAddress, pPort, pSocketAddressFamily, pBindOrConnect, pSocketAddress);*/

    const char* pEndPoint = "tcp://127.0.0.1:5555";
   w_socket_tcp* pSocket=(w_socket_tcp*)w_malloc(sizeof(w_socket_tcp),"");
    const char* pTLSServerName = "127.0.0.1";

    W_RESULT _net_open_tcp_socket = w_net_open_tcp_socket(pEndPoint, req_rep_dialer, true, true, true, true, NULL, pTLSServerName, false, cert, key,&pSocket);


   /* char* pMessage = "yes";
    size_t pMessageLength = strlen(pMessage);
    W_RESULT _net_send_msg_tcp =w_net_send_msg_tcp( pSocket, pMessage, 3, false);

    W_RESULT _net_receive_msg_tcp= w_net_receive_msg_tcp(pSocket, pMessage, &pMessageLength);
  
    W_RESULT  net_close_tcp_socket = w_net_close_tcp_socket( pSocket);*/


   const char* pEndPoint_udp = "udp://127.0.0.1:5555";
   w_socket_udp* udp_Socket =w_malloc(sizeof(w_socket_udp),"");
   //W_RESULT _net_open_udp_socket =w_net_open_udp_socket(pEndPoint_udp, &udp_Socket);


   
   void w_net_close_udp_socket(udp_Socket);

   char* pMessag = "jhg";
   W_RESULT _io_udp_socket(_one_way_puller, udp_Socket, pMessage, pMessageLength);
   
      
    wolf_terminate();

    return 0;
}
