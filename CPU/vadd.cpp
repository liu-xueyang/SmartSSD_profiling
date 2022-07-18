// OpenMP program to run vector add
// using C language
 
// OpenMP header
#include <omp.h>
 
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <string>

#include "../timer.hpp"

#define FILENAME "/home/xueyang.liu/vadd.txt"
 
int main(int argc, char* argv[])
{
    int numElements = 1024 * 1024 *512;
    size_t size = numElements * sizeof(int);
    int numThreads = 8; 
    int ops_per_thread = numElements / numThreads + 1;
    Timer timer;
    int t_SSD2CPU, t_CPU, t_CPU2SSD;
    printf("[Vector addition of %d elements with %d threads]\n", numElements, numThreads);

    // Allocate the input vector A
    
    int *h_input = (int *)malloc(size);
    int *h_A = (int *)malloc(size);

    // Allocate the input vector B
    // int *h_B = (int *)malloc(size);

    // Allocate the output vector C
    int *h_C = (int *)malloc(size);

    // Initialize the host input vectors
    std::ofstream oFile (FILENAME, std::ios::binary);
    std::cout << "Trying to open file " << FILENAME << std::endl;
    if (oFile.is_open()) {
        for (int i = 0; i < numElements; ++i) {
            int tmp = rand();
            h_input[i] = tmp;
        }
        timer.reset();
        oFile.write(reinterpret_cast<char *>(&h_input[0]), size);
        oFile.close();
    }
    else {
        std::cout << "Unable to open file, please check your sudo permission since the file is SSD" << std::endl;
        return 0;
    }
    

    std::ifstream iFile (FILENAME, std::ios::binary);
    int i = 0;
    timer.reset();
    iFile.read(reinterpret_cast<char *>(&h_A[0]), size);
    // iFile.
    iFile.close();
    t_SSD2CPU = timer.elapsed();
    std::cout << "data copy elapsed time = " << t_SSD2CPU << std::endl;

    // Execute vector add in parallel
    timer.reset();
    #pragma omp parallel num_threads(numThreads)
    {
 
        // printf("Hello World... from thread = %d\n",
        //        omp_get_thread_num());
        int threadId = omp_get_thread_num();
        for (i = 0; i < ops_per_thread; i++) {
            int arr_i = threadId * ops_per_thread + i;

            if (arr_i < numElements) {
                h_C[arr_i] = h_A[arr_i] + 10;
            }
        }
        #pragma omp barrier
        
    }
    // Ending of parallel region
    t_CPU = timer.elapsed();
    std::cout << "CPU execution elapsed time = " << t_CPU << std::endl;

    // write output results
    oFile.open(FILENAME, std::ios::trunc | std::ios::binary);
    timer.reset();
    oFile.write(reinterpret_cast<char *>(&h_C[0]), size); // do I need to flush this to SSD
    t_CPU2SSD = timer.elapsed();
    std::cout << "CPU to SSD data transfer elapsed time = " << t_CPU2SSD << std::endl;

}