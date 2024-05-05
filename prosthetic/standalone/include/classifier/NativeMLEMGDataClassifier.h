#pragma once

#include "EMGDataClassifier.h"

class NativeMLEMGDataClassifier final : EMGDataClassifier {
public:
    explicit NativeMLEMGDataClassifier(const std::string& in) : classifier(in, torch::kCPU) {}

    DoFStateClassification Classify(const torch::Tensor input) override {
        return classifier.Classify(input);
    }
private:
    DoFStateClassifier classifier;
};
