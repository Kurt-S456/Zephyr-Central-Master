# Zephyr Central Master SPI Benchmarking

An RTOS-based benchmarking suite designed to measure, evaluate, and profile Serial Peripheral Interface (SPI) communication constraints and throughput across three interconnected microcontrollers using Zephyr RTOS.

---

## Setup & Getting Started

This guide outlines how to configure, build, flash, and monitor the multi-MCU SPI benchmarking application on an Arch-based Linux distribution using the **STM32F103C8T6 (Blue Pill)** development board, an **ST-Link V2** flasher and a **CH340-based USB Debugger**.

### 1. Prerequisites & System Packages

Install the required compiler utilities, flashing tools, and serial monitors via the package manager:

```bash
sudo pacman -S tk dtc openocd picocom

```

#### Serial Permissions

To access the CH340 USB-to-UART adapter logs without root privileges, add your user account to the system's hardware communication group (`uucp`):

```bash
sudo usermod -aG uucp $USER

```

### 2. Workspace & Toolchain Initialization

Activate the local Python virtual environment using the Fish-specific script, then fetch the lightweight ARM embedded toolchain:

```fish
# Navigate to your workspace root
cd Zephyr-Central-Master

# Activate the virtual environment in Fish shell
source .venv/bin/activate.fish

# Install the minimal ARM toolchain and host tools
west sdk install -t arm-zephyr-eabi

```

---

### 3. Hardware Interfacing

#### Debugger (ST-Link V2)

Connect ST-Link V2 to the SWD header pins located on the short edge of the Blue Pill board:

* **GND** -> **GND**
* **SWCLK** -> **DCLK**
* **SWDIO** -> **DIO**
* **3.3V** -> **3.3V**

#### Serial Logging (CH340 USB-to-UART)

Cross-wire the adapter to the default UART_1 pins assigned by Zephyr's devicetree architecture:

| CH340 Pin | Blue Pill Pin | Peripheral Function |
| --- | --- | --- |
| **GND** | **GND** | Ground Reference |
| **RXD** | **PA9** | MCU UART1 TX |
| **TXD** | **PA10** | MCU UART1 RX |


---

### 4. Build, Flash, and Debug

Always ensure your virtual environment (`.venv`) is active before running `west` commands.

#### Step A: Compile the Application

Navigate into the target application sub-folder and trigger a clean build for the Blue Pill platform (`stm32_min_dev` variant `blue`):

```fish
cd Central-Master-STM32
west build -b stm32_min_dev/stm32f103xb -p always

```

#### Step A.1: Build Role-Specific Firmware (Worker/Child)

Build two firmware variants from the same project:

```fish
cd Central-Master-STM32

# Worker build (default build directory: build/)
west build -b stm32_min_dev@blue/stm32f103xb -p always -- \
	-DOVERLAY_CONFIG=controller.conf \
	-DDTC_OVERLAY_FILE=boards/app_controller.overlay

# Child build (separate build directory: build_target/)
west build -b stm32_min_dev@blue/stm32f103xb -d build_target -p always -- \
	-DOVERLAY_CONFIG=target.conf \
	-DDTC_OVERLAY_FILE=boards/app_target.overlay

```

#### Step B: Flash the Microcontroller

With the ST-Link V2 plugged into your computer, flash the target binary via OpenOCD:

```fish
west flash

```

If you are flashing the role-specific builds, run:

```fish
# Flash worker firmware from build/
west flash

# Flash child firmware from build_target/
west flash -d build_target

```

* **Troubleshooting Clone Chips:** If your Blue Pill utilizes a non-ST clone MCU (e.g., CH32, CS32, GD32) and OpenOCD halts due to an `unexpected idcode`, pass the core tap-ID override flag directly to the runner:
```fish
west flash -- -O "-c set CPUTAPID 0x2ba01477"

```



#### Step C: Monitor Serial Output

Open your serial console inside a terminal window to observe runtime performance indicators and boot banners:

```fish
# Open the specific CH340 port path at 115200 baud
picocom -b 115200 /dev/serial/by-id/usb-1a86_USB_Serial-if00-port0

```

* To view the initial boot sequences or benchmark metrics again, leave the terminal interface active and press the physical **Reset (NRST)** button on the surface of the Blue Pill.
* To exit the terminal console session, execute the hotkey combination: **`Ctrl + A`** then **`Ctrl + X`**.