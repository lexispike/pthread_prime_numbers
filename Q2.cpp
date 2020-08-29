/*
 * Threaded Prime Program
 * Alyxandra Spikerman
 * High Performace Computing
 * Jan 2019
 * Question 2 Homework 2
 */
#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <vector>
#include <ctime>

using namespace std;

int HIGHEST_NUM;
int NUM_THREADS;
int index;

// int vector for prime numbers where i is the number and primes[i] is 1 if yes prime and 0 if not prime
vector<int> primes;

pthread_mutex_t lock;

/*
 * is_prime
 *
 * @prime : the number we are trying to determine if prime
 */
int is_prime(int prime) {
    int val = 1;
    if (prime <= 3) { // 1-3 are all primes, so we don't have to check in the for loop
        val = 1;
    } else if (prime % 2 == 0) { // all even numbers are not primes
        val = 0;
    } else {
        val = 1;
        // check all odd numbers up to sqrt prime if they're prime by dividing against all odd numbers
        for (int j = 3; j <= sqrt(prime); j+=2) {
            if (prime % j == 0) {
                val = 0;
                break;
            }
        }
    }
    return val;
}

/*
 * all_primes
 *
 * This function will find all the prime numbers between 1 and the HIGHEST_NUM
 */
void* all_primes(void* arg) {
    int my_index;
    while (true) {
        // protect the retrieval and setting of index by locking
        pthread_mutex_lock(&lock);
        my_index = index;

        // we need to break the while loop once the index is higher than the highest number
        if (my_index == HIGHEST_NUM + 1) {
            pthread_mutex_unlock(&lock);
            break;
        }

        index++;
        pthread_mutex_unlock(&lock);

        primes[my_index] = is_prime(my_index);
    }

    return NULL;
}

/*
 * Inputs: #threads largest#
 */
int main(int argc, char* argv[]) {
    // we'll be using time_t variables to get the total serial vs parallel times
    time_t start_serial1 = time(NULL);

    // get the command line arguments and error out if there aren't enough
    if (argc == 3) {
        HIGHEST_NUM = atoi(argv[1]);
        NUM_THREADS = atoi(argv[2]);
        primes.resize(HIGHEST_NUM + 1, 1);
    } else {
        cout << "Error: must input 2 arguments, <highest number> <number of threads>" << endl;
        return 1;
    }
    index = 1;

    // initialize the lock for updating index
    if (pthread_mutex_init(&lock, NULL) != 0) {
        cout << "Error: failed to initialize mutex" << endl;
        return 1;
    }

    // declare pthread variables
    pthread_t threads[NUM_THREADS];
    int pthread_return;

    for (int i = 0; i < NUM_THREADS; i++) {
        // create the thread and call all_primes
        pthread_return = pthread_create(&threads[i], NULL, &all_primes, NULL);
        if (pthread_return) {
            cout << "Error: cannot create thread, " << pthread_return << endl;
            return 1;
        }
    }
    time_t end_serial1 = time(NULL);
    time_t start_parallel = time(NULL);
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_return = pthread_join(threads[i], NULL); // join the threads
        if (pthread_return) {
            cout << "Error: cannot join thread, " << pthread_return << endl;
            return 1;
        }
    }
    time_t end_parallel = time(NULL);
    time_t start_serial2 = time(NULL);
    // print out all prime numbers between 1 and the highest number inputted
    for (int i = 1; i <= HIGHEST_NUM; i++) {
        if (primes[i]) {
            cout << i << endl;
        }
    }
    pthread_mutex_destroy(&lock);

    time_t end_serial2 = time(NULL);

    // get the parallel time and the serial time and print it out
    double parallel_time = difftime(end_parallel, start_parallel);
    double serial_time = difftime(end_serial1, start_serial1) + difftime(end_serial2, start_serial2);
    cout << "Parallel time: " << parallel_time << "s, Serial time: " << serial_time << "s\n";

    return 0;
}
