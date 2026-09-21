# Thermal Tracking Automatic Fan

A thermal tracking fan developed for my EGR 375 Mechatronics
course at Central Michigan University.

## Overview

The system uses an MLX90640 infrared thermal camera to detect
and track a person based on temperature. An ESP32 processes the
thermal data and controls a two-axis pan/tilt mechanism to keep
the detected heat source centered.

## Features

- Real-time thermal tracking using a 32x24 MLX90640 sensor
- ESP32-based control system
- Two-axis servo-controlled pan/tilt mechanism
- Proportional control for target tracking
- Automatic search mode when no target is detected
- MOSFET-controlled 120 mm cooling fan

## Hardware

- Arduino Nano ESP32
- MLX90640 thermal camera
- Lynxmotion pan/tilt mechanism
- 2x high-torque servos
- 120 mm fan
- MOSFET fan controller
- 12 V power supply
- UBEC voltage regulator

## Software

The ESP32 reads the 768 temperature values produced by the
MLX90640 and identifies the hottest region above a temperature
threshold. The target's position is compared with the center of
the camera's field of view.

A proportional control algorithm then adjusts the pan and tilt
servos to keep the target centered.

## What I Learned

This project gave me hands-on experience with:

- Embedded programming
- I2C communication
- Sensor integration
- Servo motor control
- Power electronics
- Control systems
- Hardware debugging and testing

## Images

[Add project photos here]

## Demo

[Add a link to a demonstration video here]
