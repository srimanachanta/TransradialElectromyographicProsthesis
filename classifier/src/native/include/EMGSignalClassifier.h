#pragma once

#include <torch/script.h>
#include <utility>
#include "ModelConfig.h"
#include <iostream>

class EMGSignalClassifier {
    inline static torch::jit::script::Module module = torch::jit::load(CLASSIFIER_TRACED_MODEL_PATH);

public:
    static void classify(const torch::Tensor& data) {
        std::vector<torch::jit::IValue> inputs;
        // MPS is disabled due to incomplete implementations of operations using the MPS.
        // Partial usage with fallback can be implemented for performance improvements.
        // TODO, handle running the classifier on CUDA enabled devices
        inputs.emplace_back(data.to(torch::kCPU));

        auto pred = module.forward(inputs).toTensor();
    }
};
