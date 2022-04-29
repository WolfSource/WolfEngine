#include "WString.hpp"
#include "wolfengine/src/system/memory/w_string.rs.h"

using WString = wolf::system::memory::WString;

auto WString::find_str(const rust::Str p_val) const -> size_t
{
    auto _str = std::string(p_val);
    return this->find(_str);
}

auto WString::find_first_of_str(const rust::Str p_val, const size_t p_pos) const -> size_t
{
    auto _str = std::string(p_val);
    return this->find_first_of(_str, p_pos);
}

auto WString::get_length() const -> size_t
{
    return this->length();
}

auto WString::get_size() const -> size_t
{
    return this->size();
}

auto WString::get_max_size() const -> size_t
{
    return this->max_size();
}

auto WString::get_c_str() const -> rust::Str
{
    return this->c_str();
}