from adafruit_servokit import ServoKit
import time

servo_controller = ServoKit(channels=16)
for i in range(16):
    servo_controller.servo[i].set_pulse_width_range(550, 2550)

for i in range(8):
    servo_controller.servo[i].angle = 90

time.sleep(1)

servo_controller.servo[0].angle = 180
servo_controller.servo[1].angle = 0

servo_controller.servo[2].angle = 180
servo_controller.servo[3].angle = 0

servo_controller.servo[4].angle = 0
servo_controller.servo[5].angle = 180

servo_controller.servo[6].angle = 0
servo_controller.servo[7].angle = 180

# for i in range(0, 4, 2):
#     servo_controller.servo[i].angle = 180 if i < 2 else 0
#     servo_controller.servo[i + 1].angle = 0



# # left starting is 0
# # right starting is
#
# servo_controller.servo[14].angle = 90
# servo_controller.servo[15].angle = 90
#
# time.sleep(1)
#
# servo_controller.servo[14].angle = 180
# servo_controller.servo[15].angle = 0
