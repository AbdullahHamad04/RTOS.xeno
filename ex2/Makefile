TARGET = race_condition
SRCS = race_condition.c

CFLAGS := $(shell xeno-config --cflags --alchemy)
LDFLAGS := $(shell xeno-config --ldflags --alchemy)

all:
	gcc $(SRCS) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

