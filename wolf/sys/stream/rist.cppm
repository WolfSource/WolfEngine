/*
        Project: Wolf Engine. Copyright   2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/
module;

#include <gsl/gsl>
#include <librist/librist.h>
#include <rist-private.h>
#include <memory>
#include <wolf.hpp>

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

export module wolf.stream.rist;

namespace wolf::stream::rist {

export struct w_rist_ctx_t {
  std::unique_ptr<rist_ctx> ctx;
  rist_profile profile;
  rist_logging_settings *log;
};
C_EXPORT typedef w_rist_ctx_t *w_rist_ctx;
C_EXPORT typedef int (*rist_log_callback)(void *, enum rist_log_level, const char *);

C_EXPORT
auto w_rist_receiver_create(w_rist_ctx p_rist, rist_log_callback p_log_callback) -> int {

  if (rist_logging_set(&p_rist->log, RIST_LOG_DEBUG, p_log_callback,
                       (void *)("receiver"), nullptr, stderr) != 0) {
    std::fprintf(stderr, "failed to setup logging for rist\n");
    return -1;
  }

  auto ctx_ptr = p_rist->ctx.get();
  return rist_receiver_create(
      &ctx_ptr, gsl::narrow_cast<rist_profile>(p_rist->profile), p_rist->log);
}

} // namespace wolf::stream::rist