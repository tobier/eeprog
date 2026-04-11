# eeprog

A CLI tool for programming 28C64 EEPROMs over serial.

## Setup

```bash
# Create and activate a virtual environment
python -m venv .venv

# Windows
.venv\Scripts\activate

# macOS/Linux
source .venv/bin/activate

# Install the package in editable mode
pip install -e .
```

## Usage

```
# Auto-detect the programmer device
eeprog read output.bin
eeprog write firmware.bin

# Specify a port explicitly
eeprog --port COM3 read output.bin
eeprog --port /dev/ttyUSB0 write firmware.bin
```
