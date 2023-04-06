# Setting up the picoprobe on Windows Subsystem for Linux (WSL)

Basically follow the online guide https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html, 
but with a few additional steps

First you need to install OpenOCD.

On Raspberry Pi OS you can install openocd directly from the command line. But really, if you are not build openOCD from source.
```sudo apt install openocd```

You need to be running OpenOCD version 0.11.0 or 0.12.0 to have support for the Debug Probe. If you’re not running Raspberry Pi OS, or your distrbution installs an older version, or require SMP support, you can build and install openocd from source.

```sudo apt install automake autoconf build-essential texinfo libtool libftdi-dev libusb-1.0-0-dev```

and then build OpenOCD.
```git clone https://github.com/raspberrypi/openocd.git --branch rp2040 --depth=1 --no-single-branch
cd openocd
./bootstrap
./configure
make -j4
sudo make install
```

## Installing GDB

We also need to install the GNU debugger (GDB)
Install gdb-multiarch.
```sudo apt install gdb-multiarch```

If you are getting any errors looking like this when starting the GDB server
```Reading symbols from objdump-multiarch --syms -C -h -w /home/silverglade303/pico/CapstoneAmmeter/build/ADCTest/ADCTest.elf
Reading symbols from nm-multiarch --defined-only -S -l -C -p /home/silverglade303/pico/CapstoneAmmeter/build/ADCTest/ADCTest.elf
Launching GDB: gdb-multiarch -q --interpreter=mi2 /home/silverglade303/pico/CapstoneAmmeter/build/ADCTest/ADCTest.elf
    Set "showDevDebugOutput": true in your "launch.json" to see verbose GDB transactions here. Helpful to debug issues or report problems
Error: nm-multiarch failed! statics/global/functions may not be properly classified: Error: spawn nm-multiarch ENOENT
    Expecting `nm` next to `objdump`. If that is not the problem please report this.
Error: objdump failed! statics/globals/functions may not be properly classified: Error: spawn objdump-multiarch ENOENT    ENOENT means program not found. If that is not the issue, please report this problem.Launching gdb-server: openocd -c "gdb_port 50003" -c "tcl_port 50004" -c "telnet_port 50005" -s /home/silverglade303/pico/CapstoneAmmeter -f /home/silverglade303/.vscode/extensions/marus25.cortex-debug-1.4.4/support/openocd-helpers.tcl -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg
    Please check TERMINAL tab (gdb-server) for output from openocd
Finished reading symbols from objdump: Time: 166 ms
Finished reading symbols from nm: Time: 157 ms
OpenOCD GDB Server Quit Unexpectedly. See gdb-server output for more details.```

these commands might resolve that issue:
```sudo apt update
sudo apt install binutils-multiarch
cd /usr/bin
ln -s /usr/bin/objdump objdump-multiarch
ln -s /usr/bin/nm nm-multiarch```

## Uploading new programs to your Pico

The Pico Debug Probe will let you load binaries via the SWD port and OpenOCD: you will not need to unplug, and then push-and-hold, the BOOTSEL button every time you push a new binary to your Pico. Using the Debug Probe uploading new binaries is an entirely hands off affair.

Once you have built a binary:
```sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program blink.elf verify reset exit"```

## Debugging with SWD
It’ll also let you use openocd in server mode, and connect GDB, which gives you break points and “proper” debugging.

### IMPORTANT
To allow debugging, you must build your binaries as Debug rather than Release build type, e.g.
```
$ cd ~/pico/pico-examples/
$ rm -rf build
$ mkdir build
$ cd build
$ export PICO_SDK_PATH=../../pico-sdk
$ cmake -DCMAKE_BUILD_TYPE=Debug ..
$ cd blink
$ make -j4
```
In a debug build you will get more information when you run it under the debugger, as the compiler builds your program with the information to tell GDB what your program is doing.

See chapter Chapter 6 of Getting started with Raspberry Pi Pico for more information.

Now if you are on WSL, there's an extra step here. Follow the guidelines outlined here https://learn.microsoft.com/en-us/windows/wsl/connect-usb 

Then run OpenOCD 'server' ready to attach GDB to:
```$ sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000"```

```
$ gdb blink.elf
> target remote localhost:3333
> monitor reset init
> continue
```
### NOTE
On non-Raspberry Pi Linux platforms you should invoke GDB by using gdb-multiarch blink.elf.

# Install extensions for VScode

install "CMake Tools", "C/C++" and "cortex Debug". It's also a good idea to download the pico-project-generator https://github.com/raspberrypi/pico-project-generator since It has an option to create a project ready to be used with picoprobe. Although you have to doublecheck a few things to make sure it runs. In the VScode launch.json file that will be generated, replace;
```"configFiles": ["interface/picoprobe.cfg", "target/rp2040.cfg"]```
with
```"configFiles": ["interface/cmsis-dap.cfg", "target/rp2040.cfg"]```
and add: 
```"openOCDLaunchCommands": ["adapter speed 5000"]```
to the configuration as well. Everything else should be good. 

But before you try and compile your program with the Cortex-Debugger, we need to allow openocd access to the usb device without root priviliges (since VScode doesn't have root priviliges). 

Firstly go to this folder
```cd /etc/udev/rules.d```

Get the vendor and product id from the picoprobe device
```lsusb```
```
Bus 002 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
Bus 001 Device 002: ID 2e8a:000c Raspberry Pi Debug Probe (CMSIS-DAP)
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
```
The "Raspberry Pi Debug Probe (CMSIS-DAP)" device is the on we want. The vendor ID is "2e8a", and
the product ID is "000c".

Then create a file "10-my-usb.rules"
```sudo nano 10-my-usb.rules```

Write the following:
```ATTRS{idProduct}=="000c", ATTRS{idVendor}=="2e8a", MODE="660", GROUP="plugdev", TAG+="uaccess"```

save the file. 

Now check if the USB service is running. 
```sudo service udev status```

if the service isn't running, start it with 
```sudo service udev start```

otherwise
```sudo udevadm control --reload-rules```

Now it should work in VScode, try out the Cortex Debugger.
