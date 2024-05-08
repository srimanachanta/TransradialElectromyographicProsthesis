#pragma once

#include "Dataset.h"
#include "EMGDataProvider.h"
#include <algorithm>

class ReplayEMGDataProvider final : EMGDataProvider {
 public:
  explicit ReplayEMGDataProvider(const std::string& dataset_name)
      : dataset(dataset_name) {};

  bool HasData() override {
    return index < dataset.size();
  }

  torch::Tensor GetDataFrame() noexcept(false) override {
    if (first_time) {
      lastTime = std::chrono::time_point_cast<std::chrono::microseconds>(
          std::chrono::steady_clock::now());
      first_time = false;

      return dataset.get_item(0);
    }

    auto const currentTime =
        std::chrono::time_point_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now());
    auto const deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                              currentTime - lastTime)
                              .count();
    lastTime = currentTime;

    index += std::min(std::max(static_cast<uint16_t>(deltaTime), static_cast<uint16_t>(1)), max_latency);

    if(index > dataset.size()) {
      index = dataset.size();
      return dataset.get_item(dataset.size() - 1);
    }

    return dataset.get_item(index);
  }

 private:
  bool first_time = true;
  uint16_t index = 0;
  static constexpr uint16_t max_latency = 20;
  Dataset dataset;
  std::chrono::steady_clock::time_point lastTime;
};
