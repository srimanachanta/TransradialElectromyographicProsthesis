#include "EMGSensorArray.h"
#include "EMGSignalClassifier.h"
#include "ProstheticServerSocket.h"
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include "ProstheticServerSocket.h"
#include "SystemDynamics.h"

const auto SAMPLING_RATE_SPS = 1000.0;
const auto ITERATION_LENGTH_MICROS = boost::chrono::microseconds((long long) ((1.0 / SAMPLING_RATE_SPS) * 1e6));

const auto SENSOR_GRID_A_ADDRESS = "/dev/cu.usbmodem11101";
const auto SENSOR_GRID_B_ADDRESS = "/dev/cu.usbmodem11201";

//EMGSensorArray sensor_grid_a(SENSOR_GRID_A_ADDRESS);
//EMGSensorArray sensor_grid_b(SENSOR_GRID_B_ADDRESS);

void data_collection_handler() {
//    sensor_grid_a.Enable();
//    sensor_grid_b.Enable();
//
//    long last_time = 0;
//    while(true) {
//         auto f1 = sensor_grid_a.GetDataFrame();
//         auto f2 = sensor_grid_b.GetDataFrame();
//
//         if(last_time == 0 || f1.deviceTimestampMicros - last_time >= ITERATION_LENGTH_MICROS.count()) {
//            last_time = f1.deviceTimestampMicros;
//
//            std::vector<double> data_frame;
//            data_frame.reserve(12);
//            for (auto dp: f1.deviceMeasurement) {
//                data_frame.emplace_back(dp);
//            }
//            for (auto dp: f2.deviceMeasurement) {
//                data_frame.emplace_back(dp);
//            }
//
//            ProstheticSystemDynamics::addSample(data_frame);
//        }
//    }

    while(true) {
        std::vector<double> data = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        ProstheticSystemDynamics::addSample(data);
    }
}

int main() {
    EMGSignalClassifier::load_model();

    boost::thread sensor_grid_thread{&data_collection_handler};
    sensor_grid_thread.detach();

    ProstheticServerSocket serverSocket("192.168.1.155", 8080);
}
