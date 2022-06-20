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
- Lab 4 - Receive data from the acceleremator in a unique Thread
  - Wiggling the accelerometer around should result in a separate processes printing a running average value, min and max in meters per second squared.
- Lab 5 - Print the x, y, z data in the random inorder in which the threads complete
  - The Data from the acceleremator should stored in a file
- Lab 6 - Write a kernel driver that exposes the x, y and z accelerometer data to the user space via the sys file system.

## Required Libraries

The C library WiringPI for I2C Raspberry PI

```
sudo apt-get install wiringpi
```

## Helpful Commands

enable i2c in raspi-config:

```
sudo raspi-config
5 (interface option)
enable i2c
```

Install I2C Tools

```
sudo apt-get update

sudo apt-get install i2c-tools
```

i2c commands

```
i2cdetect

i2cdump

i2cset

i2cget
```
