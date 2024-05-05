#pragma once

#include "Dataset.h"
#include "EMGDataProvider.h"

class ReplayEMGDataProvider final : EMGDataProvider {
 public:
  explicit ReplayEMGDataProvider(const std::string& dataset_name)
      : dataset(dataset_name){};

  torch::Tensor GetDataFrame() noexcept(false) override {
    if (index == -1) {
      lastTime = std::chrono::time_point_cast<std::chrono::microseconds>(
          std::chrono::steady_clock::now());
      index = 0;
      return dataset.get_item(0);
    }

    auto const currentTime =
        std::chrono::time_point_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now());
    auto const deltaTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(currentTime -
                                                              lastTime)
            .count();
    lastTime = currentTime;

    index += static_cast<uint16_t>(std::min(deltaTime, max_latency));
    if (index > dataset.size())
      throw DataNotAvailableException();

    return dataset.get_item(index);
  }

 private:
  uint16_t index = -1;
  static constexpr long max_latency = 20;
  Dataset dataset;
  std::chrono::steady_clock::time_point lastTime;
};
