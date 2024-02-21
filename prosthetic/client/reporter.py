from adafruit_servokit import ServoKit

controller = ServoKit(channels=16)
while True:
    print([controller.servo[i].angle for i in range(16)])
