#pragma once

#include "ServoBonnet.h"
#include <mutex>
#include <thread>
#include "TrapezoidalMotionProfile.h"
#include <cmath>
#include <atomic>

class Prosthetic {
 public:
  struct JointPositions {
    double thumbMetacarpalPitchAngleRad;
    double thumbProximalPitchAngleRad;

    double littleMetacarpalYawAngleRad;
    double littleMetacarpalPitchAngleRad;
    double littleProximalPitchAngleRad;

    double ringMetacarpalYawAngleRad;
    double ringMetacarpalPitchAngleRad;
    double ringProximalPitchAngleRad;

    double middleMetacarpalYawAngleRad;
    double middleMetacarpalPitchAngleRad;
    double middleProximalPitchAngleRad;

    double indexMetacarpalYawAngleRad;
    double indexMetacarpalPitchAngleRad;
    double indexProximalPitchAngleRad;

    double wristPitchAngleRad;
  };

  struct ServoPositions {
    double thumbProximalPitchRad;
    double thumbMetacarpalPitchRad;

    double indexMetacarpalLeftRad;
    double indexMetacarpalRightRad;
    double indexPitchRad;

    double middleMetacarpalLeftRad;
    double middleMetacarpalRightRad;
    double middlePitchRad;

    double ringMetacarpalLeftRad;
    double ringMetacarpalRightRad;
    double ringPitchRad;

    double littleMetacarpalLeftRad;
    double littleMetacarpalRightRad;
    double littlePitchRad;
  };

  explicit Prosthetic(const JointPositions& initalPositions);
  ~Prosthetic();

  void SetJointGoalPositions(const JointPositions& positions);

  JointPositions GetCurrentBufferedPositions() const;
  JointPositions GetCurrentTruePositions();

  void Stop() { running = false; }

 private:
  static constexpr TrapezoidalMotionProfile::Constraints
      FINGER_PITCH_CONSTRAINTS{1.25 * M_PI, 3 * M_PI};
  static constexpr TrapezoidalMotionProfile::Constraints FINGER_YAW_CONSTRAINTS{
      M_PI_4, M_PI_4 / 2.0};
  static constexpr TrapezoidalMotionProfile::Constraints
      WRIST_PITCH_CONSTRAINTS{M_PI_4, M_PI_4 / 2.0};

  static constexpr int period_ms = 10;
  static constexpr bool real_prosthetic = false;

  ServoBonnet controller;
  std::thread control_thread;
  std::mutex control_mutex;
  std::atomic_bool running;

  JointPositions currentPositions{};
  JointPositions goalPositions{};

  TrapezoidalMotionProfile thumbMetacarpalYawMotionProfile{
      FINGER_YAW_CONSTRAINTS};
  TrapezoidalMotionProfile thumbMetacarpalPitchMotionProfile{
      FINGER_PITCH_CONSTRAINTS};
  TrapezoidalMotionProfile thumbProximalPitchMotionProfile{
      FINGER_PITCH_CONSTRAINTS};

  TrapezoidalMotionProfile littleMetacarpalYawMotionProfile{
      FINGER_YAW_CONSTRAINTS};
  TrapezoidalMotionProfile littleMetacarpalPitchMotionProfile{
      FINGER_PITCH_CONSTRAINTS};
  TrapezoidalMotionProfile littleProximalPitchMotionProfile{
      FINGER_PITCH_CONSTRAINTS};

  TrapezoidalMotionProfile ringMetacarpalYawMotionProfile{
      FINGER_YAW_CONSTRAINTS};
  TrapezoidalMotionProfile ringMetacarpalPitchMotionProfile{
      FINGER_PITCH_CONSTRAINTS};
  TrapezoidalMotionProfile ringProximalPitchMotionProfile{
      FINGER_PITCH_CONSTRAINTS};

  TrapezoidalMotionProfile middleMetacarpalYawMotionProfile{
      FINGER_YAW_CONSTRAINTS};
  TrapezoidalMotionProfile middleMetacarpalPitchMotionProfile{
      FINGER_PITCH_CONSTRAINTS};
  TrapezoidalMotionProfile middleProximalPitchMotionProfile{
      FINGER_PITCH_CONSTRAINTS};

  TrapezoidalMotionProfile indexMetacarpalYawMotionProfile{
      FINGER_YAW_CONSTRAINTS};
  TrapezoidalMotionProfile indexMetacarpalPitchMotionProfile{
      FINGER_PITCH_CONSTRAINTS};
  TrapezoidalMotionProfile indexProximalPitchMotionProfile{
      FINGER_PITCH_CONSTRAINTS};

  TrapezoidalMotionProfile wristPitchMotionProfile{WRIST_PITCH_CONSTRAINTS};

  void Periodic();
};
