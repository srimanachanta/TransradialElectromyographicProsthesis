import websockets
import asyncio
import struct


async def main():
    uri = "ws://192.168.1.155:8080"

    print(f"Looking for Prosthetic Server Connection at {uri}")
    async for websocket in websockets.connect(uri):
        try:
            print("Websocket Connection Opened")
            while True:
                current_servo_positions = [0 for k in range(16)]

                data = bytearray()
                for n in current_servo_positions:
                    data.extend(struct.pack('<H', n))
                await websocket.send(data)
                resp_positions_ba = await websocket.recv()

        except websockets.ConnectionClosed:
            print("Websocket Connection Closed")
            continue


if __name__ == "__main__":
    asyncio.run(main())
