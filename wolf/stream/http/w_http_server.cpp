#include "w_http_server.hpp"
#include <cstdio>

using w_http_server = wolf::stream::http::w_http_server;

int log_message(const struct mg_connection *p_conn, const char *p_message)
{
  (void)p_conn;
  std::fprintf(stderr, "%s\n", p_message);
  return 0;
}

static CivetCallbacks s_callbacks;
const CivetCallbacks *get_civet_callbacks()
{
  std::memset(&s_callbacks, 0, sizeof(s_callbacks));
  s_callbacks.log_message = &log_message;
  return &s_callbacks;
}

w_http_server::w_http_server(const std::vector<std::string> &p_options,
                             const void *p_user_data)
    : _server(std::make_unique<CivetServer>(p_options, get_civet_callbacks(),
                                            p_user_data)) {}