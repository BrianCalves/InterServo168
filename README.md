
# InterServo168

This is ATmega168 microcontroller firmware for interactive control of a hobby servomotor by an Arduino Diecimila.

The firmware maintains an interactive control session via the Arduino Diecimila's USB port and positions a servo, accordingly.


## Synopsis

```
Keyboard controls
-----------------
    ENTER  Reset
    SPACE  Move to center
    <      Set counter-clockwise limit
    >      Set clockwise limit
    ,      Move counter-clockwise
    .      Move clockwise
    0-9    Move to relative position between min and max.


Resetting servo with 1500 microsecond pulse in [0, 65535].
Positioning servo with 1450 microsecond pulse.
Positioning servo with 1400 microsecond pulse.
Positioning servo with 1350 microsecond pulse.
```


## Description

The firmware allows you to connect to the Arduino Diecimila with a terminal program and command a hobby servo wired to the Diecimila.

The firmware assumes the servomotor signal line is connected to PORTD, pin 6 (`PIND6`) of the ATmega168.

This firmware differs from typical Arduino community practice, in that it does not employ the Arduino IDE or its language, Processing.


## System Requirements

This C/C++ language firmware was developed on Mac OS X using:

- Arduino Diecimila (ATmega168, 5V, 16 Mhz)
- avr-gcc (GCC) 4.3.0
- avrdude version 5.11
- GNU Make 3.81

The build tools are, or were, commonly included with the Arduino IDE.
If you have Arduino IDE of suitable vintage, you likely already have the required tools installed on your host platform.

The instructions which follow assume you are using Mac OS X 10.5 or a similar, UNIX-like, operating system.
If you have not already done so, you may need to add toolchain executables to your path in order to successfully execute the instructions which follow.


## Compiling the Firmware

The source code comprises `Main.cc` and `Makefile`.

```
$ make
    o Compiling Main.cc
Main.cc: In function 'int main()':
Main.cc:143: warning: only initialized variables can be placed into program memory area
Main.cc:144: warning: only initialized variables can be placed into program memory area
Main.cc:145: warning: only initialized variables can be placed into program memory area
Main.cc:146: warning: only initialized variables can be placed into program memory area
Main.cc:147: warning: only initialized variables can be placed into program memory area
Main.cc:148: warning: only initialized variables can be placed into program memory area
Main.cc:149: warning: only initialized variables can be placed into program memory area
Main.cc:150: warning: only initialized variables can be placed into program memory area
Main.cc:151: warning: only initialized variables can be placed into program memory area
Main.cc:152: warning: only initialized variables can be placed into program memory area
Main.cc:153: warning: only initialized variables can be placed into program memory area
    o Linking Main.elf
    o Creating Main.hex
```


## Installing the Firmware

Assuming your Arduino Diecimila includes the standard bootloader, you need only connect your device and use `avrdude` to flash the firmware.
For your convenience, there is a Make target for this purpose.

1. Connect your Arduino Diecimila to the host platform.
2. Detect the device file name of your Arduino Diecimila (typically of the form `/dev/tty.usbserial-XXXXXXXX`)
3. Edit the Makefile, replacing the placeholder device name with your device name (`/dev/tty...`).
4. `make arduino` to flash the compiled firmware onto your device


## Establishing an Interactive Control Session

Use a terminal program to interact with the firmware over a virtual serial port.
If you are using Mac OS X 10.5 as your host platform, you can use the `cu(1)` "Call Up" program.
Typically:
```
cu --line /dev/tty.usbserial-XXXXXXXX --speed 9600 --parity=none
```

Substitute the name of your device for the placeholder text, above.
When the connection is established with the firmware, the firmware will emit advice similar to:
```
Free RAM: 260 bytes

Keyboard controls
-----------------
    ENTER  Reset
    SPACE  Move to center
    <      Set counter-clockwise limit
    >      Set clockwise limit
    ,      Move counter-clockwise
    .      Move clockwise
    0-9    Move to relative position between min and max.


Resetting servo with 1500 microsecond pulse in [0, 65535].
```

At this point, use the `,` and `.` keys, to position the servo.

You can establish a software limit by moving the servo to the desired limit position and activating the `<` or `>` keys, for counter-clockwise limit or clockwise limit, respectively.

Once you set a clockwise limit and counter-clockwise limit, you can use the `0` to `9` keys to position the servo, proportionately, within the limits you defined.


## Wiring a Hobby Servo to the Diecimila

![Photo of Arduino Diecimila](ArduinoDiecimila-PinD6.png?raw=true "Photo of an Arduino Diecimila with Pin D6 Marked for Attention")

I tested the firmware with a small, low-voltage, low-current, hobby servomotor.

A typical hobby servo has three conductors: `+V`, `Ground`, and `Control`.
I wired the `Ground` and `+V` conductors of my servo to power supply pins, and the `Control` conductor to pin D6 of the Arduino Diecimila.

If you connect the servo control line to a different pin, you'll need to edit the firmware source code, accordingly.

If your servo has atypical characteristics, you may need to edit the firmware source code to adjust signal timing.
Some servos could require additional circuitry not described here.

BEFORE CONNECTING ANY CONDUCTORS, RESEARCH THE SPECIFICATIONS OF YOUR PARTICULAR COMPONENTS.
THIS IS A CURSORY GUIDE TO THIS FIRMWARE, NOT A SUBSTITUTE FOR EDUCATION.
THERE IS ALWAYS RISK OF INJURY, DAMAGE, OR DESTRUCTION, IF YOU MISHANDLE OR MISWIRE COMPONENTS, OR EXCEED THEIR SPECIFICATIONS.
YOU ACCEPT ALL RISK ASSOCIATED WITH THIS ACTIVITY.
REFER TO FURTHER DISCLAIMERS BELOW, AND IN THE LICENSE WHICH ACCOMPANIES THIS SOFTWARE.


## Caveats

- Various parameters regulating behavior of the firmware are necessarily hard-coded in the source code, or an attendant makefile. You may need to maintain correspondence of certain values between the source code, makefile, and any command-line invocations you make.

- The suitability of various hard-coded values depends upon your specific hardware. If your hardware differs from mine, you'll likely need to edit the firmware source code to fit your circumstances.

- During device boot-up, the servo may hunt briefly, because it is not under control of the firmware, yet.


## License Terms

This software is provided to you under the [Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0) (the "License").
You may not use this software except in compliance with the License.

THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
IN NO EVENT SHALL THE AUTHORS OR RIGHTS HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY, ARISING FROM, OUT OF, OR IN CONNECTION WITH THE SOFTWARE, OR ITS USE OR OTHER DEALINGS.
REFER TO THE LICENSE FOR SPECIFIC LANGUAGE GOVERNING PERMISSIONS AND LIMITATIONS.

