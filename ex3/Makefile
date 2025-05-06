TARGET = pi_estimation
SRCS = pi_estimation.c

CFLAGS := $(shell xeno-config --cflags --alchemy)
LDFLAGS := $(shell xeno-config --ldflags --alchemy)

all:
	gcc $(SRCS) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)
