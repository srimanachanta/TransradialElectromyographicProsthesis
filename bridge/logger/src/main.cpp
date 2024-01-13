#include "Sensor.h"
#include <iostream>
#include <thread>
#include <fstream>

// "pickup_ball"
// "pickup_ball_palm"
// "throw_ball"
// "throw_ball_palm"
// "piano"

// "solo_finger"
// "thumb_duo"
// "index_duo"
// "middle_duo"
// "pinkie_duo"
// "solo_finger_press"
// "thumb_duo_press"
// "index_duo_press"
// "middle_duo_press"
// "ring_duo_press"
// "pinkie_duo_press"

// "wrist"
// "fist"

int main() {
    using namespace std::chrono_literals;

    auto SAMPLING_RATE_SPS = 500.0;
    auto SAMPLING_PERIOD_DURATION = std::chrono::duration_cast<std::chrono::microseconds>(1.0s / SAMPLING_RATE_SPS);
    std::string DATA_DIR_PATH = "/Users/srimanachanta/dev/Research/TransradialElectromyographicProsthesis/bridge/logger/data";
    std::string OUTFILE_PATH = DATA_DIR_PATH + "/pickup_ball/x.txt";

    std::atomic_bool v = true;

    std::thread t1([&] {
        auto SENSOR_A_ADDRESS = "/dev/cu.usbmodem1101";
        auto SENSOR_B_ADDRESS = "/dev/cu.usbmodem1201";

        Sensor first_sensor(SENSOR_A_ADDRESS);
        Sensor second_sensor(SENSOR_B_ADDRESS);

        first_sensor.Enable();
        second_sensor.Enable();

//        std::ofstream outfile;
//        outfile.clear();
//        outfile.open(OUTFILE_PATH);

        std::vector<std::vector<double>> data;

        while(v.load()) {
            auto start_time = std::chrono::steady_clock::now();

            auto f1 = first_sensor.GetDataFrame();
            auto f2 = second_sensor.GetDataFrame();

//            for(int i = 0; i < 12; i ++) {
//                if(i < 6) {
//                    outfile << f1.second[i];
//                } else {
//                    outfile << f2.second[i - 6];
//                }
//                outfile << ",";
//            }
//            outfile << std::endl;

//            auto out = std::vector<double>();
//            for(int i = 0; i < 12; i ++) {
//               if(i < 6) {
//                    out[i] = f1.second[i];
//                } else {
//                   out[i] = f2.second[i - 6];
//                }
//            }
//
            data.push_back({0.0, 0.0});

            auto end_time = std::chrono::steady_clock::now();
            auto elapsed = end_time-start_time;
            auto remaining = std::chrono::duration_cast<std::chrono::microseconds>( SAMPLING_PERIOD_DURATION - elapsed);

            if(remaining.count() > 0) {
                std::this_thread::sleep_for(remaining);
            } else {
                std::cout << "Loop Overrun" << std::endl;
            }
        }

        std::cout << data.size() << std::endl;

        first_sensor.Disable();
        second_sensor.Disable();

//        outfile.close();
    });

    std::this_thread::sleep_for(3s);

//    std::cin.get();

    v.store(false);

    t1.join();
}
