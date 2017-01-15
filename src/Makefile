
BUILD ?= normal

build:
	platformio run -e ${BUILD}

upload prog:
	platformio  run -e ${BUILD} -t upload

monitor:
	platformio  serialports monitor -b 115200

clean:
	platformio run -t clean

archive:
	(cd .. && git archive --format=zip -v --prefix=solarsystem-controller/ -o solarsystem-controller.zip HEAD)

RELEASE:=$(shell date +"%Y-%m-%d")
OUTPUT:=solarsystem-$(RELEASE)
release:
	@echo $(RELEASE)
	rm -rf .pioenvs $(OUTPUT)
	make archive
	make BUILD=faster
	make BUILD=normal
	mkdir $(OUTPUT)
	cp .pioenvs/faster/firmware.hex $(OUTPUT)/firmware-faster-clock.hex
	cp .pioenvs/normal/firmware.hex $(OUTPUT)/firmware-normal.hex
	cp ../win/bin/Release/SolarsystemController.exe $(OUTPUT)/SolarsystemController.exe
	mv ../solarsystem-controller.zip $(OUTPUT)/
	apack $(OUTPUT).zip $(OUTPUT)/
	rm -rf $(OUTPUT)/
