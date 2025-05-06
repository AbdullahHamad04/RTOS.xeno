TARGET = reader_writer
SRCS   = reader_writer.c

CFLAGS := $(shell xeno-config --cflags --alchemy)
LDFLAGS := $(shell xeno-config --ldflags --alchemy)

all:
	gcc $(SRCS) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

