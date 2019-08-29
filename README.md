# W.U.C.Y.  

  **Wearable**  
  **Unlimited** (open source general-purpose)  
  **Computer**  
  **Yeah..!**  

## Idea

It's not a watch.
It's a Computer on your wrist.
It will not make your life easier.
It's to have fun.  

Nor it's a smartwatch.
I dislike the "smart" part in tech.
It's stupid.

## Vision

My vision is to create hardware & software platform and spark a community of hackers around it :). I encourage you to help me build a community around this project for nerds like you and me (it' a compliment!). I'm not sure how this github thing works, I'm an amateur hacker so DM me if you think you can help me kickstart this project, although I'm quite happy working alone in current progress stage.

Maybe someone could help me to create a bootloader that could enable loading precompiled applications such as a game from storage to RAM and run it within OS. Or something simmilar.

### Hardware

_Highest priority from top to bottom. 
(if less significant additions will get hard to implement cost/time/skill-wise along the progress of building it I'll consider to do some cuts down the list.)_

* **Computer - ESP32-PICO-D4** MCU (WiFi & BLE).
* **Display - SSD1351** (128x128 18-bit color OLED).
* **Storage - microSD** card slot.
* **Connectivity - USB type C.** (micro usb might also do, but i Prioritize C because it's a better port by all mannerse except cost and   ease for hand-soldering). USB for for programming & Charging. (AT commands?).
* Battery - 280 mAh Li-po 303030 (found on ebay, it fits my CAD model really well and is cheap althoug it's specified energy density sits around 380 Wh/L while current technology could be almost a double of that - 693 Wh/L, 500 mAh @ those dimensions).
* ??? Charger - Charger IC.
* ??? Programming - USB to UART converter.
* ??? Sensors - Accelerometer and Gyroscope.
* Real-time clock - 32K Quartz.
* Input - 4 buttons. Scroll wheel, and one additional on other side.
* Indication - single color or RGB LED (depends on IO pins left on MCU).
* Vibration - tiny motor (maybe on rev.2).

### Software 
Custom therram kernel wrapped in FreeRTOS with basic but sexy GUI, easy access to system hardware IO, external file storage etc. It has to run low power and be flexible - save power as much as possible while not doing anything but leave it flexible for burst of speed. Will try to make dynamic clock speed. Advanced control on CPU cores.  Another key factor is to make an API framework for easy application developement. All code in C, no Arduino or simmilar frameworks, I want this wearable to be fast, open and sexy. 

### License
All of my contribution will be under Open Hardware and Open Software licenses (not sure which, never done this, lol). To be safe, I will not release any significant source files until I work that out. I would like some help and suggestions for this cause.

[More info in wiki](https://github.com/therram/thera/wiki).
  
