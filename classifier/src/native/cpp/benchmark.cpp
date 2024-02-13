#include "EMGSignalClassifier.h"

#include <iostream>
#include <chrono>

int main() {
    EMGSignalClassifier::load_model();

    int num_samples = 1000;
    std::vector<double> samples;
    samples.reserve(num_samples);

    for (int i = 0; i < num_samples; i++) {
        auto start_time = std::chrono::steady_clock::now();

        auto x_data = torch::rand({1, 512, 12});
        auto pred = EMGSignalClassifier::classify(x_data);

        auto stop_time = std::chrono::steady_clock::now();

        std::chrono::duration<double, std::milli> elapsed_time_millis = stop_time - start_time;

        samples.emplace_back(elapsed_time_millis.count());
    }

    std::cout << "Num samples: " << num_samples << std::endl;
    std::cout << "Min: " << *std::min_element(samples.begin(), samples.end()) << std::endl;
    std::cout << "Max: " << *std::max_element(samples.begin(), samples.end()) << std::endl;

    double sum = 0;
    int c = 0;
    for (double sample: samples) {
        sum += sample;

        if (sample >= 10.0) c++;
    }
    double average = sum / static_cast<double>(samples.size());
    std::cout << "Avg (ms): " << average << std::endl;
    std::cout << "Num samples longer than 10ms: " << c << std::endl;
}
