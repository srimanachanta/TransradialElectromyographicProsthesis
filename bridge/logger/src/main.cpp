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

    auto SAMPLING_RATE_SPS = 1000.0;
    auto WINDOW_SIZE_MICROS = (long) ((1.0/SAMPLING_RATE_SPS) * 1000000) - 100;

    auto MEASURE_TIME_SEC = 6.0;
    auto MEASURE_TIME_MICROS = (long) MEASURE_TIME_SEC * 1000000;

    auto SAMPLING_PERIOD_DURATION = std::chrono::duration_cast<std::chrono::microseconds>(1.0s / SAMPLING_RATE_SPS);
    std::string DATA_DIR_PATH = "/Users/srimanachanta/dev/Research/TransradialElectromyographicProsthesis/bridge/logger/data";
    std::string OUTFILE_PATH = DATA_DIR_PATH + "/pickup_ball/x.txt";

    std::atomic_bool v = true;

    std::thread t1([&] {
        auto SENSOR_A_ADDRESS = "/dev/cu.usbmodem1101";
        auto SENSOR_B_ADDRESS = "/dev/cu.usbmodem1201";

        Sensor first_sensor(SENSOR_A_ADDRESS);
        Sensor second_sensor(SENSOR_B_ADDRESS);

        std::ofstream outfile;
        outfile.clear();
        outfile.open(OUTFILE_PATH);

        first_sensor.Enable();
        second_sensor.Enable();

        int count = 0;
        long last_time = 0;
        while(v.load()) {
            auto f1 = first_sensor.GetDataFrame();
            auto f2 = second_sensor.GetDataFrame();

            if(f1.first > MEASURE_TIME_MICROS) {
                break;
            } else if(last_time == 0 || f1.first - last_time >= WINDOW_SIZE_MICROS) {
                count++;
                last_time = f1.first;

                for(int i = 0; i < 12; i++) {
                    if(i < 6) {
                        outfile << f1.second[i];
                    } else {
                        outfile << f2.second[i - 6];
                    }
                }
                outfile << std::endl;
            }
        }

        first_sensor.Disable();
        second_sensor.Disable();

        outfile.close();

        std::cout << "Saved the following # of samples: " << count << std::endl;
    });

//    std::cin.get();
//
//    v.store(false);

    t1.join();
}
