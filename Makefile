CC ?= gcc
CFLAGS_common ?= -Wall -std=gnu99 -DRUN_TEST
CFLAGS_orig = -O0
CFLAGS_opt  = -O0 $(CFLAGS_pool)
CFLAGS_opt_hash1  = $(CFLAGS_opt) -DHASH_1
CFLAGS_opt_hash2  = $(CFLAGS_opt) -DHASH_2
CFLAGS_opt_thread1  = $(CFLAGS_opt_hash1) -pthread -DTHREAD
CFLAGS_opt_thread2  = $(CFLAGS_opt_hash2) -pthread -DTHREAD -DTHD2
CFLAGS_pool = -DUSE_MEM_POOL

EXEC = phonebook_orig phonebook_opt \
		phonebook_opt_hash1 phonebook_opt_hash2 \
		phonebook_opt_thread1 phonebook_opt_thread2

all: $(EXEC)

SRCS_common = main.c

SRC_HASH = phonebook_opt_hash

phonebook_orig: $(SRCS_common) phonebook_orig.c phonebook_orig.h
	$(CC) $(CFLAGS_common) $(CFLAGS_orig) \
		-DIMPL="\"$@.h\"" -o $@ \
		$(SRCS_common) $@.c

phonebook_opt: $(SRCS_common) phonebook_opt.c phonebook_opt.h
	$(CC) $(CFLAGS_common) $(CFLAGS_opt) \
		-DIMPL="\"$@.h\"" -o $@ \
		$(SRCS_common) $@.c

phonebook_opt_hash1: $(SRCS_common) phonebook_opt_hash.c phonebook_opt_hash.h
	$(CC) $(CFLAGS_common) $(CFLAGS_opt_hash1) \
		-DIMPL="\"$(SRC_HASH).h\"" -o $@ \
		$(SRCS_common) $(SRC_HASH).c

phonebook_opt_hash2: $(SRCS_common) phonebook_opt_hash.c phonebook_opt_hash.h
	$(CC) $(CFLAGS_common) $(CFLAGS_opt_hash2) \
		-DIMPL="\"$(SRC_HASH).h\"" -o $@ \
		$(SRCS_common) $(SRC_HASH).c

phonebook_opt_thread1: $(SRCS_common) phonebook_opt_hash.c phonebook_opt_hash.h
	$(CC) $(CFLAGS_common) $(CFLAGS_opt_thread1) \
		-DIMPL="\"$(SRC_HASH).h\"" -o $@ \
		$(SRCS_common) $(SRC_HASH).c

phonebook_opt_thread2: $(SRCS_common) phonebook_opt_hash.c phonebook_opt_hash.h
	$(CC) $(CFLAGS_common) $(CFLAGS_opt_thread2) \
		-DIMPL="\"$(SRC_HASH).h\"" -o $@ \
		$(SRCS_common) $(SRC_HASH).c

run1: phonebook_orig
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_orig && echo 3 | sudo tee /proc/sys/vm/drop_caches"

run2: phonebook_opt
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_opt && echo 3 | sudo tee /proc/sys/vm/drop_caches"

run3: phonebook_opt_hash1
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_opt_hash1 && echo 3 | sudo tee /proc/sys/vm/drop_caches"

run4: phonebook_opt_hash2
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_opt_hash2 && echo 3 | sudo tee /proc/sys/vm/drop_caches"

run5: phonebook_opt_thread1
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_opt_thread1 && echo 3 | sudo tee /proc/sys/vm/drop_caches"

run6: phonebook_opt_thread2
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	watch -d -t "./phonebook_opt_thread2 && echo 3 | sudo tee /proc/sys/vm/drop_caches"

cache-test: $(EXEC)
	@rm -f *.txt
	sudo sh -c " echo 0 > /proc/sys/kernel/kptr_restrict"
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		./phonebook_orig 1>/dev/null
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		./phonebook_opt 1>/dev/null
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		./phonebook_opt_hash1 1>/dev/null
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		./phonebook_opt_hash2 1>/dev/null
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		./phonebook_opt_thread1 1>/dev/null
	echo 1 | sudo tee /proc/sys/vm/drop_caches
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		./phonebook_opt_thread2 1>/dev/null

cc:
	sudo sh -c " echo 0 > /proc/sys/kernel/kptr_restrict"
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	echo 1 | sudo tee /proc/sys/vm/drop_caches

rpt1: phonebook_orig cc
	perf record \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		-o perf.orig ./phonebook_orig
	perf report -i perf.orig

rpt2: phonebook_opt cc
	perf record \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		-o perf.opt ./phonebook_opt
	perf report -i perf.opt

rpt3: phonebook_opt_hash1 cc
	perf record \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		-o perf.opt_hash1 ./phonebook_opt_hash1
	perf report -i perf.opt_hash1

rpt4: phonebook_opt_hash2 cc
	perf record \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		-o perf.opt_hash2 ./phonebook_opt_hash2
	perf report -i perf.opt_hash2

rpt5: phonebook_opt_thread1 cc
	perf record \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		-o perf.opt_thread1 ./phonebook_opt_thread1
	perf report -i perf.opt_thread1

rpt6: phonebook_opt_thread2 cc
	perf record \
		-e cache-misses,cache-references,instructions,cycles,branches,branch-misses \
		-o perf.opt_thread2 ./phonebook_opt_thread2
	perf report -i perf.opt_thread2

output.txt: cache-test calculate
	./calculate

plot: output.txt
	gnuplot scripts/runtime.gp

calculate: calculate.c
	$(CC) $(CFLAGS_common) $^ -o $@

.PHONY: clean
clean:
	$(RM) $(EXEC) *.o perf.* \
		calculate *.txt *.png
