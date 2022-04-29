/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/
#pragma once

#include "rust/cxx.h"
#include <folly/FBString.h>
#include <memory>

namespace wolf::system::memory
{
    class WString : public folly::fbstring
    {
    public:
        // default constructor
        WString() = default;

        // destructor
        virtual ~WString() = default;

        // constructor
        explicit WString(std::string p_val) : folly::fbstring(p_val)
        {
        }

        // copy constructor
        WString(const WString &p_val) : folly::fbstring(p_val.data()) // NOLINT (fuchsia-default-arguments-calls)
        {
        }

        // move constructor
        WString(WString &&p_val) noexcept
        {
            *this = p_val;
        }

        // copy assignment
        WString &operator=(const WString &p_val)
        {
            if (this != &p_val)
            {
#if _MSC_VER
                strncpy(this->data(), p_val.data(), p_val.size());
#else
                strlcpy(this->data(), p_val.data(), p_val.size());
#endif
            }
            return *this;
        }

        // move assignment
        WString &operator=(WString &&p_val) noexcept
        {
            this->swap(p_val);
            return *this;
        }

        /**
         * the function returns the index position of searched one
         * @param p_val the string which should be find
         * @return result start index 
         */
        auto find_str(const rust::Str p_val) const -> size_t;

        /**
         * find the first occurrence of a string in this string
         * @param p_val the string which should be find
         * @param p_pos from position
         * @return result start index 
         */
        auto find_first_of_str(const rust::Str p_val, const size_t p_pos) const -> size_t;

        /**
         * the length of string
         * @return the length 
         */
        auto get_length() const -> size_t;

        /**
         * the size of string
         * @return the length 
         */
        auto get_size() const -> size_t;

        /**
         * the max size of string
         * @return the max size 
         */
        auto get_max_size() const -> size_t;

        /**
         * convert to c str to rust str
         * @return the rust string 
         */
        auto get_c_str() const -> rust::Str;
    };

    /**
     * create an instance of WString
     * @return a unique pointer to the instance of WString 
     */
    inline auto New() -> std::unique_ptr<WString>
    {
        return std::make_unique<WString>();
    }

    /**
     * create an instance of WString
     * @param p_val the initial value
     * @return a unique pointer to the instance of WString 
     */
    inline auto From(const rust::Str p_val) -> std::unique_ptr<WString>
    {
        auto _str = std::string(p_val);
        return std::make_unique<WString>(_str);
    }

    /**
     * get npos of string
     * @return the npos of string 
     */
    inline auto npos() -> size_t 
    {
        return folly::fbstring::npos;
    }
} // namespace wolf::system::memory
