obj-m += main.o


VERSION = `uname -r`

KDIR = /lib/modules/$(VERSION)/build

all:
	make -C $(KDIR) M=$(CURDIR) modules
	cp -iv ./00-keyboards.rules /etc/udev/rules.d/.

clean:
	make -C /lib/modules/$(VERSION)/build M=$(CURDIR) clean
	rm -iv /etc/udev/rules.d/00-keyboards.rules
