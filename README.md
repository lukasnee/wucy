# W.U.C.Y.  

  **Wearable**  
  **Unlimited** (open source, general-purpose)  
  **Computer**  
  **Yeah..!**  

### Idea 
_...wucy's sacred text..._  
  
`It is not a watch.`  
  
`It's a computer on your wrist.`  
`It'll not make your life easier.`  
`It's explicitly 4 fun.`   
  
`Nor it's a smartwatch.`  
  
`"Smart"?`  
`You better be afraid of this devilish word.`  
`It's as dumb as any other computer.`  
`That's why computers are so lovable !`  
  
### Vision

My vision is to create hardware & software platform and spark a community of hackers around it :). I encourage you to help me build this community for nerds like you and me (that's a compliment !). If there will be a demand, I might consider making wucy's DIY kits or prebuild hardware making them plug&play, however that sounds like a devastating labour by hand... Well, thats a problem for future, if there will ever be one, lol.

I'm still learning how github works, I'm an amateur hacker so DM me if you think you can help me kickstart this project, although I'm quite happy working alone in current progress stage. Down at the very bottom about the progress.

### Hardware

_Highest priority from top to bottom. 
(if hardware features at the list's bottom will become hard to implement physical-space/cost/time/skill-wise, I might do some cuts)_

* **CPU - ESP32-PICO-D4** MCU (very powerful & magical SIP: Xtensa® dual-core 32-bit LX6 microprocessors, up to 600 MIPS, integerated WiFi & BLE, 448 KB ROM, 520 KB SRAM, integrates 4 MB of external SPI flash, Quad-SPI supports multiple flash/SRAM chips).
* **Display - SSD1351** (128x128 18-bit color OLED). I think it is perfect for this application. Costs just above 10 $ on aliExpress.
* **Digital inputs - 4 (?).** Side scroll wheel (tri-state) and one additional tactile push button on contrary side (could change).
* **Storage - microSD** card slot for mass storage.
* **Battery - 280 mAh Li-po (303030).** Found on ebay, it fits my CAD model really well and is cheap though it's specified energy density sits around 380 Wh/L while with current technology you could get almost double of that - 730 Wh/L, +500 mAh at those dimensions).
* **Connectivity - USB type C.** (micro USB might also do, but i prioritize type C because it is becoming the new convenient standart, also it is a much better mechanical part. Only drawbacks are cost and difficult for hand-soldering. USB for for programming & Charging File access if possible (need to research that) ? Ability to use AT commands.
* **Programming - CP2102**  USB to UART bridge (Espressif's recommends)
* ??? **Power management** - battery management IC, 3.3V LDO.
* **Real-time clock** - 32K Quartz(?)(todo: research RTC capabilities inside ESP32, my priority to use as little external parts as possible).
* **Extra Flash - better safe than sorry.** Will do if there will be enough space left. 
* ??? Sensors - Accelerometer and Gyroscope at least.



* Indication - single color or RGB LED (depends on IO pins left on MCU).
* Vibration - tiny motor (maybe on rev.2 if power consumption will be efficient).

### Software 
Custom therram kernel wrapped in FreeRTOS with basic but sexy GUI, easy access to system hardware IO, external file storage etc. It has to run low power and be flexible - save power as much as possible while not doing anything but leave it flexible for burst of speed. Will try to make dynamic clock speed. Advanced control on CPU cores.  Another key factor is to make an API framework for easy application developement. All code in C, no Arduino or simmilar frameworks, I want this wearable to be fast, open and sexy. 

### Progress

Maybe someone could help me create a bootloader(?) that could enable loading precompiled applications such as a game from storage to RAM and run it within OS.

### License
All of my contribution will be under Open Hardware and Open Software licenses (not sure which, never done this, lol). To be safe, I will not release any significant source files until I work that out. I would like some help and suggestions for this cause.

[More info in wiki](https://github.com/therram/thera/wiki).
  
