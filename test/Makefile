.PHONY: test

CFLAGS=-I ../src -Wall -g #-DSENTINEL

ifeq ($(SENTINEL), 1)
	CFLAGS += -DSENTINEL
endif

test: test-rbtree
	./test-rbtree

test-rbtree: test-rbtree.o ../src/rbtree.o

../src/rbtree.o:
	$(MAKE) -C ../src rbtree.o SENTINEL=$(SENTINEL)

clean:
	rm -f test-rbtree *.o
