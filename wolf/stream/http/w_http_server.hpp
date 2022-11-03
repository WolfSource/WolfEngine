#pragma once

#include <CivetServer.h>
#include <functional>
#include <json/json.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace wolf::stream::http
{
  using w_http_function =
      std::function<std::pair<std::map<std::string, std::string>, Json::Value>(
          const mg_request_info *req_info, const Json::Value &)>;

  class w_http_server
  {
  public:
    w_http_server(const std::vector<std::string> &p_options,
                  const void *p_user_data);
    virtual ~w_http_server(){};

    // prevent copy constructor
    w_http_server(const w_http_server &) = delete;

    // prevent copying
    w_http_server &operator=(const w_http_server &) = delete;

    // void add_handlers(std::map<std::string, w_http_function> &p_funcs);
    // void remove_handler(std::string p_handler_name);

  private:
    std::unique_ptr<CivetServer> _server;
  };
} // namespace wolf::stream::http
