#include "EMGSensorArray.h"
#include <iostream>
#include <thread>
#include <fstream>

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

// "pickup_ball_palm"
// "throw_ball_palm"
// "piano_twinkle"


int main() {
    using namespace std::chrono_literals;

    auto SAMPLING_RATE_SPS = 1024.0;
    auto WINDOW_SIZE_MICROS = (long) ((1.0/SAMPLING_RATE_SPS) * 1000000) - 80;

    auto MEASURE_TIME_SEC = 10.0;
    auto MEASURE_TIME_MICROS = (long) MEASURE_TIME_SEC * 1000000;

    auto SAMPLING_PERIOD_DURATION = std::chrono::duration_cast<std::chrono::microseconds>(1.0s / SAMPLING_RATE_SPS);
    std::string DATA_DIR_PATH = "/Users/srimanachanta/dev/Research/TransradialElectromyographicProsthesis/dataset";
    std::string PROCEDURE_NAME = "throw_ball_palm";
    std::string OUTFILE_PATH = DATA_DIR_PATH + "/" + PROCEDURE_NAME + "/x.txt";

    auto SENSOR_A_ADDRESS = "/dev/cu.usbmodem11101";
    auto SENSOR_B_ADDRESS = "/dev/cu.usbmodem11201";

    EMGSensorArray first_sensor(SENSOR_A_ADDRESS);
    EMGSensorArray second_sensor(SENSOR_B_ADDRESS);

    // Used for Debugging EMGSensorArray Placement
    // first_sensor.Enable();
    // second_sensor.Enable();
    // return 0;

    std::atomic_bool v = true;

    std::this_thread::sleep_for(1s);

    std::thread t1([&] {
        std::cout << "Starting" << std::endl;

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

            if(f1.deviceTimestampMicros > MEASURE_TIME_MICROS) {
                break;
            } else if(last_time == 0 || f1.deviceTimestampMicros - last_time >= WINDOW_SIZE_MICROS) {
                count++;
                last_time = f1.deviceTimestampMicros;

                for(int i = 0; i < 12; i++) {
                    if(i < 6) {
                        outfile << f1.deviceMeasurement[i];
                    } else {
                        outfile << f2.deviceMeasurement[i - 6];
                    }
                    outfile << ",";
                }
                outfile << std::endl;
            }
        }

        first_sensor.Disable();
        second_sensor.Disable();

        outfile.close();

        std::cout << "Saved the following # of samples: " << count << std::endl;
    });

    t1.join();
}
