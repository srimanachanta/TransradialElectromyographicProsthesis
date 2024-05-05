#include "native/include/DoFStateClassifier.h"

#include "Dataset.h"
#include "ModelConfig.h"
#include <fmt/format.h>


void sample_metrics_test() {
    DoFStateClassifier classifier{TRACED_FIST_MODEL_PATH, torch::kCPU};

    constexpr int num_samples = 1000;
    std::vector<double> latency_measurements;
    latency_measurements.reserve(num_samples);

    for (int i = 0; i < num_samples; i++) {
        auto start_time = std::chrono::steady_clock::now();

        auto x_data = torch::rand({1, 512, 12});
        auto pred = classifier.Classify(x_data);

        auto stop_time = std::chrono::steady_clock::now();

        std::chrono::duration<double, std::milli> elapsed_time_millis = stop_time - start_time;

        latency_measurements.emplace_back(elapsed_time_millis.count());
    }

    std::cout << "Num samples: " << num_samples << std::endl;
    double sum = std::accumulate(latency_measurements.begin(), latency_measurements.end(), 0.0);
    double mean = sum / static_cast<double>(latency_measurements.size());
    double variance = 0.0;
    for (const double x : latency_measurements) {
        variance += pow(x - mean, 2);
    }
    const double sd = sqrt(variance / static_cast<double>(latency_measurements.size()));

    const auto newEnd = std::remove_if(latency_measurements.begin(), latency_measurements.end(),
    [&](const double val) {
        return val < mean - 2 * sd || val > mean + 2 * sd;
    });

    latency_measurements.erase(newEnd, latency_measurements.end());

    std::cout << "Num samples Filtered: " << num_samples - latency_measurements.size() << std::endl;
    std::cout << "Min: " << *std::min_element(latency_measurements.begin(), latency_measurements.end()) << std::endl;
    std::cout << "Max: " << *std::max_element(latency_measurements.begin(), latency_measurements.end()) << std::endl;

    sum = std::accumulate(latency_measurements.begin(), latency_measurements.end(), 0.0);
    mean = sum / static_cast<double>(latency_measurements.size());
    std::cout << "Avg (ms): " << mean << std::endl;
}

void prosthetic_data_pipeline_test() {
    Dataset dataset{"fist"};
    // Dataset dataset{"solo_finger"};

    DoFStateClassifier classifier{TRACED_FIST_MODEL_PATH, torch::kCPU};
    // DoFStateClassifier classifier{TRACED_SOLO_FINGER_MODEL_PATH, torch::kCPU};

    for(int i = 0; i < dataset.size(); i+= 15) {
        const auto data = dataset.get_item(i);
        const auto pred_states = classifier.Classify(data);

        fmt::print("{}, {}\n", i, pred_states);
    }
}

int main() {
        // sample_metrics_test();
    prosthetic_data_pipeline_test();
}