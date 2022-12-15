import asyncio
from bleak import BleakClient, discover
from typing import *
import datetime
from datetime import *
from vision.utils.bl_logging import log


class BLEConnection:
    def __init__(
            self,
            loop: asyncio.AbstractEventLoop,
            read_characteristic="00002a57-0000-1000-8000-00805f9b34fb",
            write_characteristic="00002A3D-0000-1000-8000-00805f9b34fb",
    ):
        self.client = None
        self.loop = loop
        self.read_characteristic = read_characteristic
        self.write_characteristic = write_characteristic

        # Device state
        self.connected = False
        self.connected_device = None

        # RX Buffer
        self.last_packet_time = datetime.now()
        self.rx_data = []
        self.rx_timestamps = []
        self.rx_delays = []

    def on_disconnect(self, client: BleakClient):
        self.connected = False
        # todo: add feedback to user
        log.error(f"Disconnected from {self.connected_device.name}")

    def notification_handler(self, sender: str, data: Any):
        self.rx_data.append(int.from_bytes(data, byteorder="big"))
        self.record_time_info()

    async def manager(self):
        """
        A connection manager that routinely check connection and reconnect when needed.
        """
        print("Starting connection manager.")
        while True:
            if self.client:
                await self.connect()
            else:
                await self.select_device()
                # await asyncio.sleep(15.0, loop=self.loop)

    async def init_connection(self, max_retry=5):
        """
        Initialize and start connection to bluetooth for once.
        """
        for i in range(max_retry):
            if not self.client:
                await self.select_device()
                await self.connect()
                return
            else:
                await self.connect()
                return
        raise ConnectionError("Cannot connect to BLE device: Exceeded max retry")

    async def connect(self):
        if self.connected:
            return
        try:
            await self.client.connect()
            self.connected = await self.client.is_connected()
            if self.connected:
                log.info(f"Connected to {self.connected_device.name}")
                self.client.set_disconnected_callback(self.on_disconnect)
            else:
                log.error(f"Failed to connect to {self.connected_device.name}")
        except Exception as e:
            print(e)

    async def cleanup(self):
        if self.client:
            await self.client.stop_notify(self.read_characteristic)
            await self.client.disconnect()

    async def select_device(self):
        log.info("Starting BLE Service")
        # await asyncio.sleep(2.0, loop=self.loop)  # Wait for BLE to initialize.
        await asyncio.sleep(2.0)  # Wait for BLE to initialize.
        devices = await discover()

        for i, device in enumerate(devices):
            if device.name=="BrushLens":
                log.info(f"Connecting to {device.name}")
                self.connected_device = device
                self.client = BleakClient(device.address, loop=self.loop)

    def record_time_info(self):
        present_time = datetime.now()
        self.rx_timestamps.append(present_time)
        self.rx_delays.append((present_time - self.last_packet_time).microseconds)
        self.last_packet_time = present_time

    def clear_lists(self):
        self.rx_data.clear()
        self.rx_delays.clear()
        self.rx_timestamps.clear()

    async def send(self, s):
        """
        Send string to bluetooth pheripheral
        :param s: string to send
        """
        # todo?: add return statement to guarantee command is sent
        if self.connected:
            bytes_to_send = bytearray(map(ord, s))
            await self.client.write_gatt_char(self.write_characteristic, bytes_to_send)
            # print(f"Sent: {s}")
