obj-m += sleep.o


VERSION = `uname -r`

all:
	make -C /lib/modules/$(VERSION)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(VERSION)/build M=$(PWD) clean
