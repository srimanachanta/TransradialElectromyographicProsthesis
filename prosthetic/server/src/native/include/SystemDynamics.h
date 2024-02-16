#pragma once

#include <torch/torch.h>
#include <boost/circular_buffer.hpp>
#include <mutex>

// Order of struct fields corresponds to the port of servo in the prosthetic
struct ProstheticServoPositions {
    unsigned short thumbLeft;
    unsigned short thumbRight;
    unsigned short indexLeft;
    unsigned short indexRight;
    unsigned short middleLeft;
    unsigned short middleRight;
    unsigned short ringLeft;
    unsigned short ringRight;
    unsigned short pinkieLeft;
    unsigned short pinkieRight;
    unsigned short wristExtension;
    unsigned short wristFlexion;
};

struct ProstheticForceReadings {
    double thumbForce;
    double indexForce;
    double middleForce;
    double ringForce;
    double pinkieForce;
    double palmForce;
};

class ProstheticSystemDynamics {
    inline static boost::circular_buffer<std::vector<double>> buf{512};
    static std::mutex mutex;

private:
    static torch::Tensor toTensor(boost::circular_buffer<std::vector<double>> data) {
        auto x_val = torch::zeros({1, 512, 12}, torch::TensorOptions().dtype(torch::kFloat32));
        for (int i = 0; i < 512; i++) {
            x_val.slice(1, i, i + 1).copy_(
                    torch::from_blob(data[i].data(), {12}, torch::TensorOptions().dtype(torch::kDouble)).to(
                            torch::kFloat32));
        }
        return x_val;
    }

public:
    static void addSample(const std::vector<double>& data) {
        std::lock_guard<std::mutex> lock(mutex);
        buf.push_back(data);
    }

    static std::optional<ProstheticServoPositions> calculateServoPositions(ProstheticServoPositions currentServoPositions) {
        boost::circular_buffer<std::vector<double>> data;

        {
            std::lock_guard<std::mutex> lock(mutex);
            if(buf.size() != buf.capacity()) {
                return std::nullopt;
            } else {
                data = boost::circular_buffer{buf};
            }
        }

        auto const data_tensor = toTensor(data);
        auto const y_pred = EMGSignalClassifier::classify(data_tensor);

        // TODO, use all this data to predict the position of the fingers

        return ProstheticServoPositions{};
    }
};