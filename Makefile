CFLAGS=-march=native -O3 -masm=intel -flto -fopenmp -std=gnu2x -ISIMDxorshift/include

manganese: manganese.o tests-512.o tests-256.o tests.o hardware.o SIMDxorshift/simdxorshift128plus.o OpenBLAS/libopenblas.a
	cc $(CFLAGS) -o manganese manganese.o tests-512.o tests-256.o tests.o hardware.o \
	SIMDxorshift/simdxorshift128plus.o OpenBLAS/libopenblas.a \

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

OpenBLAS/libopenblas.a:
	 $(MAKE) USE_OPENMP=1 USE_THREAD=1 NO_LAPACK=0 DYNAMIC_ARCH=1 DYNAMIC_LIST="HASWELL SKYLAKEX ATOM COOPERLAKE SAPPHIRERAPIDS ZEN" -C OpenBLAS

.PHONY: clean
clean:
	rm -f manganese.iso *.o manganese
	rm -rf /tmp/manganese-fs /tmp/manganese-iso

.PHONY: clean-all
clean-all: clean
	rm -f *.tcz* tinycore.iso
	$(MAKE) -C SIMDxorshift clean
	$(MAKE) -C OpenBLAS clean

### Standalone ISO

manganese.iso: tinycore.iso manganese
	sudo bash build-iso.sh

tinycore.iso:
	curl -o tinycore.iso "http://tinycorelinux.net/13.x/x86_64/release/CorePure64-13.0.iso"
