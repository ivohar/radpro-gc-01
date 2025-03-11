# User's manual

## Features

* Measurement view with large digits, ideal for field application.
* Configurable pulse indication with optional threshold: sound (off, clicks, chirps, beeps), vibration (on supported devices), pulse LED (on supported devices), display flashes (on display sleep).
* Configurable alarm signaling: sound, vibration, pulse LED and display flashes.
* Ability to dismiss alarms.
* Tube fault alarm.
* Multiple measurement units: Sievert, rem, cpm (counts per minute), cps (counts per second).
* Configurable instantaneous rate averaging (adaptive fast, adaptive precision, 1 minute, 30 seconds and 10 seconds), 
* Configurable average timer for performing surveys.
* Customizable Geiger-Müller tube settings: conversion factor, dead-time compensation, background compensation, high voltage generator PWM frequency and duty cycle (on supported devices).
* Preconfigured high voltage profiles (on supported devices).
* Offline and live data logging with data compression.
* Compatibility with the [GeigerLog](https://github.com/Gissio/geigerlog-radpro) data logging software.
* Keyboard lock mode.
* Statistics for tracking device usage and state.
* Geiger tube dead-time measurement.
* Game: nuclear chess (on supported devices).
* User interface based on the industry-standard [OpenBridge 5.0](https://www.openbridge.no/) design system.
* Anti-aliased text rendering on color screens.
* `radpro-tool` for low-level access to the device from a computer.
* Power-on self-test and safety watchdog.

## Measurements

For proper tracking of dose, always power off the device using the keyboard. Avoid removing the batteries to power off the device.

Rad Pro supports the following measurement modes:

### Instantaneous rate

The instantaneous rate is estimated by dividing the number of pulses within the instantaneous rate averaging period, minus one, by the time between the first and last pulse within that period.

The secondary view can be switched between an instantaneous rate bar view ([logarithmic](https://en.wikipedia.org/wiki/Logarithmic_scale), with each tick representing a tenfold increase in radiation level, and 1 µSv/h and 10 µSv/h alert zones), a time view that displays the length of the averaging period, an instantaneous rate max view, and an instantaneous rate cpm (counts per minute) view.

The [confidence interval](https://en.wikipedia.org/wiki/Confidence_interval) estimates the range of values that contain the true, actual instantaneous rate with a 95% probability, assuming a constant level of radiation.

An example: Imagine measuring an instantaneous radiation rate of 1.000 µSv/h with a confidence interval of ±40%. This means there's a 95% probability that the true radiation level lies between 0.600 µSv/h and 1.400 µSv/h. In other words, there's a 5% chance the true level falls outside this range. As the confidence interval narrows (becomes smaller), the measurement becomes more precise and certain.

To reset the instantaeous rate or dismiss a rate alarm, read the device's installation instructions.

### Average rate

The average rate is estimated by dividing the number of pulses within the averaging period, minus one, by the time between the first and last pulse within that period.

The secondary view can be switched between a time view that displays the length of the averaging period, an average rate cpm (counts per minute) view, and a counts view.

The confidence interval assumes a constant level of radiation over the averaging period.

Averaging can be indefinite, or limited by a configurable time or confidence level. When averaging finishes, the device will flash and emit a beep, signaling the completion of the measurement.

To reset the average, read the device's installation instructions.

An example: Imagine measuring background radiation. A one-minute average yields 0.210 µSv/h with a ±36% confidence interval. This means there's a 95% chance the true radiation level lies between 0.134 µSv/h and 0.286 µSv/h. Finding this interval too wide, you repeat the measurement with a 30-minute average. The new measurement is 0.154 µSv/h, now with a much narrower ±7.7% confidence interval.

### Cumulative dose

The cumulative dose is calculated from the number of pulses in the time period.

The secondary view can be toggled between a time view, showing the duration of the measurement period, and a counts view, showing the pulse count.

To reset the cumulative dose or dismiss a dose alarm, refer to the device's installation instructions.

**WARNING**: Accurate cumulative dose measurements require an energy-compensated Geiger tube. The default tubes in Geiger units supported by Rad Pro are **not** energy-compensated, resulting in significant measurement errors.

### History

The history is calculated from the instantaneous rate, sampled once per second. The plot uses a [logarithmic scale](https://en.wikipedia.org/wiki/Logarithmic_scale), where each division represents a tenfold increase in radiation level. Alert zones at 1 µSv/h and 10 µSv/h are visually indicated.

The view can be toggled between a 10-minute view, 60-minute view or 24-hour view.

To reset the history, consult the device's installation instructions.

## Conversion factor

The conversion factor specifies the relationship between cpm (counts per minute) and the level of radiation in µSv/h. It is specific to the type of radiation being detected and the particular characteristics of the tube.

Rad Pro comes with default conversion factors for various Geiger-Müller tubes:

* J305: 153.8 cpm/µSv/h
* J321: 153.8 cpm/µSv/h
* J613: 60.0 cpm/µSv/h
* J614: 60.0 cpm/µSv/h
* M4011: 153.8 cpm/µSv/h
* SBM-20: 153.8 cpm/µSv/h

You can also set a custom conversion factor by going to the settings, selecting “Geiger tube”, “Conversion Factor”, and choosing an appropriate value from the list.

When using sensitive detectors with a conversion factor of 600 cpm/µSv/h or higher, Rad Pro caps the pulse indication at 600 cpm/µSv/h to provide approximately one pulse per second at background levels of radiation. This does not impact the actual measurement.

## Data logging

Rad Pro lets you log cumulative dose count, from which both rate and dose can be derived.

To start logging, ensure that the time zone, date and time of your device are correctly set, and enable data logging in the settings. Data is automatically logged in the background. When the memory becomes full, older data is overwritten.

To establish a USB data connection, some devices require enabling “Data mode” in the settings. To save power, disable “Data mode” when you are finished.

To download the data logs or log data live to a computer, use the [GeigerLog](https://github.com/Gissio/geigerlog-radpro) data logging software. “CPS” data is the low-level counts per second value. To average the data, use the “MvAvg” function.

Please note that data is not logged during the download process.

## Instantaneous rate averaging

Rad Pro offers several options for adjusting the instantaneous rate averaging period:

* “Adaptive fast” averages the last 20 pulses, providing a confidence interval of ±50%. It is best for users who need quick radiation alerts. You can view the “adaptive fast” response curves (radiation level vs. response time) here: [instantaneousaveraging-adaptivefast.ipynb](../tests/instantaneousaveraging-adaptivefast.ipynb)
* “Adaptive precision” is similar to “Adaptive fast”, but also sets a minimum averaging time of 5 seconds. This ensures higher precision when more than 20 pulses occur within the last 5 seconds. Otherwise it functions identically to "Adaptive fast". It is best for users who deem precision more important than fast response. You can view the “adaptive precision” response curves (radiation level vs. response time) here: [instantaneousaveraging-adaptiveprecision.ipynb](../tests/instantaneousaveraging-adaptiveprecision.ipynb)
* “1 minute”, “30 seconds” and “10 seconds” use fixed averaging periods. They can produce higher precision than the adaptive options, but have a much slower response.

## Instantaneous rate alarm

To minimize false alarms, the instantaneous rate alarm triggers only when the confidence interval falls below 75%.

## Dead time and dead-time compensation

[Dead time](https://en.wikipedia.org/wiki/Geiger%E2%80%93M%C3%BCller_tube#Quenching_and_dead_time) is the period of time during which the Geiger-Müller tube is unable to detect another radiation event immediately after detecting one. This occurs because the tube becomes saturated after each radiation event, typically for 50-200 µs. Consequently, measurements of high levels of radiation will be inaccurate as the tube fails to register the counts during the dead-time period.

Rad Pro lets you add these missed counts by applying dead-time compensation.

To use dead-time compensation you need to measure the dead time first. You can do this by going to Rad Pro's settings, selecting “Statistics” and monitoring the “Dead time” value. Dead time is estimated by measuring the shortest time interval between pulses. Obtaining an accurate result requires several hours under normal levels of radiation. To accelerate this process, use a radioactive source.

Rad Pro employs the non-paralyzable model for dead-time compensation:

$$n = \frac{m}{1 - m \tau}$$

Here, $m$ is the uncompensated rate in counts per seconds, $\tau$ is the tube's dead time in seconds and $n$ is the compensated rate in counts per second. To prevent overflow, the compensation factor $1/(1 - m\tau)$ is limited to a maximum value of 10.

Dead-time compensation is applied to instantaneous rate, average rate, cumulative dose and history. It is not applied to tube life pulse count nor data logs.

## Background compensation

Geiger-Müller tubes, being composed of matter, inherently contain atoms prone to radioactive decay. Consequently, the tubes themselves emit radiation. This intrinsic radiation will depend on factors such as tube type, size, and production batch.

Rad Pro lets you remove these extra counts by applying background compensation.

Background compensation is applied to instantaneous rate, average rate, cumulative dose and history. It is not applied to tube life pulse count nor data logs.

## HV profiles

(on supported devices)

HV profiles let you control the high voltage supplied to the Geiger-Müller tube. Rad Pro includes several pre-configured profiles that let you balance power consumption and accuracy.

You can also define your own HV profile. **WARNING:** Wrong profile settings may damage the tube from overvoltage, as well as the switching transistor from overcurrent.

Setting up a custom HV profile requires measuring the high voltage at the tube. To accomplish this, connect a 1 GΩ resistor in series to the positive terminal of a high-quality multimeter (with a 10 MΩ input impedance). Ensure the resistor is clean to prevent spurious currents. Set the multimeter to the 20 V range. Connect the negative terminal of the multimeter to ground, and the free end of the resistor to the tube's anode. The high voltage corresponds approximately to the multimeter reading multiplied by a factor of (1000 MΩ + 10 MΩ) / 10 MΩ = 101. **WARNING:** High voltage can be lethal.

An HV profile consists of a [PWM](https://en.wikipedia.org/wiki/Pulse-width_modulation) frequency and duty cycle. Typically, higher frequency values produce lower voltage ripple (voltage variations in time) but consume more power. Conversely, lower frequency values require less power, but may sacrifice measurement accuracy.

## Tube fault alarm

Rad Pro triggers a fault alarm if no pulses are detected within a five-minute interval. This may occur due to a malfunctioning high-voltage generator, or a defective Geiger-Müller tube.

Rad Pro also triggers a fault alarm if the Geiger-Müller tube becomes saturated due to high radiation levels, or shorted due to a malfunction.

## Random generator

The [random generator](https://en.wikipedia.org/wiki/Hardware_random_number_generator) produces 16 symbols per run. If additional symbols are required, simply return to the menu and initiate a new run.

The generator produces random bits by comparing the time interval between two successive pulses. To avoid bias, every second bit is flipped. The generator stores random data in a 128-bit buffer.

Random symbols are generated from the bits using the [Fast Dice Roller](https://arxiv.org/abs/1304.1916) algorithm. “Alphanumeric” uses approximately 6 bits per symbol, “Full ASCII” uses 7 bits, “20-sided dice” uses 5 bits, “Hexadecimal”, “Decimal” and “12-sided dice” use 4 bits, “8-sided dice” and “6-sided dice” use 3 bits, “4-sided dice” uses 2 bits, and “Coin flip” uses 1 bit.

For faster bit generation, use a radioactive source.

## radpro-tool

`radpro-tool` gives you low-level access to your device from a computer, allowing you to download data logs, log data live, submit data live to radiation monitoring websites, get device information, set custom HV profiles (on supported devices) and sync the device's clock.

To use `radpro-tool`, install [Python](https://www.python.org) and [PIP](https://pip.pypa.io/en/stable/). You must also install the necessary requirements by running the following command in a terminal:

    pip install -r tools/requirements.txt

To get help about `radpro-tool`, run the following command in a terminal:

    python tools/radpro-tool.py --help

To sync the clock of a Rad Pro device connected to COM13:

    python tools/radpro-tool.py --port COM13

To download the data log to the file `datalog.csv`:

    python tools/radpro-tool.py --port COM13 --download-datalog datalog.csv

To log pulse data live to the file `live.csv` every minute:

    python tools/radpro-tool.py --port COM13 --log-pulsedata live.csv --period 60

To submit data live to the https://gmcmap.com website:

    python tools/radpro-tool.py --port COM13 --submit-gmcmap [USER_ACCOUNT_ID] [GEIGER_COUNTER_ID]

## Data communications

To communicate with Rad Pro through a USB serial port, read the [communications protocol description](comm.md).

## FAQ

**Q: How can I reset the average and cumulative measurements?**

**A:** Refer to the keyboard mapping section of your device's installation instructions for guidane on resetting measurements.

**Q: Why does my device's date and time reset every time I turn it on?**

**A:** Your device's date and time reset because its real-time clock (RTC) loses power when turned off. On the FNIRSI GC-01, this is likely due to a failing backup battery (a CR1220). Replacing this battery should fix the issue.

**Q: When I power on my device, why does Rad Pro stay on the splash screen for so long (up to 60 seconds)?**

**A:** The extended splash screen time could be due to the microprocessor initializing the real-time clock. If this delay persists, check the previous answer for troubleshooting steps.

**Q: Why are my instantaneous rate measurements so noisy?**

**A:** There's a balance between noise and response speed in instantaneous rate measurements. To reduce noise, choose an averaging option that extends the measurement period, though this will slow the response time.

**Q: I enabled clicks but I'm not hearing any.**

**A:** If a pulse threshold is set, you'll only hear clicks when radiation levels exceed this threshold. Make sure this setting isn't active if you expect to hear clicks for all counts.

**Q: Why isn’t the rate alarm triggering?**

**A:** The rate alarm is designed to avoid false alarms by only triggering when the confidence interval drops below 75%.

**Q: My device seems to be missing counts.**

**A:** If background compensation is turned on, it subtracts counts from the total. Verify whether this feature is active.

**Q: My device is not clicking for every count.**

**A:** If a conversion factor over 600 cpm/µSv/h is selected, Rad Pro limits pulse indication to 600 cpm/µSv/h to prevent excessive audio feedback.
