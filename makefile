src = $(wildcard *.c)
obj = $(src:.c=.o)

testsrc = $(wildcard tests/*.c) $(wildcard tests/testoasterror/src/*.c)
testobj = $(testsrc:.c=.o)

CFLAGS += -Wall

tasks: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

test: export CFLAGS+= -D__HAS_TEST_EXECUTABLE
test: $(obj) $(testobj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) $(testobj) tasks test

.PHONY: check
check: clean test
	./test
