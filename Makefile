#
VERSION=1.11

releases/passwordvault-v$(VERSION).uf2: .pio/build/seeed_wio_terminal/firmware.bin
	python uf2conv.py -c -b 0x4000 -o $@ $<

