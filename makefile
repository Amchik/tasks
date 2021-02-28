NAME = tasks
NAME_TEST = test
BIN = bin
OBJ = obj

SOURCES 		 = $(wildcard *.c)
SOURCES_TEST = $(wildcard tests/*.c tests/testoasterror/src/*.c)
OBJECTS 		 = $(patsubst %.c,$(OBJ)/%.o,$(SOURCES))
OBJECTS_TEST = $(patsubst %.c,$(OBJ)/%.o,$(SOURCES_TEST))

CFLAGS += -Wall -Wextra -std=gnu99

$(OBJ)/%.o: %.c
	@echo -e "\e[1mBuilding \e[4m$@\e[0m"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BIN)/$(NAME): $(OBJECTS)
	@echo -e "\e[1mLinking \e[4m$@\e[0m"
	@mkdir -p $(@D)
	@$(CC) $(LDFLAGS) -o $@ $^

$(BIN)/$(NAME_TEST): export CFLAGS+= -D__HAS_TEST_EXECUTABLE
$(BIN)/$(NAME_TEST): $(filter-out $(OBJ)/main.o,$(OBJECTS)) $(OBJECTS_TEST)
	@echo -e "\e[1mLinking \e[4m$@\e[0m"
	@mkdir -p $(@D)
	@$(CC) $(LDFLAGS) -o $@ $^

check: $(BIN)/$(NAME_TEST)
	./$(BIN)/$(NAME_TEST)

cleanobj:
	@echo "Cleaning objects"
	@rm -fv $(OBJECTS) $(OBJECTS_TEST)

clean: cleanobj
	@echo "Cleaning executables"
	@rm -fv $(BIN)/$(NAME) $(BIN)/$(NAME_TEST)

# vim: tabstop=2 shiftwidth=2
