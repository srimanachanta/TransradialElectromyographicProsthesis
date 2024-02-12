#pragma once

#include <torch/script.h>
#include <utility>
#include "ModelConfig.h"
#include <iostream>

/**
 * Class references of the classifier. Represents the muscular state of each major DoF of the hand.
 */
enum DoFForce {
    kNoMovement,
    kExtending,
    kFlexing
};

/**
 * Wrapped class around a TorchScript model for classifying the force output of the hand based on the last 512
 * samples collected from the bridge. Samples should be collected and fed in a uniform fashion using the classify
 * method call (~1 ksps).
 */
class EMGSignalClassifier {
    inline static torch::jit::script::Module module = torch::jit::load(CLASSIFIER_TRACED_MODEL_PATH);

public:
    /**
     * Determine the finger muscular force state of each DoF based on measured motor units collected over a
     * uniform time period (1 ksps). Calling this method with non-uniform time periods will significantly reduce
     * the accuracy of predictions.
     *
     * @param data
     * @return Returns the predicted classes of the DoF based on measured motor units. The DoFs are as follows:
     * 1. Proximal Thumb
     * 2. Proximal Index
     * 3. Proximal Middle
     * 4. Proximal Ring
     * 5. Proximal Pinkie
     * 6. Wrist
     */
    static std::vector<DoFForce> classify(const torch::Tensor& data) {
        std::vector<torch::jit::IValue> inputs;
        // MPS is disabled due to incomplete implementations of operations using the MPS.
        // Partial usage with fallback can be implemented for performance improvements.
        // TODO, handle running the classifier on CUDA enabled devices
        inputs.emplace_back(data.to(torch::kCPU));

        auto pred = module.forward(inputs).toTensor();

        std::vector<DoFForce> out;
        out.reserve(6);
        for(int i = 0; i < 6; i++) {
            auto encoded_force = pred[0][i];

            // Decode the encoded force
            out.emplace_back(static_cast<DoFForce>(torch::argmax(encoded_force).item<int>()));
        }

        // Ensure there wasn't an error in decoding
        assert(out.size() == 6);

        return out;
    }
};
