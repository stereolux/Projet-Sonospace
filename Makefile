erase:
	sudo dfu-programmer atmega16u2 erase

reset:
	sudo dfu-programmer atmega16u2 reset

hiduino:
	sudo dfu-programmer atmega16u2 flash firmwares/HIDUINO_MIDI.hex

usbuno:
	sudo dfu-programmer atmega16u2 flash firmwares/usbserial_uno_16u2.hex --suppress-bootloader-mem

usbmega:
	sudo dfu-programmer atmega16u2 flash firmwares/usbserial_mega_16u2.hex --suppress-bootloader-mem

midi: erase hiduino reset

uno: erase usbuno reset

mega: erase usbmega reset

.PHONY: erase reset hiduino usbuno usbmega midi uno mega