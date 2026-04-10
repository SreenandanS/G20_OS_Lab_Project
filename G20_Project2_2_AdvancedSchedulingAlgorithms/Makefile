CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -O2 -Iinclude

TARGET := bin/scheduler_sim
SRCS := \
	src/main.c \
	src/core/parser.c \
	src/core/report.c \
	src/core/simulator.c \
	src/schedulers/mlfq.c \
	src/schedulers/lottery.c \
	src/schedulers/edf.c \
	src/schedulers/custom.c
OBJS := $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS) | bin
	$(CC) $(CFLAGS) $(OBJS) -o $@

bin:
	mkdir -p bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) --algo custom --cpus 4 --input data/workloads/sample_mixed.csv --output-dir out/sample_run

clean:
	rm -f $(OBJS) $(TARGET)
