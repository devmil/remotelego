#!/bin/bash
dfu-util -d 2b04:d058 -a 0 -s 0x08020000 -D duo-system-part1-v0.2.3.bin
dfu-util -d 2b04:d058 -a 0 -s 0x08040000 -D duo-system-part2-v0.2.3.bin
dfu-util -d 2b04:d058 -a 0 -s 0x080C0000 -D espruino_1v85_redbearduo_v1.0.0.bin
dfu-util -d 2b04:d058 -a 2 -s 0x140000 -D duo-fac-tinker-v0.2.3.bin