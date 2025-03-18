# SPIFFS-Download


## Overview

`SPIFFS-Download` is a project that allows your ESP32 microcontroller to download resources from a web server and store them in the SPIFFS (Serial Peripheral Interface Flash File System). 

## Features

- Download resources from a web server using HTTP(S)
- Store downloaded files in SPIFFS on the ESP32
- Measures download speed
- Handles connection drop and storage limits
  

## Requirements

- ESP32 Development Board
- Arduino IDE
- SPIFFS filesystem
- Wi-Fi connection
- Arduino HTTPClient library (or similar for handling web requests)
