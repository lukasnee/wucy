# W.R.C.  

  **Wearable**  
  **Retro**  
  **Computer**  

`Technically a neat watch.`  
`Philosophically - truly personal wearable.`  
`For hackers. For fun.`  
 
### Vision

is to create retro watch-like open-source wearable computer. Technically you could call this an open source smartwatch, but I don't like the word "smart" - in fact - computers are dumb, they do only what's told as long as they are open. I'd like to keep it that way. So I'd rather want wucy to be _honest_ than _smart_. My plan is to create hardware & software platform, spark a community, have fun and make something special. :)

Currently I am working on this alone, but I encourage you to join.  
I'm still learning how github works, I'm an amateur hacker, DM me if you think you can help. About the progress down at the bottom.

### Hardware

Loose initial idea for hardware.
_(Highest priority from top to bottom)_

* **CORE - ESP32-PICO-D4** (very powerful SIP: Xtensa® dual-core 32-bit LX6 microprocessors, up to 600 MIPS, integerated WiFi & BLE, 448 KB ROM, 520 KB SRAM, integrates 4 MB of external SPI flash, Quad-SPI supports multiple flash/SRAM chips).
* **Display - SSD1351** (128x128 18-bit color OLED). I think it is perfect for this application. Costs just above 10 $ on aliExpress.
* **Digital inputs - 4 (?).** Side scroll wheel (tri-state) and one additional tactile push button on contrary side (could change).
* **Storage - microSD** card slot for mass storage.
* **Battery - 280 mAh Li-po (303030).** Found on ebay, it fits my CAD model really well and is cheap though it's specified energy density sits around 380 Wh/L while with current technology you could get almost double of that - 730 Wh/L, +500 mAh at those dimensions.
* **Connectivity - USB type C.** USB for for programming & Charging File access if possible (need to research that) ? Ability to use AT commands (micro USB might also do, but i prioritize type C because it is becoming the new convenient standart, also it is a much better mechanical part. Only drawbacks are cost and difficulty to hand-solder). 
* **Programming - CP2102**  USB to UART bridge (Espressif's recommendation).
* ??? **Power management** - battery management IC, 3.3V LDO.
* ??? **Real-time clock** - 32K Quartz(?)(todo: research RTC capabilities inside ESP32, my priority to use as little external parts as possible).
* ??? **Extra flash - better safe than sorry.** Will do if there will be enough space left. 
* ??? **Sensors** - Accelerometer and Gyroscope at least.

* ??? Indication - single color or RGB LED (depends on IO pins left on MCU).
* ??? Vibration - tiny motor (maybe on rev.2 if power consumption will be efficient).

If there will be a demand, I might consider making wucy's DIY kits or prebuild hardware making them plug&play, however that sounds like a devastating labour by hand... Thats a problem for future, if there will ever be one, lol.

### Software 
Custom kernel wrapped in FreeRTOS with neat GUI, easy access to system hardware IO, external file storage etc. I want great flexibility on power usage and computing performance. I want to make an API framework for easy application developement within the kernel and on top of the system. I want this wearable to be fast and efficient. Currently developing on ESP-IDF framework, I think ESP32 is a great SoC to start on, however I do think about portability, so I structure my code accordingly.  

### Progress

* 2019-09-28 
Very much a work in progress. Nothing really works as a whole. I have written a bit of kernel and GUI framework - it is a year old project started on STM32, it already has working button driver, animated menu dropdown list, visual RTC, animated watch face, time editing, settings menu, fps indication, freeRTOS run time stats window. Currently that GUI is desinged around monochrome SSD1306 OLED display, so I will need to reconstruct and port that progress to my current ESP-IDF platform. Now I am working on SSD1351 driven OLED display which is bigger and full color. Already written a driver with windows support, early gfx library, both need a lot of tweaking and new features. Considering what display interface to use -serial or parallel. I really do want the display to run at 60 fps, but it seems it is impossible with SPI. Theoretically I can get up to around 50 fps, which might be good enough. I have to decide that to move on with the hardware. I already have crude Fusion360 model, case-less parts composition, board dimensions planned out, started designing PCB and selecting parts...

### Future ideas

Maybe create a bootloader that could enable loading precompiled applications such as games from SD card storage to RAM and run it within the OS. I have no skill on this, so contact me if you think you can help.

### License
All of my contribution will be under open hardware and open software licenses, however I'm very new to licensing.

I want this project to be purely open-source and stay that always, I'm a believer in free software, world is to corrupt already, so I decided to license software under copy-left GPL-3.0 (at least for now). 

Hardware licensing will come into consideration later, currently I don't have any decent documentation to publish.

### Extra

Take everything with a grain of salt, I'm an inexperienced engineer. I don't know what I'm doing.
Luckily I have this idea and pursuit of making it real.

Some of my inspirations: 
Pebble watch; Zak Kemble's N|Watch (never had one tho);
Steve Wozniak, Unexpected maker, Mohit Bhoite, Linus Torvalds.

[More info in wiki](https://github.com/therram/thera/wiki).
  
![](https://www.draw.io/?lightbox=1&highlight=0000ff&edit=_blank&layers=1&nav=1&title=wucy-code-structure.drawio#R7V1bc5s8Gv41ntm9MKMz4tJJmrZf202mSefrXnWITRy22HgxzmF%2F%2FUqYk4SwMQEct007LQgsjJ7nPeqVMsLni%2Bf3kbt6%2BBLOvGCEwOx5hC9GCGGEgPhPtrxsWyCy05Z55M%2FStqLhxv%2BflzZmt238mbdWbozDMIj9ldo4DZdLbxorbW4UhU%2FqbfdhoD515c69SsPN1A2qrX%2F7s%2Fhh28opKNo%2FeP78IXsyBOmVhZvdnDasH9xZ%2BFRqwu9G%2BDwKw3h7tHg%2B9wI5etm4bD93WXM1%2F2KRt4ybfODTBwrvLz5FTzZH%2F8JXZ7dPV9%2FGMHuRRzfYpK%2Bcft34JRuDKNwsZ57sBo7w2dODH3s3K3cqrz4J2EXbQ7wI0svrOAp%2F5mMlW%2BaBu5YoAHF87wfBeRiEkThfhkvRxZkbTVPYaf7x7JYRwmQi%2F4grM3f9kH%2BJ6sun4%2FHoRbH3XGpKB%2BO9Fy68OHoRt6RXSUaxlJo4Q%2B6pwBln9zyUMQZOyq%2BUW%2FO872L4xUGKwCFoEOd3RYNyFQ2CnAoakJjQcGBfaFD0u6KRq4UdsoGMaGQC04NssN8VDV02GKmiUSMbvaEBye%2BKhi4bJjSI0W70hoZJT7FAPPbsThzM4%2BTF04ZIbxFPLN2lQSgeLjwt71D4UDdDjYlmFKiB%2BJn%2BV4aa9Ef85mN9H4o3Lw8n%2B%2B8mzC6M1wl5J%2BIGSFfPxcWsl8vI877eXt2UcNp2eDLwjTUPiwFUhQ8Y4GP9GZH96D35y5kMGwYVmK58WqSOuA2JBbiNEXAYpgSyyvjbtDr8maXtfvhNJnw7qOuVu6yXlXt34QcvW2kRl9zFKhkvjIkcVC949GJ%2F6lauJHefmz7jRr4b1N%2B%2Fdpfr8dqL%2FHvZlFzQZBaYRHaEqB4LUzE8sjWJ8%2FKzbLhoMmCi5UIey6dTOSRUjPK%2Be2F%2Bb0aZVt2gopstKvkV%2BY%2FERjRI4aDJO8qTmb9eBe5L%2BXryLOWmotstpHm3mYDIm0oiIq8nQiLbEzGRLTA5LYtK0Vr64lv5aDI0%2BaV8VAoholKMioHhxTCCl6Ids1L7U%2F6dkjYpTNmlVKDya7T0MSFYefu89Hwds%2BQ0B67cqNIpva%2FCu1xfbaWrRmWJsYtV3RR5gujuXXKDpP8q9Jdxogzo2YheSPnZxOFWGJIPuIE%2FX4rjwLuXXUlAhDwGk7Q5DldbEZ%2F6y%2FmtPLkYk240HgLMoqrOI1UthwzuGOrNyNj7jcz7y%2B9btaTZmMC%2Fi1z5HYY2PjM%2F8qaxH0q41uFGtnfiwSEVHJZ5a2UTZAAH95ViQQ38tw%2BTz%2BbhTk3VZtXW0%2BNGq0HPoJSqXZjXeH7KV3lTvh%2FkWHXdHZrJ6R7vg%2FYWJdWHSU3xc3bgZ4nrH9xo9iSGTRxO7taxCFETkULgs%2FviRbV47lXIanSbBsCmmDhVuFMBnHheVRMv%2FNksqONEEa%2BDCi1wR7SwtYiOGSI6zAbU1pmC2pXI8JaziZxLkCMrkxL%2BVIXHe%2Fbj7%2BmwyeN%2Fy2OLpmcXz6VLFy%2FZyVJ8%2B%2B9ZB%2FKk9Cl5WnwsOXsxwrJTWoUej6be%2FryB4OncixsoTW%2BmTJZUUS6hSA1aPWuLvMCN%2FUd1isWEbPqEa%2Bl%2FlEgEaMXma%2FZi%2B%2Brp5wqGVLuielfM0brajk6lq4Rs%2Bau%2Fgn%2F13kLj%2FAEyByOaWbu5uYCYwqqdufr8TnQOZpGAJNpl%2BfYpruMGv4hopDCEu9zAyt6yDaRBXu5uE8fSQuiYqGCcVBICMy0JYVd1%2FKCG3zhzoOHw98fLj01HW%2Fuou1r9gI3uPEk4qWNrKlJE%2BccFtMnE3F6b3dL%2Btre2GQ33m9s3ZW0Z0iNswjVgm1pbIoJ1bmv2FgBLyzf2bHEZr1UHap5Ej9lfolA8cDKdygTLboVtVAMmqd%2BZmTm24GtIMVQRe8ck9qQnsWcNqiMau%2BoWYFhx1xHe47CLk2sv8sW7yNBKdeKB5RCkKhKbsj2qJDnTu2yvXzJen5h%2BoVDXLxS29OYpIBVTpZGxZ93idGGZcopSByoUBRjvpmhBR6G2VTo6IkRqQUcT69tTNAt2ToyihCILc4ghtjnDufuT6UVILe4AG1IRhwLC7XbkxZxLnQGyH6gSmRHxFQblcj412xGZy1RuSmOCiEZjpxWNX%2BG1NU6SvCnKUtu2CMfEoTagFFBbc7psCwvKYUBsoVRaMpZCnFg5hadUCAMHOosHo2yX%2BrdNNg9YmOiU5W0Ye4BLt5fD8DTVLqbMkjMH%2BY%2FCNBsJH6sdcQmmwv3jGnGRRYZ1FiDs1ltoo2BpmatwD1FfwUBymgx0KulhPVvYmHOUW8JHqCGzsPtWxobB2Gea%2BXhtKZsxFf3%2B28fR3rnM%2BvzX25gPU8pRxSukxaeQdhNto0qahYNqms1U5dtfIcOOeq2OGVIzAzFUwWNXMxAI6OqC4gqGpnoH3fJ0iGGHSRMlGm1qYSDXMqyc56HpMC58zuMTsz4MQAvamNkMc8Y5VSNCBpCFMHAYpwBC4mgMap4zEZzlSEQKAIoYl2hhJxGBAnWgIyIGIsZx2LlRmHF1lwL6entezbC6q9VJTrYQrC28wQb9MeRUC0S4Lx8VNtUglOKyBhkDCwA8sArJ3vrUVAiuTN7oq1CaKgqmle5gnXO9K4MGxfvCkTAqg2KC9qTUQXXulWTJmuMphC4KpkrzJnykeAcY7U3wFVM4ACk5Guaw3TqlbgqnS0VxmpEuA5p0t57j1Rfy6qule1cT9dO7HQctN95yHUbr18Qrby%2B07URrvbVAFvexas%2FIifPtPhT%2Box%2B%2F%2FCGGRgxWWRBYXQ1gXMfcW3SMTd5tP8pi%2BuDNNoG%2FnMsXWc7Ev7fC5ox%2Fet4qafzDFbWK0FYzpOYtCPCQWmR3PWcDYrBddbqN6WXs5WkzNawa%2BuRFy6JYqYZbp7UkXF9AwKu0gNigQ3hv24SQPnQIMYE8uf7F8uhdEEJzNihjFkS2A5hjM5tkMrunBrxHpdFgNeDNxXbZ8eXkdmRaFbjNi5%2FmknTEiarHcVVgs5IZJa7pbRE6NTmDrTNaHKplhFy87yvKCJGa6xIxrj10tpycZrUAzcpT88ATq100L8RSi2UIHbjuig7nlMZhJDfA%2B2NUtGWKTJ0GwUVh6dFiWGpKf2pgrB%2FclTycRuF6fedG%2B3W%2BukdTeRjrd1SqOnYGdHuYIofadkCyYlvHhA5r3akp2fRbgTJmbD8qtjMoKqze59q9z8y%2BPZkqavjdzTWWT%2FqHOBh%2FvLj8564FIIdvxPELqNKxHrHluJcjNjYsP0w1Efo2UCKgVjbfOsQE9iBnWPOj8x1ylc20UHUYcW%2FDmAG7cy8NT%2B7pA75Ovpxk9AK56lQeP3qxO4xeyvU8jWfjgXhfJUJBoFWBcy06DcqZUxBOLELBjFo2hnltvBavQMeigBSV823DF4fL2v68zlSPZmwJYalAn6qP6S64ueJfrmY%2Fv8O%2F8F%2Fxtf%2FtcfPl%2FD%2Fm%2FRNrHCZ%2FkWzOnRuzz%2B6dF1yHaz%2Fdp%2BgujONwIW4I5IUzd%2Fpznpiyknt0n%2FwYDOJ2Syp3vdpuGn7vP0sDeJY8cpK1gqxFHD%2FE8SpxDS7FX2%2B9wshaCu6hy%2BSGtTj4EXlbjORJ4hj8uNwskx1gfpwF4fTnjwvfnUfuwlo%2FznMBqOgig0wc4HzZvKKfGDApKH0xURsNZUa43rgas8%2BtI1qjh5b6YQdFtI33XerGNKGOsNcm0%2FLCniF21zQij%2Br9gZ0Lq89duTNcolhN2c843EwfDkau34XVr0KOaELLSFVobZPMdjANagauC5%2Bik%2F2MZO5U2bBZnly7sXAblkmLkPscBM2st1ySCqu%2BxA69dizfQYiyBUpWG6rRAKUtvQUHWKUFVlwv%2F6XYGtY%2F6LTcdIQPX8AHLQA0CpPDObzD4z2UtTuN7H7WHnkFn8JaoO3QRPXYtClrRVCrL%2FQvWoZiaoPUX6uFFc2XmgLMVKbaeN9kUa9U3blj%2Bt5gLa2vOBpXiWMVmtCxbZ1fr6BqSXEzvduBaWuseKoJwN5sxvp1jrP2CxyQaQ%2FKvqYRzJj8ClM7HQeyBlDyGuVhQKlfHtvNJEI5Qik2jq0JYE51BiHhSvoFQRdEIfokYLb15xDTCWaitP2tBm2IchPex3%2BI0kbLH58nplnJtqFNu90ebJgvrkn3JmH7HMa%2BI5lsuJvl8o9WXORgy%2BFqRTIDjsV4yWVs5yBSJPzOjAp5ESO1OCl5jcO6iIdFNqliUNI2tYpFOBvOO%2Fmn4mzgA3h1LBpAbXku0n%2F5YON9p7WO8sTLQBA3qVmeBG60GJkyskdcXf0qm0AR0tIGGA%2B3la0ZiJ62n288nWvbRDUJgL9ibXUnJiEj59s2CcTW1%2BZip2USllBqMbnCJfvR1vxSx6KlKVvQ2zZVZjg63Xa1%2F12qqimudtxsnM16g9MFxKLEJjamgHNGGIHeGKq%2FkKV1QmtMkCWYXrCx0q%2FVlS0bpb%2FoqnR78Suu8Lv%2FAw%3D%3D)
