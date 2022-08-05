#pragma once

#include <wolf.hpp>
#include <memory>
#include <lsquic.h>
#include <lsquic_types.h>
#include <lsxpack_header.h>

namespace wolf::stream::quic
{
  class w_quic
  {
  public:
    w_quic();
    virtual ~w_quic(){};

  private:
    std::unique_ptr<lsquic_engine_t> quic_engine;
  };
} // namespace wolf::stream::http
