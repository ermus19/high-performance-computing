all: run

jacobi: jacobi.cu nvtx_macros.h Makefile
	nvcc -DNO_SYNC -DUSE_NVTX -arch=sm_37 jacobi.cu -lnvToolsExt -o jacobi

run: jacobi
	./jacobi

profile: jacobi
	nvprof --unified-memory-profiling per-process-device ./jacobi

memcheck: jacobi
	cuda-memcheck ./jacobi
	
clean:
	rm -f jacobi

