CFLAGS=-march=native -O3 -masm=intel -flto -fopenmp -std=gnu2x -ISIMDxorshift/include

manganese: manganese.o tests-512.o tests-256.o tests.o hardware.o SIMDxorshift/simdxorshift128plus.o
	cc $(CFLAGS) -o manganese manganese.o tests-512.o tests-256.o tests.o hardware.o SIMDxorshift/simdxorshift128plus.o

manganese.o: manganese.c
	cc $(CFLAGS) -c manganese.c

tests-512.o: tests-512.c tests-512.h
	cc $(CFLAGS) -mrdrnd -mavx512bw -c tests-512.c

tests-256.o: tests-256.c tests-256.h
	cc $(CFLAGS) -mrdrnd -mavx2 -c tests-256.c

tests.o: tests.c tests.h
	cc $(CFLAGS) -c tests.c

hardware.o: hardware.c hardware.h
	cc $(CFLAGS) -c hardware.c

SIMDxorshift/simdxorshift128plus.o:
	 $(MAKE) CFLAGS=-mavx512f -C SIMDxorshift simdxorshift128plus.o

.PHONY: clean
clean:
	rm -f *.o manganese
	$(MAKE) -C SIMDxorshift clean
