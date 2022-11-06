//#ifdef WOLF_STREAM_RIST
//
//#include "w_rist_data_block.hpp"
//
//#include <DISABLE_ANALYSIS_BEGIN>
//#include <librist/librist.h>
//#include <rist-private.h>
//#include <DISABLE_ANALYSIS_END>
//
//using w_rist_data_block = wolf::stream::w_rist_data_block;
//
//struct w_rist_data_block::priv
//{
//    priv() noexcept : block(nullptr)
//    {
//    }
//    
//    priv(const priv&) = delete;
//    priv(priv&&) = delete;
//    priv& operator=(const priv&) = delete;
//    priv& operator=(priv&&) = delete;
//
//    ~priv()
//    {
//        if (this->block == nullptr) return;
//        rist_receiver_data_block_free2(&this->block);
//        this->block = nullptr;
//    }
//
//    rist_data_block* block = nullptr;
//};
//
//w_rist_data_block::w_rist_data_block() noexcept : _priv(std::make_unique<priv>())
//{
//}
//
//w_rist_data_block::~w_rist_data_block()
//{
//    release();
//}
//
//// Move constructor.
//w_rist_data_block::w_rist_data_block(w_rist_data_block&& p_other) noexcept
//{
//    //release the current data
//    release();
//
//    //move new data to current
//    this->_priv.swap(p_other._priv);
//    p_other._priv.reset();
//}
//
//// Move assignment operator.
//w_rist_data_block& w_rist_data_block::operator=(w_rist_data_block&& p_other) noexcept
//{
//    if (this != &p_other)
//    {
//        //release the current data
//        release();
//
//        //move new data to current
//        this->_priv.swap(p_other._priv);
//        p_other._priv.reset();
//    }
//    return *this;
//}
//
//tl::expected<void, w_result> w_rist_data_block::init()
//{
//    constexpr auto TRACE = "w_rist_data_block::init";
//
//    if (this->_priv == nullptr)
//    {
//        auto _res = w_result("_priv is null");
//        return tl::unexpected(_res);
//    }
//
//    // allocate memory for rist_data_block
//    this->_priv->block = gsl::owner<rist_data_block*>(calloc(1, sizeof(rist_data_block)));
//    if (this->_priv->block == nullptr)
//    {
//        auto _res = w_result("could not allocate memory for rist_data_block");
//        return tl::unexpected(_res);
//    }
//
//    W_EXPECTED_VOID
//}
//
//tl::expected<void, w_result> w_rist_data_block::set(const void* p_data, size_t p_len)
//{
//    constexpr auto TRACE = "w_rist_data_block::set";
//
//    if (this->_priv == nullptr || this->_priv->block == nullptr)
//    {
//        auto _res = w_result("missing context");
//        return tl::unexpected(_res);
//    }
//
//    this->_priv->block->payload = p_data;
//    this->_priv->block->payload_len = p_len;
//
//    W_EXPECTED_VOID
//}
//
//tl::expected<const void*, w_result> w_rist_data_block::get() const
//{
//    if (this->_priv == nullptr || this->_priv->block == nullptr)
//    {
//        auto _res = w_result("unexpected nullptr");
//        return tl::unexpected(_res);
//    }
//
//    return this->_priv->block->payload;
//}
//
//tl::expected<size_t, w_result> w_rist_data_block::get_len() const
//{
//    if (this->_priv == nullptr || this->_priv->block == nullptr)
//    {
//        auto _res = w_result("unexpected nullptr");
//        return tl::unexpected(_res);
//    }
//
//    return this->_priv->block->payload_len;
//}
//
//void w_rist_data_block::release() noexcept
//{
//    if (this->_priv != nullptr)
//    {
//        this->_priv.reset();
//    }
//}
//
//tl::expected<rist_data_block*, w_result> w_rist_data_block::get_internal_block()
//{
//    if (this->_priv == nullptr || this->_priv->block == nullptr)
//    {
//        auto _res = w_result("unexpected nullptr");
//        return tl::unexpected(_res);
//    }
//
//    return this->_priv->block;
//}
//
//#endif