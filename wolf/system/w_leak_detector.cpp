#include "w_leak_detector.hpp"
#include <iostream>

using w_leak_detector = wolf::system::w_leak_detector;

w_leak_detector::w_leak_detector() noexcept
#if defined(WIN32) && defined(_DEBUG)
    : _mem_state()
#endif
{
#if defined(WIN32) && defined(_DEBUG)
  // take a snapshot from memory
  _CrtMemCheckpoint(&this->_mem_state);
#endif
}

w_leak_detector::~w_leak_detector() noexcept {
#if defined(WIN32) && defined(_DEBUG)
  try {
    // take a snapshot from memory
    _CrtMemState _diff_mem;
    _CrtMemState _new_mem_state;
    _CrtMemCheckpoint(&_new_mem_state);

    const auto _dif = _CrtMemDifference(&_diff_mem, &this->_mem_state, &_new_mem_state);
    if (_dif > 0) {
      _CrtMemDumpStatistics(&_diff_mem);
      _CrtMemDumpAllObjectsSince(&this->_mem_state);
      _CrtDumpMemoryLeaks();
      std::cerr << "Detected memory leak!\r\n";
      assert(false);
      std::terminate();
    }
  } catch (...) {
  }
#endif
}
