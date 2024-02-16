#include "ThreadSafeCircularBuffer.h"
#include "EMGSensorArray.h"
#include "EMGSignalClassifier.h"
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include "ServerProstheticSocket.h"

//const auto BUFFER_SIZE_FRAMES = 512;
//const auto SAMPLING_RATE_SPS = 1000.0;
//const auto ITERATION_LENGTH_MICROS = boost::chrono::microseconds((long long) ((1.0 / SAMPLING_RATE_SPS) * 1e6));
//
//const auto SENSOR_GRID_A_ADDRESS = "/dev/cu.usbmodem11101";
//const auto SENSOR_GRID_B_ADDRESS = "/dev/cu.usbmodem11201";
//
////EMGSensorArray sensor_grid_a(SENSOR_GRID_A_ADDRESS);
////EMGSensorArray sensor_grid_b(SENSOR_GRID_B_ADDRESS);
//
//ThreadSafeCircularBuffer<std::vector<double>> muscle_data(BUFFER_SIZE_FRAMES);
//
//void data_collection_handler() {
////        sensor_grid_a.Enable();
////        sensor_grid_b.Enable();
////
////        long last_time = 0;
////        while(true) {
////             auto f1 = sensor_grid_a.GetDataFrame();
////             auto f2 = sensor_grid_b.GetDataFrame();
////
////             if(last_time == 0 || f1.deviceTimestampMicros - last_time >= ITERATION_LENGTH_MICROS.count()) {
////                last_time = f1.deviceTimestampMicros;
////
////                std::vector<double> data_frame;
////                data_frame.reserve(12);
////                for (auto dp: f1.deviceMeasurement) {
////                    data_frame.emplace_back(dp);
////                }
////                for (auto dp: f2.deviceMeasurement) {
////                    data_frame.emplace_back(dp);
////                }
////
////                muscle_data.push_back(data_frame);
////            }
////        }
//
//    while(true) {
//        std::vector<double> data = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
//        muscle_data.push_back(data);
//    }
//}
//
//std::optional<ProstheticServoPositions> get_servo_positions() {
//    if(!muscle_data.is_full()) return std::nullopt;
//
//    auto emg_data = muscle_data.get_buffer();
//
//    // Convert data to Tensor
//    auto x_val = torch::zeros({1, 512, 12}, torch::TensorOptions().dtype(torch::kFloat32));
//    for (int i = 0; i < 512; i++) {
//        x_val.slice(1, i, i + 1).copy_(
//                torch::from_blob(emg_data[i].data(), {12}, torch::TensorOptions().dtype(torch::kDouble)).to(
//                        torch::kFloat32));
//    }
//
//    auto pred = EMGSignalClassifier::classify(x_val);
//
//    // TODO, use pred and EMG readings to determine servo positions
//    ProstheticServoPositions out{};
//
//    return out;
//}
//
//void websocket_client_handler() {
//
//}

int main() {
//    EMGSignalClassifier::load_model();

//    boost::thread sensor_grid_thread{&data_collection_handler};
//    sensor_grid_thread.detach();

    ServerProstheticSocket serverSocket("192.168.1.155", 8080);
}
