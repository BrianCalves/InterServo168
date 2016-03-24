#
# cu --line /dev/tty.usbserial-ACDC1234 --speed 9600 --parity=none
#

ATPRG		=	avrdude
CC		=	avr-gcc
CCDEFINES	=       -D F_CPU=16000000L -D __STDC_LIMIT_MACROS
CCFLAGS		=	-g -Os -Wall -mmcu=atmega168 -mcall-prologues $(CCDEFINES) $(INCDIRS)
CCC		=	avr-g++
CCCFLAGS	=	-g -Os -Wall -mmcu=atmega168 -mcall-prologues $(CCDEFINES) $(INCDIRS)
ECHO		=	echo
LD		=	avr-g++
LDFLAGS		=	-g -Os -Wall -mmcu=atmega168 -mcall-prologues $(CCDEFINES) -Wl,--gc-sections -Wl,--relax -Wl,-u,vfprintf $(OBJS) $(LIBOBJS) -lprintf_flt -lm
OBJCP		=	avr-objcopy
TARGET		=	Main.hex

INCDIRS		=	

OBJS		=	

LIBOBJS		=	


.PRECIOUS:	%.hex %.elf %.o
.SUFFIXES:	.hex .elf

%.hex:	%.elf
	@$(ECHO) "    o Creating $@"
	@$(OBJCP) --remove-section=.eeprom --output-target=ihex $< $@

%.elf:	%.o $(OBJS)
	@$(ECHO) "    o Linking $@"
	@$(LD) $(LDFLAGS) -o $@ $<

%.o:	%.cc
	@$(ECHO) "    o Compiling $<"
	@$(CCC) $(CCCFLAGS) -c -o $@ $<

%.o:	%.c
	@$(ECHO) "    o Compiling $<"
	@$(CC) $(CCFLAGS) -c -o $@ $<

all:	$(TARGET)

arduino:	$(TARGET)
	@$(ECHO) "    o Programming Arduino"
	@$(ATPRG) -P /dev/tty.usbserial-ACDC1234 -b 19200 -p atmega168 -c arduino -e -U flash:w:$(TARGET)

tidy:
	@$(ECHO) "    o Tidying"
	@$(RM) *.elf *.o

clean:
	@$(ECHO) "    o Cleaning"
	@$(RM) *.hex *.elf *.o


