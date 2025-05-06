# Installing Rad Pro on the FNIRSI GC-01/JOY-IT JT-RAD01 with an ST-LINK USB dongle

These instructions let you install Rad Pro on an FNIRSI GC-01/JOY-IT JT-RAD01 with a CH32F103R8T6 (WCH) or APM32F103RBT6 (Geehy) microprocessor if you:

* cannot access the USB drive as described in the [main installation instructions](install.md).
* want to install Rad Pro from macOS or Linux.
* your FNIRSI GC-01/JOY-IT JT-RAD01 became unresponsive and you need to recover your device.

## Supplies

To install Rad Pro on your FNIRSI GC-01/JOY-IT JT-RAD01 Geiger counter with an ST-LINK USB dongle, you will need the following tools and components:

* An [ST-LINK V2 USB dongle (or clone)](https://www.amazon.com/s?k=st-link+v2)
* A 4-pin header
* A Philips screwdriver
* Optional: A soldering iron and solder
* If you use Windows, the [ST-LINK driver](https://www.st.com/en/development-tools/stsw-link009.html)

## Step 1: Open the device

![FNIRSI GC-01 circuit board types](img/gc-01-board-type.jpg)

Follow these steps to start installing Rad Pro:

* Turn off the device.
* Remove the screws holding the back case and open the device.
* Disconnect the large battery.
* Take note of the MCU (microprocessor) installed in your device: CH32F103R8T6 (WCH) or APM32F103RBT6 (Geehy). The picture above should help you identify the MCU.
* Take note of the type of Geiger tube: the markings on the glass should be either J321 (as in the picture above), J305, M4011, J614 or J613. If you have an unmarked tube measuring 55 or 65 mm long, you probably have a J614 or J613 tube, respectively.

The board should look like in the photo above. If it does not, you may have a different hardware revision. In this case, Rad Pro may not function properly.

**WARNING:** Rad Pro is incompatible with devices with a CACHIP microprocessor. On such devices, these instructions WILL BREAK YOUR DEVICE.

## Step 2: Connect the programmer

![SWD connectors](img/gc-01-swd.jpg)

Now, follow these steps:

* Optional: Solder the 4-pin header to the JP1 pads of the board.
* If you use Windows, install the [ST-LINK driver](https://www.st.com/en/development-tools/stsw-link009.html).
* Connect the ST-LINK V2 device to the four pads next to the button cells.
  * If the pads are unlabeled, the pins are, from left to right:
    * 3.3V
    * SWDIO
    * SWCLK
    * GND

  * If the pads are labeled, connect as follows:
    * CLK > SWCLK
    * DIO > SWDIO
    * GND > GND
    * VCC > 3.3V

![ST-LINK V2 programmer](../../img/ST-LINK-V2.png)

Make sure the electrical connections are correct. You may break your device if the connections are not correct.

## Step 3: Flash the firmware

Now, download the latest `radpro-flashtool` from the [Rad Pro releases](https://github.com/Gissio/radpro/releases).

* To install from Windows, go to the `fnirsi-gc01` folder and double click `fnirsi-gc01-[mcu]-[type].bat`.
* To install from macOS or Linux, go to the `fnirsi-gc01` folder and double click `fnirsi-gc01-[mcu]-[type].sh`.

`[mcu]` is the type of microprocessor of your board: `ch32f103r8t6` or `apm32f103rbt6`. **NOTE:** Flashing the CH32F103R8T6 takes a couple of minutes.

`[type]` is the type of installation: `install` or `update`. Note that not all releases support updating.

To flash the [original firmware](firmware), drag it onto `fnirsi-gc01-[device]-[type].bat` (Windows) or `fnirsi-gc01-[device]-[type].sh`.

If flashing does not work, try powering your device from USB.

After successfully flashing the firmware, reconnect the battery connector and close the device.

Now, proceed to [step 3 of the main installation instructions](install.md#step-3-configure-your-device).
