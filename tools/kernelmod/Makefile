ifndef KERNELDIR
	KERNELDIR  := /lib/modules/$(shell uname -r)/build
endif

include $(PWD)/src/Makefile.src

obj-m += ssd1306.o
ssd1306-objs := ssd1306_main.o $(OBJS_C)

EXTRA_CFLAGS += -I$(PWD)/../../src -std=gnu99 -Wno-declaration-after-statement

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install
	/sbin/depmod -ae


