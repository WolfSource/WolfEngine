#include "w_nudity_detection.hpp"

#include <iostream>

#include <torch/script.h>
#include <torch/torch.h>

#include "../w_common.hpp"
#include <opencv2/opencv.hpp>

using w_nud_det = wolf::ml::nudet::w_nud_det;

w_nud_det::w_nud_det(_In_ std::string& nudity_detection_model_path) {
  _model = torch::jit::load(nudity_detection_model_path);
  _model.to(torch::kCUDA);
  _model.eval();

  network_warm_up(get_env_int("TEMP_IMAGE_HEIGHT"), get_env_int("TEMP_IMAGE_WIDTH"));
}

w_nud_det::~w_nud_det() = default;

auto w_nud_det::nudity_detection(_In_ uint8_t* pImageData, _In_ const int pImageWidth,
                                 _In_ const int pImageHeight, _In_ const int pImageChannels)
    -> std::vector<float> {
  std::vector<float> result;

  // // Put image in tensor
  // std::vector<uint8_t>
  // 	image_data;
  // auto e = std::end(image_data);
  // const int total = pImageWidth * pImageHeight * pImageChannels;
  // image_data.insert(e, pImageData, pImageData + total);

  // auto input = cppflow::tensor(image_data, {pImageHeight, pImageWidth, pImageChannels});
  // input = cppflow::cast(input, TF_UINT8, TF_FLOAT, false);
  // input = input / 255.f;
  // input = cppflow::expand_dims(input, 0, static_cast<cppflow::datatype>(3));
  // auto output = _model(input);

  // result = output.get_data<float>();

  torch::Tensor tensor_image =
      torch::from_blob(pImageData, {1, pImageHeight, pImageWidth, pImageChannels}, torch::kByte);

  tensor_image = tensor_image.permute({0, 3, 1, 2});
  tensor_image = tensor_image.toType(torch::kFloat);
  tensor_image = tensor_image.div(255);

  // Define normalization constants
  const float kNormalizationMean[] = {0.485f, 0.456f, 0.406f};
  const float kNormalizationStd[] = {0.229f, 0.224f, 0.225f};

  // Create the normalization transform
  auto normalization_transform = torch::data::transforms::Normalize<float>(
      {kNormalizationMean[0], kNormalizationMean[1], kNormalizationMean[2]},
      {kNormalizationStd[0], kNormalizationStd[1], kNormalizationStd[2]});

  // Apply normalization transform to the image tensor
  tensor_image = normalization_transform(tensor_image);

  tensor_image = tensor_image.to(torch::kCUDA);

  torch::Tensor output;

  output = _model.forward({tensor_image}).toTensor();

  for (int i = 0; i < output.sizes()[1]; i++) {
    result.push_back(output[0][i].item<float>());
  }

  return result;
}

auto w_nud_det::network_warm_up(_In_ int pHeight, _In_ int pWidth) -> void {
  for (int i = 0; i < 2; i++) {
    cv::Mat const temp_image = cv::Mat(cv::Size(pWidth, pHeight), CV_8UC3, cv::Scalar(0, 0, 0));
    auto result = nudity_detection(temp_image.data, pWidth, pHeight, temp_image.channels());
  }
}
