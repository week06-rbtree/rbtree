.PHONY: clean

CFLAGS=-Wall -g

ifeq ($(SENTINEL), 1)
	CFLAGS += -DSENTINEL
endif

driver: driver.o rbtree.o SENTINEL=$(SENTINEL)

clean:
	rm -f driver *.o
