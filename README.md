Arduino Motion Logger
=====================

This code tracks PIR events using an SD shield with Real time clock.

Usage
-----

First time usage of the SD card shield requires setting the RTC. Uncomment `set_rtc_time_from_compile()` in the sketch, and upload. It will blink quickly until power is removed.

Now comment out the `set_rtc_time_from_compile()` method and upload again. It will blink 10 times very fast to indicate its succesfully initialized and ready to log data.

![CC0](http://i.creativecommons.org/p/zero/1.0/88x31.png) By Kevin Alford (zeroeth)
