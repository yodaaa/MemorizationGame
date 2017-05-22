# ここにプロジェクト名を記述(面倒ならdemoのままでもよい)
PROJECT =  demo

# ここにビルドに必要なソースファイル名を列記する
SRCS = user.c

# シリアルデバイス名
DEV = /dev/ttyACM0

# ここからは編集する必要はない
OBJECTS = $(SRCS:.c=.o)
MPU = atmega88pa
F_CPU = 8000000UL

MAPFILE = ${PROJECT}.map
LSSFILE = ${PROJECT}.lss
ELFFILE = ${PROJECT}.elf
HEXFILE = ${PROJECT}.hex
EEPFILE = ${PROJECT}.eep

CC = avr-gcc
CFLAGS = -mmcu=${MPU} -W -Wall -Werror-implicit-function-declaration -DF_CPU=${F_CPU} -Os
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums 
LDFLAGS = -mmcu=${MPU} -Wl,-Map=${MAPFILE}

all: clean ${HEXFILE}

${HEXFILE}: cboot0.hex ${OBJECTS}
	avr-gcc ${LDFLAGS} ${OBJECTS} -o ${ELFFILE}
	avr-objcopy -j.text -j.data -O ihex ${ELFFILE} ${HEXFILE}
	avr-objcopy -j.eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex ${ELFFILE} ${EEPFILE}
	avr-objdump -h -S ${ELFFILE} > ${LSSFILE}
	@echo
	@avr-size -C --mcu=${MPU} ${ELFFILE}

dl:	all
	stty -F ${DEV} 4800 cs8 -cstopb -parenb
	ascii-xfr -dsvn cboot0.hex > ${DEV}
	ascii-xfr -dsvn ${HEXFILE} > ${DEV}

cb: all
	xsel -c -b > /dev/null
	cat cboot0.hex | xsel -b -i
	cat ${HEXFILE} | xsel -a -b -i

cboot0.hex:
	/bin/echo -e ":020000020000FC\r\n:02000000FFCF30\r\n:101BE000F894EE27E0936E00E0937000E093C1005C\r\n:0C1BF000E1E0E5BFEE27E5BFA89502C2CA\r\n:021BFE00F00DE8" > $@

clean:
	rm -fr ${OBJECTS} ${MAPFILE} ${LSSFILE} ${ELFFILE} ${HEXFILE} ${EEPFILE} ${PROJECT}*~ ${PROJECT}.[iso] ./a.out cboot0.hex

%.o:	%.c Makefile
	${CC}  -c  $< -o $@ ${CFLAGS}
