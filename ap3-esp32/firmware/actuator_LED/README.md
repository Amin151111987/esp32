# BTstack Port for the Espressif ESP32 Platform

Status: Initial port. Only SPP + LE Throughput example provided.

## Setup

- Follow [Espressif IoT Development Framework (IDF) setup](https://github.com/espressif/esp-idf) to install xtensa toolchain.
- Checkout our [esp-idf fork](https://github.com/mringwal/esp-idf) and switch to branch 'support-alternative-bluetooth-stack' to allow alternative Bluetooth stack and make IDF_PATH point to it.
- In port/esp32, configure serial port for firmware upload as described before
	
## Usage

In port/esp32, run

	make

to compile the LE Counter example

Run 
	
	make flash

to upload it to your device.

### Acknowledgments

First HCI Reset was sent to Bluetooth chipset by [@mattkelly](https://github.com/mattkelly)
