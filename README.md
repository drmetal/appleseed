# Appleseed

### Appleseed has changed. this repo uses the now unsupported STM32 standard peripheral libraries. Have a look at [drmetal/like-posix](https://github.com/drmetal/like-posix) for an updated version that uses the STM32 Cube HAL. 

Appleseed is a mini POSIX like operating system for small microcontroller devices.

Appleseed is designed to present a development flow similar to writing c programs for Linux, for platforms that are too low on memory to run Linux.

The point of Appleseed? To enable C programs to be written for the tiny embedded platform, that can also run without modification on a PC.

Multi threading kernel (FreeRTOS), networking (LwIP), filesystem (Chans FF), Serial device IO, Shell, are connected under a subset of the posix system calls and a minimal libc implementation. 

OS and libc level features:
 - pthreads api 
 - posix system call api (Eg termios, unistd)
 - fully integrated file oriented IO for files, devices and sockets
 - minimal standard libraries (stdlib, stdio)
 - stdio file stream api, (Eg fopen, fprintf, perror, tmpfile etc)
 - powerful shell with IO redirection

Many hardware peripheral drivers are also included:
 - TFT Display, graphic widgets and smooth fonts
 - I2S
 - ADC's
 - DAC's
 - One Wire
 - PWM
 - Serial (raw and via /dev/ttyXX)
 - SPI (raw and via /dev/ttyXX)

Presently STM32F1, STM32F4 devices are supported. Any board with almost any STM32 device on it can be supported.

Demo projects are located here [STM32 Demos](https://github.com/drmetal/stm32-demos/blob/master/README.md).

<h2>Including Appleseed in a project</h2>

This repository may sit anywhere releative to a particular project.

```bash
# get a copy of Appleseed
git clone https://github.com/drmetal/appleseed.git
```

Once you have the code on disk, include Appleseed in your project makefile, this is outlined in the next section which describes the **build-env** folder.


<h4>build-env</h4>

appleseed/build-env contains a bunch of makefiles and scripts that can be used to build Appleseed projects.

Both STM32F1 and STM32F4 are supported, as well as multiple boards. Chip and board support is implemented in 
appleseed/stm32-device-support/board and appleseed/stm32-device-support/device.
 
The project build environment may be specified in the project makefile, by including appleseed/build-env/setup.mk in the project makefile. setup.mk contains the full set of configuration options.

All of the settings in **setup.mk** may be overridden in your project makefile, or as environment variables, or from the command line.

Project configuration makes up the first section of the project makefile. **setup.mk** must be included at the end of your Makefile, to enable building with Appleseed.

Here is an example:

``` make

#******************************************************************************
# Sample project makefile
#******************************************************************************

## this file lives where your project source is.

# to set options, define them here, set to an appriopriate value 

# set board if not specified on the command line
BOARD ?= HY-STM32_100P

# set project if not specified on the command line
PROJECT_NAME ?= demo-project

# specify project source
SOURCE = main.c

# the following are modules to enable - see setup.mk for a complete list of options.
USE_MINLIBC = 1
USE_LOGGER = 1
USE_LIKEPOSIX = 1
USE_DRIVER_LEDS = 1
USE_DRIVER_USART = 1
USE_DRIVER_SDCARD = 1
USE_DRIVER_FAT_FILESYSTEM = 1

## be sure to include the other makefiles
include ../appleseed/build-env/setup.mk
```

<h4>build options</h4>

 - make clean (remove all build artifacts)
 - make all (default build, no debug, optimization set to 2)
 - make small (default build but with optimization for size)
 - make release (no debug, optimization set to 2)
 - make debug (debug enabled, no optimization)

to rebuild completely:

 - make clean release
 - make clean debug

```bash
#example with build customization via command line parameters 
make clean release PROJECT_NAME="demo-project" BOARD="HY-STM32_100P"
```

<h2>stm32-device-support</h2>

A collection of code that supports building for different stm32fxxx devices.

Includes chip and board configurations, and hardware drivers.

chips supported: STM32F103VE, STM32F107RC, STM32F407VE, STM32F407VG

boards supported: HY-STM32_100P, stm32f4_discovery, uemb1, uemb4


<h4>build options</h4>

 - DEVICE_SUPPORT_DIR
	 - the path to the stm32-device-support root directory, relative to the project makefile
 - BOARD
	 - the name of the board to build for - this must be one of those listed in board.mk->BOARDS
 - APP_ADDRESS_OFFSET		
 	- can be set to some other value to make the built application run from a location other than the flash origin. for example setting to 0x4000 will offset the application in flash by 16KBytes. The first 16KBytes could then be used for a bootloader or non volatile data space... defaults to **0x0000**.


<h4>Timers</h4>

Timers are frequently used to support device drivers. It can be hard to work out if its safe to use a timer in the application. The drivers that use timers are:

|module	|file      |set in              |defaults to   |shared with|
|-------|----------|--------------------|--------------|-----------|
|systimer|systimer.c|board/<board>/systime_config.h|TIM2|lcd|
|adc_stream|adc_stream.c|<project>/adc_stream_config.h|TIM3||
|dac_stream|dac_stream.c|<project>/dac_stream_config.h|TIM6||
|i2s_stream|i2s_stream.c|<project>/i2s_stream_config.h|TIM5||
|ds1820|ds1820.c|<project>/ds1820_config.h|TIM4||
|lcd|lcd.c|<project>/board/<board>/lcd_config.h|TIM2|systimer|
|pwm|pwm.c|pwm_config.h|any left over||

<h4>About memory on the STM32 devices, with FreeRTOS</h4>

A ramble on memory usage.

For an overview WRT FreeRTOS, see:
http://www.freertos.org/Stacks-and-stack-overflow-checking.html

**The main stack, on the STM32F4, with FreeRTOS**

CPU/main stack is in CCRAM.

 - the scheduler itself will utilise the main stack
 - ISR’s will use the main stack
 - nothing else utilizes the main stack

Ways to harness CCRAM in the user program:

 - Use the special memory allocation api, accessible via **heap_ccram.h**. this provides the functions *malloc_ccram()*, *calloc_ccram()* and *free_ccram()*.

**Memory used in tasks**

Task stacks are allocated on the FreeRTOS heap, which lives in the bss section, in the main SRAM.

Many tasks will require buffer space for string or signal processing or whatever. Which memory region to use?

**static** - *text/bss*

- lives in main SRAM
- functions using static memory and not using thread synchronization primitives such at mutexes, will not be thread safe.
- using statically defined memory is good since memory usage is clear at compile time, and wrt access timing is the fastest and most deterministic.
- no allocation code is required so code size can be lower.
- static memory cannot be resized, so may waste space if it is infrequently used. 
	
**task stack** - *local variables*
	
- lives in the FreeRTOS heap, allocated at task creation time.
- in FreeRTOS the task stack is of fixed size, preallocated at task startup.
- the task stack can overflow if local variables are used extensively.
- a normal stack shrinks and grows with use, the freertos stack does not. it uses the space allocated at all times, until the task is deleted.
- The task stack has to be large enough to deal with all the local variables of all functions called. This can add up if there are a lot of nested function calls, or a lot of local variables.
- the task may use a lot of memory when it may not be using it, it is good to check stack usage at runtime to be sure memory is not wasted.
- the task designer should scale the task stack with any changes to the local variables
- tasks are fully reentrant in themselves, since their stack is separate for every instance.
	
**dynamic memory** - *FreeRTOS heap* / *CCRAM heap*
- under freertos there is no general, unmanaged heap like there is in a bare-metal libc.
- FreeRTOS claims a large memory array for its heap
- this is where tasks are created, and where malloc gets its memory from.
- All dynamic memory used by tasks (stacked and heaped) exists here
- its farly slow, ait is not managed by hardware as a stack would be.
- it can fragment, causing memory “loss”
- for every allocation, 8 extra bytes are required. this leads to a loss of 1kb when allocating 128 chunks of 128 bytes each (16384 bytes).
- not easy to plan dynamic allocation for an entire project, especially when 3rd party libraries are included
- using dynamic memory can lead to memory leaks when not manged with care.
- dynamic CCRAM - same as a normal heap but managed via the calls malloc_ccram() and free_ccram().
- dynamic CCRAM - care must be taken in choosing to use it, CCRAM may not be used by the DMA controllers (Eg FatFS and Audio uses DMA).
	 
**main stack** - *SRAM/CCRAM*

 - on the STM32F4 the main stack resides at the top of *CCRAM*
 - on the STM32F1 the main stack resides at the top of *SRAM*
 - the main stack is only used by the CPU under FreeRTOS - only the main function and ISR’s stack anything here.
 - when operating without FreeRTOS, this stack is used by all functions.


<h2>autensils</h2>

audio utensils. some of the modules are hardware dependant.

<h4>Wav</h4>

wave file parsing module.

<h4>Wav Stream</h4>

streams wave file data out of the specified stream interface.

<h4>Siggen</h4>

streams a synthesised sine wave out of the specified stream interface.


<h2>cutensils</h2>

software utensils, written in c. all the software in cutensils is in pure hardware independent c (can run on a Linux PC)

<h4>Logger</h4>

This is a general logging module.
supporting logging:
 - to file
 - to serial port
 - to socket
 - or any other file like device
 - optionally with ansi colour
 - optionally with timestamp

<h4>Confparse</h4>

configuration file parser / writer. supports:
 - key value storage in files
 - inline / whole line comments with the '#' character
 - insertion / deletion / reading of key value pairs
 - iteration over all key value pairs.

```
# Eg, example config file

# whitespace is ok

mykey myvalue
an_integer 1234
more data # inline comment
# spaces inside keys or values not allowed
mykey my value # illegal
```

<h4>Strutils</h4>

String handling utilities.

<h2>graphics</h2>

thread safe lcd graphics library that works on top of the lcd driver in stm32-device-support. the library provides...

 - smooth fonts
 - text
 - shapes - lines, squares, circles. etc
 - images - just bitmaps stored in rom
 - widgets - touch keys, statusbar
 
<h2>like-posix</h2>

A posix style system call library.

relies upon:
 - FatFs by ChaN
 - FreeRTOS
 - cutensils
 - minlibc
 - the file likeposix_config.h (modified for a specific project)

To find which system calls are implemented, see:
 - appleseed/like-posix/syscalls.c
 - appleseed/minlibc/*.c
 
in general support for time.h, stat.h, dirent.h, stdio.h, stdlib.h, string.h, termios.h, unistd.h is ongoing.
where a function exists in newlib but is very large, it will be replaced. 
where a function does not exist in newlib, it should be implemented.


<h4>like-posix configuration</h4>

the file likeposix_config.h is required at project level.

``` c

/**
 * sample configuration for the like-posix module.
 */

#ifndef LIKEPOSIX_CONFIG_H_
#define LIKEPOSIX_CONFIG_H_

/**
 * fudge factor for the file table, presently can be any value higher than STDIN_FILENO...
 */
#define FILE_TABLE_OFFSET		10
/**
 * the maximum number of open files/devices
 */
#define FILE_TABLE_LENGTH 		32
/**
 * the maximum number of installed devices, maximum of 255
 */
#define DEVICE_TABLE_LENGTH 	10
/**
 * location where devices get installed to. 
 * this directory is special, dont write regular files here :)
 * it is reserved for devices.
 */
#define DEVICE_INTERFACE_DIRECTORY 	"/dev/"
/**
 * this is a hack that adds an ofset in seconds onto the time returned by time/gettimeofday.
 * corrects time set by NTP for your timezone. 12 hours for NZT
 */
#define TIMEZONE_OFFSET (12 * 60 * 60)
/**
 * enable full integration of lwip sockets in likeposix
 */
#define ENABLE_LIKEPOSIX_SOCKETS    1

#endif /* LIKEPOSIX_CONFIG_H_ */

```

<h4>Base Filesystem</h4>

Many projects are better when based on a structured file system.
like-posix provides a filesytem template that can be installed on an SD card.
It is loosely structured, as follows:

 - /dev
    - this directory is generated by the system.
 	- devices such as serial and USB ports installed here, IO may be performed on them just like normal files.
 	- for serial devices the file naming convention will be "ttySx", starting at 0
 - /var/log
 	- log files are stored here, syslog, bootlog and errorlog for example
 - /var/lib/httpd
 	- default location for files served by an http server
 - /etc/network
 	- network configuration files live here: interface, ntp, resolv...
 - /etc/logging
 	- logging configuration files live here: logging.conf
 - /etc/shell
 	- shell server configuration files live here: shelld_config
 - /etc/echo
 	- echo server configuration files live here: echod_config
 - /etc/http
 	- http server configuration files live here: httpd_config
 - /rom
    - firmware images for use by the sdcard bootloader live here


<h2>minlibc</h2>

tiny implementations of c standard library functions.

The code is targeted for use on small embedded targets where memory is limited.

<h4>stdio</h4>

the minimal standard io code has some dependancies - the functions _read, _write, _lseek, _ftell 
must be defined somewhere in your project.

under Appleseed they are defined in syscalls.c

supported string formatting:



| % format	| operation |
| ----------|-----------|
| %		| double % prints % symbol |
| c		| print character |
| s		| print null terminated string |
| i		| print signed int |
| d		| print signed int |
| u		| print unsigned int |
| x		| print unsigned int, in lower case hexadecimal format |
| X		| print unsigned int, in upper case hexadecimal format |
| p		| print pointer address, in lower case hexadecimal format, preceded by '0x' |
| f		| print floating point value |

|modifier|operation|
|------------|---------|
| . or 0, space	| left padding with space and 0 characters are supported by i, u, d, x, X, p |
| .         | when used with the f type, as in %.3f, specifies the number of decimal places|
| #     	| a '0x' precedes hexadecimal formatted numbers when # is inserted. supported by x, X |
| +			| a + preceeds numeric formatted numbers when + is inserted, and the number is > 0. supported by i, u, d, f: |
| l,h		| ignored. normally specify width of the data type as long or short respectively|
| -         | ignored.


<h4>Testing</h4>

Setting up greenlight

```
git clone https://github.com/drmetal/greenlight.git
cd greenlight
echo "export PATH=$PATH:$PWD" >> ~/.bashrc
source ~/.bashrc
```

Building and runnning the tests

```
cd appleseed/minlibc/test
./runtest.sh
```

<h2>nutensils</h2>

network oriented utilities.

relies upon:

 - the cutensils module

<h4>Socket</h4>

simple socket utilities that can simplify socket programming at application level.

<h4>HTTP</h4>

this is a collection of HTTP protocol utilities. These build upon the nutensils socket module.
includes HTTP server and client modules.


<h4>Threaded Server</h4>

this builds on the the socket server module to provide a server that spawns a thread to run each new connection as a separate instance.
this is used by the HTTP and Shell servers.

<h4>Shell</h4>

The shell is an extensible multi threaded server that can run user commands, supports scripting, can function on files, serial ports and sockets.


<h4>jsmn extensions</h4>

provides a JSON object/array accessor API supporting keying/indexing and iteration.


<h2>Third Party</h2>

There are several third party libraries used by Appleseed.

<h4>LwIP</h4>
obtained from:

http://savannah.nongnu.org/projects/lwip/

currently at V1.4.1

<h4>FatFs</h4>

obtained from: http://elm-chan.org/fsw/ff/00index_e.html

 - Appleseed adds a diskio interface for stm32 microcontrollers.
 - Appleseed additions rely upon the cutensils module 
 - Appleseed additions rely upon the stm32-device-support module, for sdcard.h and sdcard driver implementation(s)
 
currently at V0.11

<h4>FreeRTOS</h4>

obtained from: 

http://www.freertos.org

currently at V8.0.0:rc2

<h4>JSMN</h4>

obtained from:

bitbucket.org/zserge/jsmn

Note: can be used in conjunction with the Appleseed JSON API  **jsmn_extensions**.

<h4>STM32Fxx Standard Peripheral Libraries</h4>

both STM32F1 and STM32F4 libraries arev included as well as their respecive CMSIS bundles.


