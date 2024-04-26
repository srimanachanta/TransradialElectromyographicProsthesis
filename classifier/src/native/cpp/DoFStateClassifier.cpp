#include "DoFStateClassifier.h"

#include <torch/cuda.h>
#include "ModelConfig.h"

DoFStateClassifier::DoFStateClassifier(const torch::DeviceType deviceType) {
    switch(deviceType) {
        case torch::kCUDA:
            device_type = torch::kCUDA;
            throw std::runtime_error("CUDA backend hasn't been implemented yet");
        default:
        case torch::kCPU:
            device_type = torch::kCPU;
            module = torch::jit::load(CLASSIFIER_TRACED_MODEL_CPU_PATH);
        break;
    }
}

torch::DeviceType DoFStateClassifier::GetDeviceType() {
    if(torch::cuda::is_available()) {
        return torch::kCUDA;
    }

    return torch::kCPU;
}


DoFStateClassification DoFStateClassifier::Classify(const torch::Tensor &data) {
    std::vector<torch::jit::IValue> inputs;
    inputs.emplace_back(data.to(device_type));

    // TODO optimize further ig
    auto const pred = module.forward(inputs).toTensor();

    const DoFStateClassification out{
        static_cast<DoFForce>(torch::argmax(pred[0][0]).item<int>()),
        static_cast<DoFForce>(torch::argmax(pred[0][1]).item<int>()),
        static_cast<DoFForce>(torch::argmax(pred[0][2]).item<int>()),
        static_cast<DoFForce>(torch::argmax(pred[0][3]).item<int>()),
        static_cast<DoFForce>(torch::argmax(pred[0][4]).item<int>()),
        static_cast<DoFForce>(torch::argmax(pred[0][5]).item<int>())
    };

    return out;
}
