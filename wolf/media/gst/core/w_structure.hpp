#pragma once

#include "wolf.hpp"

#include "media/gst/internal/w_wrapper.hpp"

#include <gst/gst.h> // use specific headers or forward-declare

#include <string_view>
#include <concepts>

namespace wolf::gst {

struct w_fraction
{
    std::size_t numerator;
    std::size_t denomerator = 1;
};

/**
 * @brief wrapper of GstStructure, commonly used to represent a single gstreamer caps.
 */
class w_structure : public w_wrapper<w_structure, GstStructure, void, gst_structure_free>
{
    friend class internal::w_raw_access;

public:
    /**
     * @brief make an instance of structure with given name.
     * @param p_name name of cap strucutre.
     * @return instance of structure on success.
     */
    [[nodiscard]] static auto make(const char* p_name)
        -> boost::leaf::result<w_structure>;

    /**
     * @brief make structure cap from given string representation.
     * @param p_str gstreamer string repr of a structure.
     * @return instance of structure on success.
     */
    [[nodiscard]] static auto make_from_str(const char* p_str)
        -> boost::leaf::result<w_structure>;

    /**
     * @brief set name/value pairs of feilds.
     */
    template <typename T, typename ...Rest>
    void set_field_pairs(const char* p_fieldname, T&& p_fieldvalue, Rest&& ...p_rest)
    {
        set_field(p_fieldname, std::forward<T>(p_fieldvalue));

        if constexpr (sizeof...(Rest) > 0) {
            set_field_pairs(std::forward<Rest>(p_rest)...);
        }
    }

    /**
     * @brief set a fraction field.
     */
    void set_field(const char* p_fieldname, w_fraction p_fraction)
    {
        gst_structure_set(
            raw(),
            p_fieldname,
            GST_TYPE_FRACTION,
            p_fraction.numerator,
            p_fraction.denomerator,
            nullptr
        );
    }

    /**
     * @brief set uint64 field.
     */
    void set_field(const char* p_fieldname, std::size_t p_value)
    {
        gst_structure_set(raw(), p_fieldname, G_TYPE_UINT64, p_value, nullptr);
    }

private:
    w_structure(internal::w_raw_tag, GstStructure* p_structure_raw)
        : w_wrapper(p_structure_raw)
    {}
};

}  // namespace wolf::gst
