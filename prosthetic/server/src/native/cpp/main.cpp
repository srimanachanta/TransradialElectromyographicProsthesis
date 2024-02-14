#include "ThreadSafeCircularBuffer.h"
#include "EMGSensorArray.h"
#include "EMGSignalClassifier.h"
#include <chrono>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <torch/torch.h>

int main() {
    const auto BUFFER_SIZE_FRAMES = 512;
    const auto SAMPLING_RATE_SPS = 1000.0;
    const auto ITERATION_LENGTH_MICROS = boost::chrono::microseconds((long long) ((1.0 / SAMPLING_RATE_SPS) * 1e6));

    const auto SENSOR_GRID_A_ADDRESS = ""; // TODO
    const auto SENSOR_GRID_B_ADDRESS = ""; // TODO

    // EMGSensorArray sensor_grid_a(SENSOR_GRID_A_ADDRESS);
    // EMGSensorArray sensor_grid_b(SENSOR_GRID_B_ADDRESS);

    EMGSignalClassifier::load_model();

    ThreadSafeCircularBuffer<std::vector<double>> muscle_data(BUFFER_SIZE_FRAMES);

    boost::thread sensor_grid_thread{[&] {
        double i = 0.0;
        while (true) {
            auto start_time = boost::chrono::steady_clock::now();

            // auto f1 = sensor_grid_a.GetDataFrame();
            // auto f2 = sensor_grid_b.GetDataFrame();
            std::pair<long, std::vector<double>> f1 = {0, {i, i, i, i, i, i}};
            std::pair<long, std::vector<double>> f2 = {0, {i, i, i, i, i, i}};
            i++;

            std::vector<double> data_frame;
            data_frame.reserve(12);
            for (auto dp: f1.second) {
                data_frame.emplace_back(dp);
            }
            for (auto dp: f2.second) {
                data_frame.emplace_back(dp);
            }

            muscle_data.push_back(data_frame);

            auto end_time = boost::chrono::steady_clock::now();

            auto elapsed_time = boost::chrono::duration_cast<boost::chrono::microseconds>(end_time - start_time);
            auto remaining_time = boost::chrono::duration_cast<boost::chrono::microseconds>(
                    ITERATION_LENGTH_MICROS - elapsed_time);

            if (remaining_time.count() > 0) {
                boost::this_thread::sleep_for(remaining_time);
            }
        }
    }};

    boost::thread client_update_thread{[&] {
        while(true) {
            if(!muscle_data.is_full()) continue;

            auto emg_data = muscle_data.get_buffer();

            auto x_val = torch::zeros({512, 12});
            for(int i = 0; i < 512; i++) {
                x_val.slice(0, i) = torch::fill(torch::zeros({1, 12}), i);
            }


//            for (int i = 0; i < 512; i++)
//                x_val.slice(0, i,i+1) = torch::from_blob(emg_data[i].data(), {12}, torch::TensorOptions().dtype(torch::kFloat32));

            std::cout << x_val.slice(0, 1, 2) << std::endl;
        }
    }};

    sensor_grid_thread.join();
    client_update_thread.join();
}
