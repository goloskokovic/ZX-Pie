# TK-Pie (a.k.a. ZX-Pie)

### A interface to connect a Raspberry Pi to ZX-Spectrum and clones.

TK-Pie is a baremetal OS, and Linux is NOT REQUIRED. It boot in less than one second!

### How it works

The interface listen for writes in ZX Spectrum VRAM and generate the video at Pi's HDMI output. You can connect your Speccy in any modern monitor or TV!  
But you not only add a new awesome video output with digital quality, but also the support for ULAplus. This feature was created by Andrew Owen and add new colors to the ZX Spectrum and there are already several games that support this mode. 

The key to achieve this "video conversion" is to use a "latch" to hold the last value written in Spectrum memory and a FIQ (Fast Interrupt Request) in Pi environment to rapidly respond to all write requests. But we also need a way to convert the TTLs 5V signals from Spectrum to 3.3V required by Pi inputs. So, we can solve both problems at same time, using a CPLD. It will act as a "middleman" latching the data and serving as 3.3V to Pi.   

### The O.S.

The PI_OS folder contains the source code for the baremetal boot. 

PI_SD are the files for the SD card. It's a precompiled kernel.bin (from the PI_OS folder) and the standard files to boot the Raspberry Pi. Just copy the files to a FAT32 formatted SD card, insert in your Pi Zero and you are good to go. 

The precompiled Kernel is for Raspberry Pi Zero only. If you wish to recompile the OS for other RPis (any RPi with 40 pins GPIO can be used) get the GNU ARM Toolchain at https://launchpad.net/gcc-arm-embedded/+download 

### CPLD

To latch and shift the levels to 3.3V any kind of CPLD 5V tolerant can be used. In my first test I use an EMP3064 combined with some 74HC245 as a proof of concept. The second interface use an EPM7128 (84 pins) direct connected from bus to RPi. My final board was designed using an EPM240 (I got a few of then for a good price, so...) This CPLD is "semi" 5V tolerant, it need to add a serie resistor with the 5V line to limit the current.

You don't need to use the EPM240, ANY CPLD 5V tolerant will do the trick, even Xilinx (XC95 series) and others can be used. The code for the CPLD is a vanilla VHDL without any fancy stuff. You can recompile in any IDE.

I precompiled POFs for EPM7128 and EPM240 (folder CPLD) according the pinout in my schematics (folder schematics). These POFs are compile with Quartus II (QSF to open the project also included).

### Gerber data
The gerber for the board as you see in photos folder is included. Have fun! 

### See in Action

You can find some TK-Pie videos here
https://www.youtube.com/watch?v=1tVO3xKxZTA
and here https://www.youtube.com/watch?v=D5r5TAMLlhw

(sorry, but videos are in portuguese!)


-----------------------------------------------------------------------------
TK-PIE

Copyright (c) 2016 - Victor Trucco


All rights reserved

Redistribution and use in source and synthezised forms, with or without
modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.

 Redistributions in synthesized form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.

 Neither the name of the author nor the names of other contributors may
 be used to endorse or promote products derived from this software without
 specific prior written permission.

 THIS CODE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.

 You are responsible for any legal issues arising from your use of this code.

