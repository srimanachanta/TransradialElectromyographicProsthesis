#pragma once

#include <filesystem>
#include <fmt/core.h>
#include "DatasetConfig.h"
#include <torch/torch.h>


class Dataset {
public:
    explicit Dataset(const std::string& dataset_name) {
        const std::filesystem::path dataset_root_path(CLASSIFIER_DATASET_PATH);
        const std::filesystem::path dataset_subpath(dataset_name);
        const std::filesystem::path dataset_file_name("dataset.pt");
        const std::filesystem::path dataset_file_path = dataset_root_path / dataset_subpath / dataset_file_name;

        const torch::jit::script::Module dataset_container = torch::jit::load(dataset_file_path);

        this->x_data = dataset_container.attr("muscle_data_normalized").toTensor();
        this->y_data = dataset_container.attr("finger_state").toTensor().toType(c10::ScalarType::Int);
    }

    [[nodiscard]] size_t size() const {
        return x_data.sizes()[0] - window_size_samples + 1;
    }

    [[nodiscard]] torch::Tensor get_item(const int16_t idx) const {
        return x_data.slice(0, idx, idx + window_size_samples).reshape({1, 512, 12});
    }

private:
    static constexpr int window_size_samples = 512;
    torch::Tensor x_data;
    torch::Tensor y_data;
};
