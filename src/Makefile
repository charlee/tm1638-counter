ifeq ($(OS),Windows_NT) 
RM = del /Q /F
CP = copy /Y
ifdef ComSpec
SHELL := $(ComSpec)
endif
ifdef COMSPEC
SHELL := $(COMSPEC)
endif
else
RM = rm -rf
CP = cp -f
endif


CC := sdcc
PACKIHX := packihx

.PHONY: all clean

all: tm1638-counter.hex

clean:
	-$(RM) -f *.asm *.lk *.lst *.map *.mem *.rel *.rst *.sym *.asm *.ihx *.hex

tm1638.rel:	tm1638.h tm1638.c
	$(CC) -c tm1638.c

main.ihx: tm1638.h main.c tm1638.rel
	$(CC) main.c tm1638.rel

tm1638-counter.hex: main.ihx
	$(PACKIHX) main.ihx > tm1638-counter.hex

