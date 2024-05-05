#pragma once

#include <torch/torch.h>
#include "DoFStateClassifier.h"

class EMGDataClassifier {
 public:
  virtual ~EMGDataClassifier() = default;

  virtual DoFStateClassification Classify(torch::Tensor input) = 0;
};
