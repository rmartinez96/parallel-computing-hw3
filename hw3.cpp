// Raul Martinez
// 20317695

// NOTES:
// Everything works!

#include <iostream>     // std::cout, std::endl
#include <thread>       // std::this_thread::sleep_for
#include <chrono>       // std::chrono::seconds
#include "mpi.h"        // MPI_Init, MPI_Comm_size, MPI_Comm_rank, MPI_Finalize
#include <utility>
#include <vector>

using namespace std;

template <class T1,class T2>
  pair<T1,T2> make_pair (T1 x, T2 y)
  {
    return ( pair<T1,T2>(x,y) );
  }

int check_prime(int n);
int prime(int n);
pair<int, int> twin(int n);
int countTwin(int n);

int main(int argc, char *argv[]){
    int myrank, world_size;

    int input_p4 = -1;

    // Initialize the MPI context.
    MPI_Init(&argc, &argv);
    // Get the size of the world.
    MPI_Comm_size( MPI_COMM_WORLD, &world_size);
    // Get the rank of the MPI process.
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    int prim = 0;
    int twin_prime1 = 0;
    int twin_prime2 = 0;
    int prime_found = 0;
    int twin_prime1_found = 0;
    int twin_prime2_found = 0;
    int count_prime = 0;
    int count_prime_found = 0;

    if (myrank == 4) {
        cin >> input_p4;
        for (int i = 0; i < 4; i++)
            MPI_Send(&input_p4, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        prim = twin_prime1 = twin_prime2 = 999999;
    }
    else {
        MPI_Recv(&input_p4, 1, MPI_INT, 4, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        pair <int, int> prime_pair = twin(input_p4);
        prim = prime(input_p4);
        twin_prime1 = prime_pair.first;
        twin_prime2 = prime_pair.second;
        count_prime = countTwin(input_p4);
    }

    MPI_Reduce(&prim, &prime_found, 1, MPI_INT, MPI_MIN, 4, MPI_COMM_WORLD);
    MPI_Reduce(&twin_prime1, &twin_prime1_found, 1, MPI_INT, MPI_MIN, 4, MPI_COMM_WORLD);
    MPI_Reduce(&twin_prime2, &twin_prime2_found, 1, MPI_INT, MPI_MIN, 4, MPI_COMM_WORLD);
    MPI_Reduce(&count_prime, &count_prime_found, 1, MPI_INT, MPI_SUM, 4, MPI_COMM_WORLD);

    if (myrank == 4) {
        cout << "prime: " << prime_found << " | " << "twin: (" << twin_prime1_found << ", " << twin_prime2_found << ") | countTwin: " << count_prime_found << endl;
    }

    // Clean up ne;
    MPI_Finalize();

    return 0;
}

int check_prime(int n)
{
    // Corner case 
    if (n <= 1) 
        return 0; 
    // Check from 2 to n-1 
    for (int i = 2; i < n; i++) 
        if (n % i == 0) 
            return 0; 
    return 1; 
}

int prime(int n)
{
    int i;
    MPI_Comm_rank(MPI_COMM_WORLD, &i);

    int m = 0;
    while (1) {
        int q = 8*m + (2*i + 1);
        if (q > n) {
            if (check_prime(q)) {
                return q;
            }
        }
        m++;
    }
    return -1;
}

pair<int, int> twin(int n) {
    int i;
    MPI_Comm_rank(MPI_COMM_WORLD, &i);

    int m = 0;
    while (1) {
        int q = 8*m + (2*i + 1);
        int qPlusTwo = -1;
        if (q > n) {
            if (check_prime(q)) {
                qPlusTwo = q + 2;
                if (check_prime(qPlusTwo)) {
                    pair <int, int> pq = std::make_pair(q, qPlusTwo);
                    return pq;
                }
            }
        }
        m++;
    }
    pair <int, int> error = std::make_pair(-1, -1);
    return error;
}

int countTwin(int n) {
    int i;
    MPI_Comm_rank(MPI_COMM_WORLD, &i);

    int m = 0, a = 0;
    while (1) {
        int q = 8*m + (2*i + 1);
        int qPlusTwo = -1;
        if (q < n) {
            if (check_prime(q)) {
                qPlusTwo = q + 2;
                if (check_prime(qPlusTwo)) {
                    a++;
                }
            }
        }
        else {
            return a;
        }
        m++;
    }
    return 0;
}