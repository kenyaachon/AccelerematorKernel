# AccelerematorKernel

This is a kernel that reads data from an acceleremator sensor using the I2C protocol

This project was part of lab for my CS COURSE 444 OPERATING SYSTEMS
For safety reasons, these labs were run on a Raspberry Pi to prevent causing serious damage to my personal working station

## Labs

- Lab 2 - wrote a program that does the following
  - Spawn child process
  - Child process generates random number
  - Sends random numbers to parent process via pipe
  - Parent process receives numbers via pipe.
  - Calculates a rolling avg, std, min and max. You can decide what is a reasonable window size
  - prints avg, std, min and max to stdout
- Lab 3 - Implement Lab 2 using threads

## Required Libraries

The C library WiringPI for I2C Raspberry PI

```
sudo apt-get install wiringpi
```
