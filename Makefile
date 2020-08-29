# compile Q2.cpp
threaded_prime: Q2.cpp
	g++ -lpthread Q2.cpp -o threaded_prime

# run threaded_prime with 100 as highest num and 2 threads
run_prime2: threaded_prime
	./threaded_prime 100 2

# clean the directory
clean:
	rm -f threaded_prime
