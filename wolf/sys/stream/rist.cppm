/*
        Project: Wolf Engine. Copyright   2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/
module;

#include <gsl/gsl>
#include <librist/librist.h>
#include <rist-private.h>
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
  rist_ctx* ctx;
  rist_profile profile;
  rist_logging_settings* log;
};
C_EXPORT typedef w_rist_ctx_t *w_rist_ctx;
C_EXPORT typedef int (*rist_log_callback)(void *, enum rist_log_level,
                                          const char *);

C_EXPORT
auto w_rist_receiver_create(w_rist_ctx *p_rist,
                            rist_log_callback p_log_callback) -> int {

  constexpr auto size = sizeof(w_rist_ctx_t);
  auto rist = gsl::narrow_cast<w_rist_ctx>(calloc(1, size));
  if (!rist) {
    return -1;
  }

  if (rist_logging_set(&rist->log, RIST_LOG_DEBUG, p_log_callback,
                       (void *)("receiver"), nullptr, stderr) != 0) {
    return -1;
  }

  auto ret = rist_receiver_create(&rist->ctx, rist->profile, rist->log);
  if (ret == 0)
  {
    *p_rist = rist;
  }
  return ret;
}

C_EXPORT
auto w_rist_destroy(w_rist_ctx *p_rist) -> void {
  if (p_rist == nullptr || (*p_rist) == nullptr)
    return;

  if ((*p_rist)->ctx) {
    rist_destroy((*p_rist)->ctx);
  }

  // use free instead of mi_free
  // there is a bug with mi_free on windows
   if ((*p_rist)->log) {
     free ((*p_rist)->log);
   }

  free(*p_rist);
}

} // namespace wolf::stream::rist