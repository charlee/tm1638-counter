TM1638 Clock
=============


This is a multi-purpose clock based on [TM1638 module](https://www.amazon.ca/8-Bit-Digital-Tube-TM1638-Module/dp/B00GNSO62Q).

## Features

This project featuers the following functionalities.

* Adjustable real time clock
* Timer
* Stopwatch
* Counter
* Fitness Timer


The TM1638 module contains 8 LEDs. In this project, these LEDs are used as functionality indicators.

When the "Func" button is pressed, the 1st ~ 8th LEDs will emit one by one. After switched to a functionality,
the 7segs will show the name of the functionality shortly.

1. CLOCK
2. TIMER
3. STOPWTCH
4. COUNTER
5. FITNESS


Development
----------------


Install [SDCC](http://sdcc.sourceforge.net/)

Install `make`:

    > chocolatey.exe install make