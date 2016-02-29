CC ?= gcc
CFLAGS ?= -Wall -std=gnu99 -O0 -DOPT_ALIGN=5

EXEC = phonebook
all: $(EXEC)

SRCS = main.c

phonebook: $(SRCS) phonebook.c phonebook.h
	$(CC) $(CFLAGS) \
		-o $@ \
		$(SRCS) $@.c

run: $(EXEC)
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook linkedlist orig.txt && echo 3 | sudo tee /proc/sys/vm/drop_caches"

cache-test: $(EXEC)
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles \
		./phonebook linkedlist orig.txt
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles \
		./phonebook avltree opt.txt

output.txt: cache-test calculate
	./calculate

plot: output.txt
	gnuplot scripts/runtime.gp

calculate: calculate.c
	$(CC) $(CFLAGS_common) $^ -o $@

astyle:
	astyle --style=kr --indent=spaces=4 --indent-switches --suffix=none *.[ch]

.PHONY: clean
clean:
	$(RM) $(EXEC) *.o perf.* \
	      	calculate orig.txt opt.txt output.txt runtime.png
