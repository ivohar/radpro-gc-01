# Installing Rad Pro on the FNIRSI GC-01/JOY-IT JT-RAD01

## Supplies

To install Rad Pro on your FNIRSI GC-01/JOY-IT JT-RAD01 Geiger counter, you will need the following tools and components:

* A USB data cable
* A Philips screwdriver

## Step 1: Open the device

![FNIRSI GC-01 circuit board types](img/gc-01-board-type.jpg)

Follow these steps to start installing Rad Pro on your FNIRSI GC-01/JOY-IT JT-RAD01:

* Turn off the device.
* Remove the screws holding the back case and open the device.
* Take note of the MCU (microprocessor) installed in your device: CH32F103R8T6 (WCH) or APM32F103RBT6 (Geehy). The picture above should help you identify the MCU. **WARNING:** Rad Pro does not work on devices with a CACHIP microprocessor.
* Take note of the type of Geiger tube: the markings on the glass should be either J321 (as in the picture above), J305, M4011 or J614. If you have an unmarked tube measuring 55 or 65 mm long, you probably have a J614 or J613 tube, respectively.

* Close the device.

## Step 2: Flash the firmware

Now, download and unzip the latest `radpro-[version].zip` from the [Rad Pro releases](https://github.com/Gissio/radpro/releases). Go to the `fnirsi` folder, then to the `install` subfolder, and select the appropriate firmware file: `radpro-fnirsi-gc01_[mcu]-[language]-x.y.z-install.bin`.

`[mcu]` is your device's microprocessor type: `ch32f103r8t6` or `apm32f103rbt6`.

`[language]` is the two-letter code for your preferred firmware language (e.g. `en` for English).

Next:

* Connect the device to a USB 2.0 port of a Windows computer using the USB data cable. **WARNING:** Installation from macOS or Linux may fail. **WARNING:** Installation from a USB 3.0 port may fail.
* Power on the device. A USB drive should become available.
* Drag the firmware file onto the USB drive. The device should restart with Rad Pro installed.

Some devices require that you keep the power button pressed from power on until the firmware is copied to the USB drive and the device restarts. Some other devices require that you press the right/settings key while pressing the power key for the USB drive to become available.

If something goes wrong, press the reset switch next to the USB connector of your device and repeat the previous procedure. You can recover your device with the [original firmware](firmware).

If you are unable to install Rad Pro on your device, or your device becomes unresponsive, follow the [FNIRSI GC-01 alternative installation instructions](install-stlink.md).

## Step 3: Configure your device

Go to Rad Pro's settings, select "Geiger tube", "Sensitivity", and select the option that matches the Geiger-Müller tube installed in your device.

Within the "Geiger tube" settings, select "HV Profile" and choose an appropriate HV profile: "Energy-saving" is best for measuring background levels of radiation; "Factory default" is the profile from the original firmware and consumes considerably more power.

If the "Energy-saving" does not produce any tube counts, you will need to setup a custom HV profile to reduce power consumption.

If your device produces spontaneous key presses by itself, select the "Energy-saving" HV profile or use a custom HV profile to fix this problem.

Last, read the [user's manual](../../manual.md) for learning how to use Rad Pro, and the [ionizing radiation field guide](../../field-guide/field-guide.md) for learning more about ionizing radiation.

## Step 4: Last steps

If you like Rad Pro, consider watching the project to get notified when new releases are out. Also, show your support by starring the project on GitHub. And don't forget to read the keyboard mapping table below!

## Hardware-specific notes

The keys are mapped as follows:

  * Power on/off: long hold of the ok/power key.
  * Switch measurement mode: the up and down keys.
  * Switch measurement secondary view: the ok/power or left/back key.
  * Reset measurement/dismiss alarm: long hold of the left/back key.
  * Put display to sleep (measurement view only): the ok/power key.
  * Enter settings: the right/settings key.
  * Go up/down: the up and down keys.
  * Select option: the ok/power or right/settings key.
  * Go back: the left/back key.
  * Toggle pulses sound: long hold of the right/settings key.
  * Toggle lock mode: long hold of the ok/power and left/back key.

To establish a USB data connection, connect your FNIRSI GC-01/JOY-IT JT-RAD01 to a computer using a USB data cable.

### FNIRSI GC-01/JOY-IT JT-RAD01 with a CH32F103R8 (WCH) microprocessor

On the FNIRSI GC-01/JOY-IT JT-RAD01 with a CH32F103R8 microprocessor, you must enable "Data mode" in the settings to establish a USB data connection. To save power, disable "Data mode" when you are finished.

<!-- Calculated as follows:

* With 1-byte differential values: [6 pages * (1 timestamp entry/page [10 bytes] + 1012 differential entries/page [1 byte each])] = 6078 entries
* With 2-byte differential values: [6 pages * (1 timestamp entry/page [10 bytes] + 506 differential entries/page [2 byte each])] = 3042 entries

* 60-minute and 10-minute intervals require 2-byte differential values.
* 1-minute intervals and less require 1-byte differential values.

 -->

Data logging can store up to 6078 data points. At normal radiation levels (20 cpm), this allows for 126 days of data at 60-minute intervals, 21 days at 10-minute intervals, 4 days at 1-minute intervals, 16 hours at 10-second intervals and almost 2 hours at 1-second intervals.

The HV profile settings are:

* Factory default: 9.207 kHz frequency and 75% duty cycle.
* Energy-saving: 5 kHz frequency and 1.5% duty cycle.

### FNIRSI GC-01/JOY-IT JT-RAD01 with a APM32F103R8 (Geehy) processor

On the FNIRSI GC-01/JOY-IT JT-RAD01 with an APM32F103R8 (Geehy) processor, the USB data connection is always available.

<!-- Calculated as follows:

* With 1-byte differential values: [51 pages * (1 timestamp entry/page [10 bytes] + 1012 differential entries/page [1 byte each])] = 51663 entries
* With 2-byte differential values: [51 pages * (1 timestamp entry/page [10 bytes] + 506 differential entries/page [2 byte each])] = 25857 entries

* 60-minute and 10-minute intervals require 2-byte differential values.
* 1-minute intervals and less require 1-byte differential values.

 -->

Data logging can store up to 61793 data points. At normal radiation levels (20 cpm), this allows for 1077 days of data at 60-minute intervals, 179 days at 10-minute intervals, 35 days at 1-minute intervals, almost 6 days at 10-second intervals and 14 hours at 1-second intervals.

The HV profile settings are:

* Factory default: 47.058 kHz frequency and 50% duty cycle.
* Energy-saving: 5 kHz frequency and 1.5% duty cycle.
