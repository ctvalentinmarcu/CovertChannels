

all:    recv

recv: 	receiver_cpu.cpp
	g++ receiver_cpu.cpp -o receiver_cpu

test:   cpu_test.cpp
	g++ cpu_test.cpp -o cpu_test
	./cpu_test > input_cpu.txt

clean:
	rm receiver_cpu cpu_test



