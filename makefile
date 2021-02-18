src = $(wildcard *.c)
obj = $(src:.c=.o)

CFLAGS += -Wall

tasks: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) tasks

check: tasks
	@echo "not completed yet"
	./tasks "task hello with label general and priority 3"
