#include "Prosthetic.h"
#include <fmt/format.h>

// true servo range is about 170°
Prosthetic::ServoPositions JointPositionsToServoPositions(
    Prosthetic::JointPositions positions) {
  // TODO based on physical prosthetic
  return Prosthetic::ServoPositions{};
}

Prosthetic::JointPositions ServoPositionsToJointPositions(
    Prosthetic::ServoPositions positions) {
  // TODO based on physical prosthetic
  return Prosthetic::JointPositions{};
}

Prosthetic::JointPositions Prosthetic::GetCurrentBufferedPositions() const{
  return currentPositions;
}

Prosthetic::JointPositions Prosthetic::GetCurrentTruePositions() {
  const ServoPositions currentServoPositions{
      controller.GetServo(0)->GetAngle(), controller.GetServo(1)->GetAngle(),
      controller.GetServo(2)->GetAngle(), controller.GetServo(3)->GetAngle(),
      controller.GetServo(4)->GetAngle(), controller.GetServo(5)->GetAngle(),
      controller.GetServo(6)->GetAngle(), controller.GetServo(7)->GetAngle(),
      controller.GetServo(8)->GetAngle(), controller.GetServo(9)->GetAngle(),
      controller.GetServo(10)->GetAngle(),
      controller.GetServo(11)->GetAngle(),
      controller.GetServo(12)->GetAngle(),
      controller.GetServo(13)->GetAngle(),
      controller.GetServo(14)->GetAngle()};

  return ServoPositionsToJointPositions(currentServoPositions);
}


void Prosthetic::SetJointGoalPositions(const JointPositions& positions) {
  std::lock_guard<std::mutex> lock(control_mutex);
  goalPositions = positions;
}

void Prosthetic::Periodic() {
  constexpr double deltaTimeSeconds = period_ms / 1000.0;

  // Create setpoints using motion profiles and current goal
  JointPositions setpointPositions{};
  {
    std::lock_guard<std::mutex> lock(control_mutex);

    setpointPositions.thumbMetacarpalYawAngleRad =
        thumbMetacarpalYawMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.thumbMetacarpalYawAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.thumbMetacarpalYawAngleRad, 0})
            .position;
    setpointPositions.thumbMetacarpalPitchAngleRad =
        thumbMetacarpalPitchMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.thumbMetacarpalPitchAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.thumbMetacarpalPitchAngleRad, 0})
            .position;
    setpointPositions.thumbProximalPitchAngleRad =
        thumbProximalPitchMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.thumbProximalPitchAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.thumbProximalPitchAngleRad, 0})
            .position;

    setpointPositions.indexMetacarpalYawAngleRad =
        indexMetacarpalYawMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.indexMetacarpalYawAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.indexMetacarpalYawAngleRad, 0})
            .position;
    setpointPositions.indexMetacarpalPitchAngleRad =
        indexMetacarpalPitchMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.indexMetacarpalPitchAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.indexMetacarpalPitchAngleRad, 0})
            .position;
    setpointPositions.indexProximalPitchAngleRad =
        indexProximalPitchMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.indexProximalPitchAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.indexProximalPitchAngleRad, 0})
            .position;

    setpointPositions.middleMetacarpalYawAngleRad =
        middleMetacarpalYawMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.middleMetacarpalYawAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.middleMetacarpalYawAngleRad, 0})
            .position;
    setpointPositions.middleMetacarpalPitchAngleRad =
        middleMetacarpalPitchMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.middleMetacarpalPitchAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.middleMetacarpalPitchAngleRad, 0})
            .position;
    setpointPositions.middleProximalPitchAngleRad =
        middleProximalPitchMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.middleProximalPitchAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.middleProximalPitchAngleRad, 0})
            .position;

    setpointPositions.ringMetacarpalYawAngleRad =
        ringMetacarpalYawMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.ringMetacarpalYawAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.ringMetacarpalYawAngleRad, 0})
            .position;
    setpointPositions.ringMetacarpalPitchAngleRad =
        ringMetacarpalPitchMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.ringMetacarpalPitchAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.ringMetacarpalPitchAngleRad, 0})
            .position;
    setpointPositions.ringProximalPitchAngleRad =
        ringProximalPitchMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.ringProximalPitchAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.ringProximalPitchAngleRad, 0})
            .position;

    setpointPositions.littleMetacarpalYawAngleRad =
        littleMetacarpalYawMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.littleMetacarpalYawAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.littleMetacarpalYawAngleRad, 0})
            .position;
    setpointPositions.littleMetacarpalPitchAngleRad =
        littleMetacarpalPitchMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.littleMetacarpalPitchAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.littleMetacarpalPitchAngleRad, 0})
            .position;
    setpointPositions.littleProximalPitchAngleRad =
        littleProximalPitchMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.littleProximalPitchAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.littleProximalPitchAngleRad, 0})
            .position;

    setpointPositions.wristPitchAngleRad =
        wristPitchMotionProfile
            .Calculate(deltaTimeSeconds,
                       TrapezoidalMotionProfile::State{
                           currentPositions.wristPitchAngleRad, 0},
                       TrapezoidalMotionProfile::State{
                           goalPositions.wristPitchAngleRad, 0})
            .position;
  }

  // Instead of retriving the current angle from the IC every time, hold them in
  // the local buffer. We can make this assumptiom becasue servos don't follow
  // standard control theory assumptions because they converge instantly.
  currentPositions = setpointPositions;

  if(OUTPUT_TO_PROSTHETIC) {
    // Form the servo positions from the joint positions
    auto const outputPositions =
        JointPositionsToServoPositions(setpointPositions);

    // Apply output positions to the physical servos
    controller.GetServo(0)->SetAngle(outputPositions.thumbMetacarpalLeftRad);
    controller.GetServo(1)->SetAngle(outputPositions.thumbMetacarpalRightRad);
    controller.GetServo(2)->SetAngle(outputPositions.thumbPitchRad);
    controller.GetServo(3)->SetAngle(outputPositions.indexMetacarpalLeftRad);
    controller.GetServo(4)->SetAngle(outputPositions.indexMetacarpalRightRad);
    controller.GetServo(5)->SetAngle(outputPositions.indexPitchRad);
    controller.GetServo(6)->SetAngle(outputPositions.middleMetacarpalLeftRad);
    controller.GetServo(7)->SetAngle(outputPositions.middleMetacarpalRightRad);
    controller.GetServo(8)->SetAngle(outputPositions.middlePitchRad);
    controller.GetServo(9)->SetAngle(outputPositions.ringMetacarpalLeftRad);
    controller.GetServo(10)->SetAngle(outputPositions.ringMetacarpalRightRad);
    controller.GetServo(11)->SetAngle(outputPositions.ringPitchRad);
    controller.GetServo(12)->SetAngle(outputPositions.littleMetacarpalLeftRad);
    controller.GetServo(13)->SetAngle(outputPositions.littleMetacarpalRightRad);
    controller.GetServo(14)->SetAngle(outputPositions.littlePitchRad);
  } else {
    fmt::print("{}, {}\n{}, {}\n{}, {}\n{}, {}\n{}, {}\n\n",
           setpointPositions.thumbMetacarpalPitchAngleRad * (180.0 / M_PI),
           setpointPositions.thumbProximalPitchAngleRad * (180.0 / M_PI),
           setpointPositions.indexMetacarpalPitchAngleRad * (180.0 / M_PI),
           setpointPositions.indexProximalPitchAngleRad * (180.0 / M_PI),
           setpointPositions.middleMetacarpalPitchAngleRad * (180.0 / M_PI),
           setpointPositions.middleProximalPitchAngleRad * (180.0 / M_PI),
           setpointPositions.ringMetacarpalPitchAngleRad * (180.0 / M_PI),
           setpointPositions.ringProximalPitchAngleRad * (180.0 / M_PI),
           setpointPositions.littleMetacarpalPitchAngleRad * (180.0 / M_PI),
           setpointPositions.littleProximalPitchAngleRad * (180.0 / M_PI));
  }
}

Prosthetic::Prosthetic(const JointPositions& initalPositions)
    : controller("/dev/i2c-3"),
      running(true),
      currentPositions(initalPositions),
      goalPositions(initalPositions) {
  for (int i = 0; i < 16; i++) {
    controller.GetServo(i)->SetPulseWidthRange(550, 2600);
  }

  control_thread = std::thread([this] {
    constexpr std::chrono::duration<int, std::milli> periodDuration(period_ms);
    while (running) {
      auto const startTime =
          std::chrono::time_point_cast<std::chrono::microseconds>(
              std::chrono::steady_clock::now());

      Periodic();

      auto const endTime =
          std::chrono::time_point_cast<std::chrono::microseconds>(
              std::chrono::steady_clock::now());

      auto const deltaTime = (endTime - startTime);

      // Sleep for the remaining time in the period. This is os-specific but
      // should be at least period_ms duration total between loop itterations.
      std::this_thread::sleep_for(periodDuration - deltaTime);
    }
  });
}

Prosthetic::~Prosthetic() {
  if (control_thread.joinable()) {
    control_thread.join();
  }
}
