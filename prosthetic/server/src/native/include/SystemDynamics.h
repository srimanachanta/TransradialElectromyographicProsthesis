#pragma once

#include <torch/torch.h>
#include <boost/circular_buffer.hpp>
#include <mutex>
#include <cmath>
#include <algorithm>

// yaw angles are relative to the dorsal view of the hand with the x-axis pointing towards the ulnar side of the hand
// joint pitch angles are relative to the plane parallel to each joint with respect to its connecting joint
struct ProstheticJointPositions {
    double thumbYawAngleRad;
    double thumbProximalPitchAngleRad;
    double thumbDistalPitchAngleRad;

    double littleYawAngleRad;
    double littleMetacarpalPitchAngleRad;
    double littleProximalPitchAngleRad;

    double ringYawAngleRad;
    double ringMetacarpalPitchAngleRad;
    double ringProximalPitchAngleRad;

    double middleYawAngleRad;
    double middleMetacarpalPitchAngleRad;
    double middleProximalPitchAngleRad;

    double indexYawAngleRad;
    double indexMetacarpalPitchAngleRad;
    double indexProximalPitchAngleRad;

    double wristPitchAngleRad;
};

// Order of struct fields corresponds to the port of servo in the prosthetic
struct ProstheticServoPositions {
    unsigned short thumbProximalLeftDegrees;
    unsigned short thumbProximalRightDegrees;
    unsigned short thumbDistalDegrees;

    unsigned short indexMetacarpalLeftDegrees;
    unsigned short indexMetacarpalRightDegrees;
    unsigned short indexProximalDegrees;

    unsigned short middleMetacarpalLeftDegrees;
    unsigned short middleMetacarpalRightDegrees;
    unsigned short middleProximalDegrees;

    unsigned short ringMetacarpalLeftDegrees;
    unsigned short ringMetacarpalRightDegrees;
    unsigned short ringProximalDegrees;

    unsigned short pinkieMetacarpalLeftDegrees;
    unsigned short pinkieMetacarpalRightDegrees;
    unsigned short pinkieProximalDegrees;

    unsigned short wristDegrees;
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
    inline static std::vector<double> perChannelMean{0.20224797073458817, 0.09448141646646423, 0.2338321342966314,
                                                     0.12366670452015649, 0.2488609666776952, 0.2586317181999994,
                                                     0.22002809853805116, 0.17676544703607844, 0.1837980856259647,
                                                     0.3094310422536541, 0.1074674631130109, 0.2691611912858288};
    inline static std::vector<double> perChannelStdevs{0.0502805180743739, 0.03862383316240277, 0.10752486691075412,
                                                       0.05098862737275849, 0.2072354346232382, 0.1707497216652996,
                                                       0.2057644291692141, 0.19113279789503274, 0.055193767793196055,
                                                       0.1404278665063978, 0.09306863390873057, 0.1436649391569654};

    inline static double fingerRateRadPerSecond = M_PI_4;
    inline static double wristRateRadPerSecond = M_PI / 6.0;

    inline static boost::circular_buffer<std::vector<double>> buf{512};
    inline static std::mutex mutex_;

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

    static long clamp(long value, long min, long max) {
        return std::max(min, std::min(value, max));
    }

    static double clamp(double value, double min, double max) {
        return std::max(min, std::min(value, max));
    }


    // Use high order inverse kinematics to derive servo positions from the joint positions
    static ProstheticServoPositions deriveServoPositionsFromJointPositions(ProstheticJointPositions jointPositions) {
        ProstheticServoPositions positions{};

        positions.pinkieProximalDegrees =
                180.0 - ((clamp(jointPositions.littleProximalPitchAngleRad, 0, M_PI_2) / M_PI_2) * 180.0);
        positions.ringProximalDegrees =
                180.0 - ((clamp(jointPositions.ringProximalPitchAngleRad, 0, M_PI_2) / M_PI_2) * 180.0);
        positions.middleProximalDegrees =
                (clamp(jointPositions.middleProximalPitchAngleRad, 0, M_PI_2) / M_PI_2) * 180.0;
        positions.indexProximalDegrees = (clamp(jointPositions.indexProximalPitchAngleRad, 0, M_PI_2) / M_PI_2) * 180.0;


        if (jointPositions.littleMetacarpalPitchAngleRad >= M_PI / 3.0 ||
            abs(jointPositions.littleYawAngleRad) < M_PI / 36) {
            double littleMetacarpalRotations = jointPositions.littleMetacarpalPitchAngleRad / M_PI_2;
            positions.pinkieMetacarpalLeftDegrees = 90.0 + (90.0 * littleMetacarpalRotations);
            positions.pinkieMetacarpalRightDegrees = 90.0 - (90.0 * littleMetacarpalRotations);
        } else {
            // Valid Yaw Angles are present
            // TODO
        }

        if (jointPositions.ringMetacarpalPitchAngleRad >= M_PI / 3.0 ||
            abs(jointPositions.ringYawAngleRad) < M_PI / 36) {
            double ringMetacarpalRotations = jointPositions.ringMetacarpalPitchAngleRad / M_PI_2;
            positions.ringMetacarpalLeftDegrees = 90.0 + (90.0 * ringMetacarpalRotations);
            positions.ringMetacarpalRightDegrees = 90.0 - (90.0 * ringMetacarpalRotations);
        } else {
            // Valid Yaw Angles are present
            // TODO
        }

        if (jointPositions.middleMetacarpalPitchAngleRad >= M_PI / 3.0 ||
            abs(jointPositions.middleYawAngleRad) < M_PI / 36) {
            double middleMetacarpalRotations = jointPositions.middleMetacarpalPitchAngleRad / M_PI_2;
            positions.middleMetacarpalLeftDegrees = 90.0 + (90.0 * middleMetacarpalRotations);
            positions.middleMetacarpalRightDegrees = 90.0 - (90.0 * middleMetacarpalRotations);
        } else {
            // Valid Yaw Angles are present
            // TODO
        }

        if (jointPositions.indexMetacarpalPitchAngleRad >= M_PI / 3.0 ||
            abs(jointPositions.indexYawAngleRad) < M_PI / 36) {
            double indexMetacarpalRotations = jointPositions.indexMetacarpalPitchAngleRad / M_PI_2;
            positions.indexMetacarpalLeftDegrees = 90.0 + (90.0 * indexMetacarpalRotations);
            positions.indexMetacarpalRightDegrees = 90.0 - (90.0 * indexMetacarpalRotations);
        } else {
            // Valid Yaw Angles are present
            // TODO
        }

        // TODO handle the thumb

        return positions;
    }

    static ProstheticJointPositions deriveJointPositionsFromServoPositions(ProstheticServoPositions servoPositions) {
        ProstheticJointPositions positions{};

        positions.littleProximalPitchAngleRad = (180 - servoPositions.pinkieProximalDegrees) / 180 * M_PI_2;
        positions.ringProximalPitchAngleRad = (180 - servoPositions.ringProximalDegrees) / 180 * M_PI_2;
        positions.middleProximalPitchAngleRad = servoPositions.middleProximalDegrees / 180 * M_PI_2;
        positions.indexProximalPitchAngleRad = servoPositions.indexProximalDegrees / 180 * M_PI_2;

        // TODO because we aren't using yaw, only use left angle cause both are equally scaled
        double littleMetacarpalRotations = (90.0 - servoPositions.pinkieMetacarpalLeftDegrees) / 90.0;
        positions.littleMetacarpalPitchAngleRad = M_PI_2 * littleMetacarpalRotations;
        positions.littleYawAngleRad = 0.0; // Assuming no yaw angle for now

        double ringMetacarpalRotations = (90.0 - servoPositions.ringMetacarpalLeftDegrees) / 90.0;
        positions.ringMetacarpalPitchAngleRad = M_PI_2 * ringMetacarpalRotations;
        positions.ringYawAngleRad = 0.0; // Assuming no yaw angle for now

        double middleMetacarpalRotations = (90.0 - servoPositions.middleMetacarpalLeftDegrees) / 90.0;
        positions.middleMetacarpalPitchAngleRad = M_PI_2 * middleMetacarpalRotations;
        positions.middleYawAngleRad = 0.0; // Assuming no yaw angle for now

        double indexMetacarpalRotations = (90.0 - servoPositions.indexMetacarpalLeftDegrees) / 90.0;
        positions.indexMetacarpalPitchAngleRad = M_PI_2 * indexMetacarpalRotations;
        positions.indexYawAngleRad = 0.0; // Assuming no yaw angle for now

        // TODO handle the thumb

        return positions;
    }


public:
    static void addSample(std::vector<double> data) {
        // Apple z-score normalization to data
        for (int i = 0; i < 12; i++) {
            data[i] = (data[i] - perChannelMean[i]) / perChannelStdevs[i];
        }
        std::lock_guard<std::mutex> lock(mutex_);
        buf.push_back(data);
    }

    static void clearBuffer() {
        std::lock_guard<std::mutex> lock(mutex_);
        buf.clear();
    }

    static std::optional<ProstheticServoPositions>
    calculateServoPositions(ProstheticServoPositions currentServoPositions) {
        boost::circular_buffer<std::vector<double>> data;

        {
            std::lock_guard<std::mutex> lock(mutex_);
            // Make sure there are enough samples to feed the model, if not reject
            if (buf.size() != buf.capacity()) {
                return std::nullopt;
            } else {
                data = boost::circular_buffer{buf};
            }
        }

        auto const data_tensor = toTensor(data);
        auto const y_pred = EMGSignalClassifier::classify(data_tensor);

        std::cout << y_pred << std::endl;

        auto const current_finger_positions = deriveJointPositionsFromServoPositions(currentServoPositions);

        // 1. use the y-pred value, actual muscle values, and current finger positions to determine the true finger state (motion) and a scalar value of the acceleration of the finger to that position
        // Get the mean of the last 50 samples;
        auto const mean = data_tensor.slice(1, 512 - 50, 512).mean(1);

        std::vector<DoFForce> true_pred;
        true_pred.reserve(6);
        for (int i = 0; i < 6; i++) {
            switch (y_pred[i]) {
                DoFForce res;
                case kExtending:
                    switch (i) {
                        case 0:
                            // Extensor Pollicis Longus (idx 8)
                            // Extensor Pollicis Brevis (idx 9)

                            res = mean[1][7][1].item<double>() + 0.8109 >= 0 ? kExtending : kNoMovement;
                            break;
                        case 1:
                            // Extensor Indicis Proprius (idx 7)
                            // Extensor Digitorum Communis (idx 5)

                            res = mean[1][6][1].item<double>() + 0.997 >= 0 ? kExtending : kNoMovement;
                            break;
                        case 2:
                        case 3:
                            // Extensor Digitorum Communis (idx 5)
                            res = mean[1][4][1].item<double>() + 0.9297 >= 0 ? kExtending : kNoMovement;
                            break;
                        case 4:
                            // Extensor Digiti Minimi (idx 6)
                            // Extensor Digitorum Communis (idx 5)
                            res = mean[1][5][1].item<double>() - 0.1804 >= 0 ? kExtending : kNoMovement;

                            break;
                        case 5:
                            // Extensor Carpi Ulnaris (idx 10)
                            // Extensor Carpi Radialis Brevis (idx 11)
                            // Extensor Carpi Radialis Longus (idx 12)

                            // Just use model prediction for wrist
                            res = y_pred[i];
                            break;
                    }
                case kFlexing:
                    switch (i) {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                            // Flexor Digitorum Superficialis (idx 1)
                            // Flexor Digitorum Profundus (idx 4)

                            // b + mean

                            // Priority on the profundus
                            // TODO
                            break;
                        case 4:
                            // Flexor Pollicis Longus (idx 2)
                            // TODO
                            break;
                        case 5:
                            // Flexor Carpi Ulnaris (idx 3)

                            // Just use model prediction for wrist
                            res = y_pred[i];
                            break;
                    }

                    true_pred[i] = res;
            }
        }

        // 2. using the predicted state and acceleration, determine the predicted finger position
        ProstheticJointPositions jointPositions{};

        // EMG Predictor doesn't estimate yaw angles, set those to zero
        jointPositions.thumbYawAngleRad = 0;
        jointPositions.indexYawAngleRad = 0;
        jointPositions.middleYawAngleRad = 0;
        jointPositions.ringYawAngleRad = 0;
        jointPositions.littleYawAngleRad = 0;

        // Calculate the new finger positions
        jointPositions.thumbProximalPitchAngleRad = fingerRateRadPerSecond * 0; // TODO
        jointPositions.indexMetacarpalPitchAngleRad = fingerRateRadPerSecond * 0; // TODO
        jointPositions.middleMetacarpalPitchAngleRad = fingerRateRadPerSecond * 0; // TODO
        jointPositions.ringMetacarpalPitchAngleRad = fingerRateRadPerSecond * 0; // TODO
        jointPositions.littleMetacarpalPitchAngleRad = fingerRateRadPerSecond * 0; // TODO
        jointPositions.wristPitchAngleRad = wristRateRadPerSecond * 0; // TODO

        // Distal joints are functions of proximal joints. In most cases, the same, approximate as such
        // TODO, validate that this approximation is valid
        jointPositions.thumbDistalPitchAngleRad = jointPositions.thumbProximalPitchAngleRad;
        jointPositions.indexProximalPitchAngleRad = jointPositions.indexMetacarpalPitchAngleRad;
        jointPositions.middleProximalPitchAngleRad = jointPositions.middleMetacarpalPitchAngleRad;
        jointPositions.ringProximalPitchAngleRad = jointPositions.ringMetacarpalPitchAngleRad;
        jointPositions.littleProximalPitchAngleRad = jointPositions.littleMetacarpalPitchAngleRad;

        return deriveServoPositionsFromJointPositions(jointPositions);
    }
};