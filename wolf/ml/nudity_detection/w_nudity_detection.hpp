/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <iostream>

#include <torch/script.h>
#include <torch/torch.h>

// #include "cppflow/cppflow.h"
// #include "dlib_export.h"
// #include "salieri.h"

#include "wolf.hpp"

namespace wolf::ml::nudet {

class w_nud_det {
 public:
  /*!
          The constructor of the class.
  */
  explicit w_nud_det(_In_ std::string& nudity_detection_model_path);

  /*!
          The deconstructor of the class.
  */
  ~w_nud_det();

  /*!
  The function gets a string as input and return the boolean representation of the input.

          \param pVariable the string input.
          \return a vector of float numbers each between 0 to 1 that shows the nudity factors.
  */
  W_API auto nudity_detection(_In_ uint8_t* pImageData, _In_ int pImageWidth, _In_ int pImageHeight,
                              _In_ int pImageChannels) -> std::vector<float>;

  /*!
  The function uses to warm-up the network in the w_nud_det class initialization.

          \param pHeight the temp image height.
          \param pWidth the temp image width.
          \return (void)
  */
  W_API auto network_warm_up(_In_ int pHeight, _In_ int pWidth) -> void;

 private:
  // :cppflow:model _model;
  torch::jit::script::Module _model;
};
}  // namespace wolf::ml::nudet
