all:
	pass
serial:
	g++ serial_matrix.cpp -o serial_m
	./serial_m
thread:
	g++ thread_matrix.cpp -o thread_m
	./thread_m
cuda:
	g++ cuda_matrix.cu -o cuda_m
	./cuda_m
