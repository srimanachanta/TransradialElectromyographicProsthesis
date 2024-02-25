#include "EMGSensorArray.h"
#include "EMGSignalClassifier.h"
#include "ProstheticServerSocket.h"
#include <boost/thread.hpp>
#include "SystemDynamics.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <chrono>

const auto SAMPLING_RATE_SPS = 1000.0;
const auto ITERATION_LENGTH_MICROS = boost::chrono::microseconds((long long) ((1.0 / SAMPLING_RATE_SPS) * 1e6));

void live_data_source() {
    const auto SENSOR_GRID_A_ADDRESS = "/dev/cu.usbmodem11101";
    const auto SENSOR_GRID_B_ADDRESS = "/dev/cu.usbmodem11201";

    EMGSensorArray sensor_grid_a(SENSOR_GRID_A_ADDRESS);
    EMGSensorArray sensor_grid_b(SENSOR_GRID_B_ADDRESS);

    sensor_grid_a.Enable();
    sensor_grid_b.Enable();

    long last_time = 0;
    while(true) {
         auto f1 = sensor_grid_a.GetDataFrame();
         auto f2 = sensor_grid_b.GetDataFrame();

         if(last_time == 0 || f1.deviceTimestampMicros - last_time >= ITERATION_LENGTH_MICROS.count()) {
            last_time = f1.deviceTimestampMicros;

            std::vector<double> data_frame;
            data_frame.reserve(12);
            for (auto dp: f1.deviceMeasurement) {
                data_frame.emplace_back(dp);
            }
            for (auto dp: f2.deviceMeasurement) {
                data_frame.emplace_back(dp);
            }

            ProstheticSystemDynamics::addSample(data_frame);
        }
    }
}

void replay_data_source() {
    std::string PROCEDURE_NAME = "fist";
    auto file_path = "/Users/srimanachanta/dev/Research/TransradialElectromyographicProsthesis/dataset/" + PROCEDURE_NAME + "/x.txt";

    std::chrono::milliseconds loop_dur(1);

    while(true) {
        std::ifstream file(file_path);
        ProstheticSystemDynamics::clearBuffer();

        std::string line;
        while(std::getline(file, line)) {
            auto const start_time = std::chrono::steady_clock::now();
            std::vector<double> data;
            std::stringstream ss(line);
            std::string token;

            while (std::getline(ss, token, ',')) {
                if (!token.empty()) {
                    data.push_back(std::stod(token));
                }
            }

            ProstheticSystemDynamics::addSample(data);
            auto const stop_time = std::chrono::steady_clock::now();

            auto const elapsed_time = stop_time - start_time;

            std::this_thread::sleep_for(loop_dur - elapsed_time);
        }
    }
}

int main() {
    EMGSignalClassifier::load_model();

    boost::thread sensor_grid_thread{&replay_data_source};
    sensor_grid_thread.detach();

    ProstheticServerSocket serverSocket("192.168.1.155", 8080);
}
