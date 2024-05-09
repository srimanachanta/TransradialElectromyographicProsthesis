#pragma once

#include <torch/script.h>
#include <fmt/core.h>

/**
 * Class references of the classifier. Represents the muscular state of each
 * major DoF of the hand.
 */
enum DoFForce { kNoMovement, kExtending, kFlexing };

struct DoFStateClassification {
  DoFForce thumb;
  DoFForce index;
  DoFForce middle;
  DoFForce ring;
  DoFForce little;
  DoFForce wrist;

  bool operator==(const DoFStateClassification& other) const {
    return this->thumb == other.thumb && this->index == other.index &&
           this->middle == other.middle && this->ring == other.ring &&
           this->little == other.little && this->wrist == other.wrist;
  }

  bool operator!=(const DoFStateClassification& other) const {
    return this->thumb != other.thumb || this->index != other.index ||
           this->middle != other.middle || this->ring != other.ring ||
           this->little != other.little || this->wrist != other.wrist;
  }
};

template <>
struct fmt::formatter<DoFForce> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const DoFForce& f, FormatContext& ctx) {
    switch (f) {
      case kNoMovement:
        return format_to(ctx.out(), "NoMovement");
      case kExtending:
        return format_to(ctx.out(), "Extending");
      case kFlexing:
        return format_to(ctx.out(), "Flexing");
    }
    return format_to(ctx.out(), "Unknown");
  }
};

template <>
struct fmt::formatter<DoFStateClassification> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const DoFStateClassification& d, FormatContext& ctx) {
    return format_to(
        ctx.out(),
        "{{thumb: {}, index: {}, middle: {}, ring: {}, little: {}, wrist: {}}}",
        d.thumb, d.index, d.middle, d.ring, d.little, d.wrist);
  }
};

/**
 * Wrapped class around a TorchScript model for classifying the force output of
 * the hand based on the last 512 samples collected from the bridge. Samples
 * should be collected and fed in a uniform fashion using the classify method
 * call and measured at a sampling rate of ~1 ksps.
 */
class DoFStateClassifier {
 public:
  explicit DoFStateClassifier(const std::string&, torch::DeviceType deviceType);

  /**
   * Determine the finger muscular force state of each DoF based on measured
   * motor units collected over a uniform time period (1 ksps). Calling this
   * method with non-uniform time periods will significantly reduce the accuracy
   * of predictions. Data should be Float32.
   *
   * @param data
   * @return Returns the predicted classes of the DoFs based on measured motor
   * units.
   */
  DoFStateClassification Classify(const torch::Tensor& data);

 private:
  inline static torch::jit::script::Module module;
  torch::DeviceType device_type;
};
