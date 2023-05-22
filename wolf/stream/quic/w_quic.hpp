#ifndef WOLF_STREAM_QUIC
#error "WOLF_STREAM_QUIC feature is not enabled."
#endif

#pragma once

#include <wolf.hpp>

#include <msquic.h>

W_API auto foo() -> const QUIC_API_TABLE*;
