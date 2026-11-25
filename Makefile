CC      = gcc
CFLAGS  = -Wall -Wextra -pthread -O0

# --- Part 1 ---
PART1_DIR   = src/part1
PART1_SRCS  = $(PART1_DIR)/philosophes.c \
              $(PART1_DIR)/producer_consumer.c \
              $(PART1_DIR)/reader_writer.c
PART1_BINS  = $(PART1_SRCS:.c=)

# --- Part 2 / tatas ---
# TATAS_DIR   = src/part2/tatas
# TATAS_SRCS  = $(TATAS_DIR)/philosophes_tatas.c \
#               $(TATAS_DIR)/producer_consumer_tatas.c \
#               $(TATAS_DIR)/reader_writer_tatas.c
# TATAS_BINS  = $(TATAS_SRCS:.c=)

# --- Part 2 / tas ---
# TAS_DIR     = src/part2/tas
# TAS_SRCS    = $(TAS_DIR)/philosophes_tas.c \
#               $(TAS_DIR)/producer_consumer_tas.c \
#               $(TAS_DIR)/reader_writer_tas.c
# TAS_BINS    = $(TAS_SRCS:.c=)

# --- Compare ---
# COMPARE_BIN = src/part2/compare_lock

ALL_BINS = $(PART1_BINS) #$(TATAS_BINS) $(TAS_BINS) $(COMPARE_BIN)

.PHONY: all clean test

all: $(ALL_BINS)

# Generic rule: compile each .c into an executable in the same directory
$(PART1_BINS): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

# $(TATAS_BINS): %: %.c
# 	$(CC) $(CFLAGS) -o $@ $<

# $(TAS_BINS): %: %.c
# 	$(CC) $(CFLAGS) -o $@ $<

# $(COMPARE_BIN): src/part2/compare_lock.c
# 	$(CC) $(CFLAGS) -o $@ $<

# --- Benchmarks ---
test: all
	./$(PART1_DIR)/benchmark_part1.sh
#	./$(TATAS_DIR)/benchmark_tatas.sh
#	./$(TAS_DIR)/benchmark_tas.sh
#	./src/part2/benchmark_compare_lock.sh

clean:
	rm -f $(ALL_BINS)