#pragma once

#include "rust/cxx.h"
#include <librist/librist.h>
#include <memory>

namespace rist_cxx
{
    class Rist
    {
    public:
        Rist(rust::Str p_name);
        virtual ~Rist();

        auto init() const -> void;
        //auto init(int p_profile, rust::Str p_url, rust::String &p_error_msg) -> int;

    private:
        std::string name;
        rist_ctx *ctx;
    };

    std::unique_ptr<Rist> New(rust::Str p_name)
    {
        return std::make_unique<Rist>(p_name);
    }
} // namespace rist_cxx