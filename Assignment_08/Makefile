SRC = test.c

OBJ = $(SRC:.c=.o)

TESTER = tester

obj-m += reverse.o

VERSION = `uname -r`

all:	$(TESTER)
	make -C /lib/modules/$(VERSION)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(VERSION)/build M=$(PWD) clean
	rm -f $(OBJ) $(TESTER)

$(TESTER):	$(OBJ)
	gcc -Wall -Wextra -Werror -o tester $(OBJ)
