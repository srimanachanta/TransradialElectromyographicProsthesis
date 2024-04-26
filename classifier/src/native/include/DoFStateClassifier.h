#pragma once

#include <torch/script.h>

/**
 * Class references of the classifier. Represents the muscular state of each major DoF of the hand.
 */
enum DoFForce {
    kNoMovement,
    kExtending,
    kFlexing
};

struct DoFStateClassification {
    DoFForce thumb;
    DoFForce index;
    DoFForce middle;
    DoFForce ring;
    DoFForce little;
    DoFForce wrist;
};

/**
 * Wrapped class around a TorchScript model for classifying the force output of the hand based on the last 512
 * samples collected from the bridge. Samples should be collected and fed in a uniform fashion using the classify
 * method call and measured at a sampling rate of ~1 ksps.
 */
class DoFStateClassifier {
public:
    explicit DoFStateClassifier(torch::DeviceType deviceType);

    static torch::DeviceType GetDeviceType();

    /**
     * Determine the finger muscular force state of each DoF based on measured motor units collected over a
     * uniform time period (1 ksps). Calling this method with non-uniform time periods will significantly reduce
     * the accuracy of predictions.
     *
     * @param data
     * @return Returns the predicted classes of the DoFs based on measured motor units.
     */
    DoFStateClassification Classify(const torch::Tensor& data);

private:
    inline static torch::jit::script::Module module;
    torch::DeviceType device_type;
};
