import struct
from adafruit_servokit import ServoKit
import asyncio
import websockets


async def main():
    uri = "ws://192.168.1.155:8080"
    servo_controller = ServoKit(channels=16)
    
    # TODO Configure the initial positions of the servos
    starting_positions = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    for i, v in enumerate(starting_positions):
        servo_controller.servo[i].set_pulse_width_range(550, 2550)
        servo_controller.servo[i].angle = v

    print(f"Looking for Prosthetic Server Connection at {uri}")
    async for websocket in websockets.connect(uri):
        try:
            print("Websocket Connection Opened")
            while True:
                current_servo_positions = [min(max(round(servo_controller.servo[k].angle), 0), 180) for k in range(16)]

                data = bytearray()
                for n in current_servo_positions:
                    data.extend(struct.pack('<H', n))
                await websocket.send(data)

                resp_positions_ba = await websocket.recv()
                requested_positions = [struct.unpack('<H', resp_positions_ba[i:i+2])[0] for i in range(0, len(resp_positions_ba), 2)]

                for i, v in enumerate(requested_positions):
                    servo_controller.servo[i].angle = v

        except websockets.ConnectionClosed:
            print("Websocket Connection Closed")
            continue


if __name__ == "__main__":
    asyncio.run(main())
