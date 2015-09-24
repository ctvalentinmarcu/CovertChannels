

all:    sender

sender: sender_cpu.cpp
	g++ sender_cpu.cpp -o sender_cpu

test:   cpu_test.cpp
	g++ cpu_test.cpp -o cpu_test
	./cpu_test > input_cpu.txt

clean:
	rm sender_cpu cpu_test



