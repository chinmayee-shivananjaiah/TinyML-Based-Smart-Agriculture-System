# TinyML-Based-Smart-Agriculture-System
TinyML-based soil analysis and crop suitability classification using STM32 NUCLEO-F446RE with an INT8 Edge Impulse model.

## Features

- Soil moisture and pH sensing
- Rainfall and humidity inputs
- INT8 TinyML model using Edge Impulse
- Neural-network architecture: 4 → 16 → 8 → 3
- Soil classification: Poor, Moderate, Suitable
- Rule-based crop recommendation
- On-device inference on STM32
- UART output at 115200 baud

## Input

[Moisture, pH, Rainfall, Humidity]

## Output

Poor → Soil improvement required  
Moderate → Maize / Millets  
Suitable → Rice / Wheat / Maize

## Hardware

- STM32 NUCLEO-F446RE
- Soil Moisture Sensor
- Soil pH Sensor

## Software

- STM32CubeIDE
- Edge Impulse
- C/C++
- PuTTY

## Model Results

- Validation Accuracy: ~80.6%
- Independent Test Accuracy: ~44.17%
- Estimated Inference Latency: ~1 ms

## Project Flow

Sensors / Inputs
        ↓
STM32
        ↓
INT8 TinyML Model
        ↓
Soil Classification
        ↓
Crop Recommendation
        ↓
UART Output

## Prototype Note

The pH sensor ADC is currently read by the STM32, but pH = 7.0 is used as a temporary test value. Rainfall and humidity are currently supplied as test inputs.
