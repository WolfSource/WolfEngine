#ifdef WOLF_STREAM_RIST

#include "w_rist_data_block.hpp"

using w_rist_data_block = wolf::stream::rist::w_rist_data_block;

w_rist_data_block::w_rist_data_block() noexcept {
  this->_block = new (std::nothrow) rist_data_block();
  // check for memory access violation
  std::ignore = gsl::not_null<rist_data_block *>(this->_block);
}

  // move constructor.
w_rist_data_block::w_rist_data_block(rist_data_block &&p_other) noexcept {
  if (this->_block != nullptr) {
    rist_receiver_data_block_free2(&this->_block);
  }
  this->_block = std::move(&p_other);
}

w_rist_data_block::~w_rist_data_block() {
  if (this->_block == nullptr)
    return;
  rist_receiver_data_block_free2(&this->_block);
  delete this->_block;
  this->_block = nullptr;
}

void w_rist_data_block::set(
    _In_ const std::tuple<const void *, size_t> &p_data) noexcept {
  this->_block->ts_ntp = 0; 
  this->_block->payload = std::get<0>(p_data);
  this->_block->payload_len = std::get<1>(p_data);
}

std::tuple<const void *, size_t> w_rist_data_block::get() const {
  return std::make_tuple(this->_block->payload, this->_block->payload_len);
}

const void* w_rist_data_block::get_data() const {
  return this->_block->payload;
}

size_t w_rist_data_block::get_data_len() const {
  return this->_block->payload_len;
}

std::string w_rist_data_block::to_string() const {
  return std::string(gsl::narrow_cast<const char *>(this->_block->payload),
                     this->_block->payload_len);
}

#endif