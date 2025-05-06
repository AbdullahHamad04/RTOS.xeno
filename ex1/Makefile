TARGET = periodic_task
SRCS = periodic_task.c

CFLAGS := $(shell xeno-config --cflags --alchemy)
LDFLAGS := $(shell xeno-config --ldflags --alchemy)

all:
	gcc $(SRCS) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

