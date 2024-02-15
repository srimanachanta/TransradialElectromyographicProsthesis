from adafruit_servokit import ServoKit
import asyncio
import websockets


async def main():
    uri = "ws://192.168.1.155:8080"
    servo_controller = ServoKit(channels=16)

    print(f"Looking for Prosthetic Server Connection at {uri}")
    async for websocket in websockets.connect(uri):
        try:
            print("Websocket Connection Opened")
            async for requested_position_byte_array in websocket:
                requested_positions = [int(byte) for byte in requested_position_byte_array]
                for i in range(16):
                    servo_controller.servo[i].angle = requested_positions[i]

        except websockets.ConnectionClosed:
            print("Websocket Connection Closed")
            continue


if __name__ == "__main__":
    asyncio.run(main())
