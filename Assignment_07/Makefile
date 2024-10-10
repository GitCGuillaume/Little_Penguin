SRC = test.c

OBJ = $(SRC:.c=.o)

TESTER = tester

VERSION = `uname -r`

.c.o:
	gcc -Wall -Wextra -Werror -c $< -o $(<:.c=.o)

obj-m += main.o

all:	$(TESTER)
	make -C /lib/modules/$(VERSION)/build M=$(PWD) modules

chmod:
	chmod -v 755 /sys/kernel/debug

clean:
	make -C /lib/modules/$(VERSION)/build M=$(PWD) clean
	rm -f $(OBJ) $(TESTER)

$(TESTER):	$(OBJ)
	gcc -Wall -Wextra -Werror -o tester $(OBJ)
