#include "Prosthetic.h"
#include "classifier/NativeMLEMGDataClassifier.h"
#include "provider/ReplayEMGDataProvider.h"
#include "ModelConfig.h"

int main() {
  ReplayEMGDataProvider provider{"fist"};
  // ReplayEMGDataProvider provider{"solo_finger"};

  NativeMLEMGDataClassifier classifier{TRACED_FIST_MODEL_PATH};
  // NativeMLEMGDataClassifier classifier{TRACED_SOLO_FINGER_MODEL_PATH};

  Prosthetic prosthetic{{0, M_PI_2, M_PI_2, 0, M_PI_2, M_PI_2, 0, M_PI_2,
                         M_PI_2, 0, M_PI_2, M_PI_2, 0, M_PI_2, M_PI_2, M_PI_2}};

  while (provider.HasData()) {
    const torch::Tensor emg_data = provider.GetDataFrame();

    // Determine per-finger direction
    auto const state = classifier.Classify(emg_data);

    // If a finger is extending, set its goal angle to 0°
    // If a finger is flexing, set its goal angle to 90°
    // If a finger isn't moving, set its goal angle to it's current actual angle
    Prosthetic::JointPositions goalPositions{};
    auto const currentPositions = prosthetic.GetCurrentBufferedPositions();

    if (state.thumb == kFlexing) {
      goalPositions.thumbMetacarpalPitchAngleRad = M_PI_2;
      goalPositions.thumbProximalPitchAngleRad = M_PI_2;
    } else if (state.thumb == kExtending) {
      goalPositions.thumbMetacarpalPitchAngleRad = 0;
      goalPositions.thumbProximalPitchAngleRad = 0;
    } else {
      goalPositions.thumbMetacarpalPitchAngleRad =
          currentPositions.thumbMetacarpalPitchAngleRad;
      goalPositions.thumbProximalPitchAngleRad =
          currentPositions.thumbProximalPitchAngleRad;
    }

    if (state.index == kFlexing) {
      goalPositions.indexMetacarpalPitchAngleRad = M_PI_2;
      goalPositions.indexProximalPitchAngleRad = M_PI_2;
    } else if (state.index == kExtending) {
      goalPositions.indexMetacarpalPitchAngleRad = 0;
      goalPositions.indexProximalPitchAngleRad = 0;
    } else {
      goalPositions.indexMetacarpalPitchAngleRad =
          currentPositions.indexMetacarpalPitchAngleRad;
      goalPositions.indexProximalPitchAngleRad =
          currentPositions.indexProximalPitchAngleRad;
    }

    if (state.middle == kFlexing) {
      goalPositions.middleMetacarpalPitchAngleRad = M_PI_2;
      goalPositions.middleProximalPitchAngleRad = M_PI_2;
    } else if (state.middle == kExtending) {
      goalPositions.middleMetacarpalPitchAngleRad = 0;
      goalPositions.middleProximalPitchAngleRad = 0;
    } else {
      goalPositions.middleMetacarpalPitchAngleRad =
          currentPositions.middleMetacarpalPitchAngleRad;
      goalPositions.middleProximalPitchAngleRad =
          currentPositions.middleProximalPitchAngleRad;
    }

    if (state.ring == kFlexing) {
      goalPositions.ringMetacarpalPitchAngleRad = M_PI_2;
      goalPositions.ringProximalPitchAngleRad = M_PI_2;
    } else if (state.ring == kExtending) {
      goalPositions.ringMetacarpalPitchAngleRad = 0;
      goalPositions.ringProximalPitchAngleRad = 0;
    } else {
      goalPositions.ringMetacarpalPitchAngleRad =
          currentPositions.ringMetacarpalPitchAngleRad;
      goalPositions.ringProximalPitchAngleRad =
          currentPositions.ringProximalPitchAngleRad;
    }

    if (state.little == kFlexing) {
      goalPositions.littleMetacarpalPitchAngleRad = M_PI_2;
      goalPositions.littleProximalPitchAngleRad = M_PI_2;
    } else if (state.little == kExtending) {
      goalPositions.littleMetacarpalPitchAngleRad = 0;
      goalPositions.littleProximalPitchAngleRad = 0;
    } else {
      goalPositions.littleMetacarpalPitchAngleRad =
          currentPositions.littleMetacarpalPitchAngleRad;
      goalPositions.littleProximalPitchAngleRad =
          currentPositions.littleProximalPitchAngleRad;
    }

    prosthetic.SetJointGoalPositions(goalPositions);
  }

  prosthetic.Stop();
}
