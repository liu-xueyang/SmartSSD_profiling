/**********
Copyright (c) 2019, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/
#include "xcl2.hpp"
#include <vector>
#include "../timer.hpp"

#define DATA_SIZE 1024*1024*64
#define INCR_VALUE 1.125
#define NUM_INSTANCE 4
// define internal buffer max size
#define BURSTBUFFERSIZE 256
#define OUT_FILENAME "profile_multiple.csv"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
    return EXIT_FAILURE;
  }

  std::string binaryFile = argv[1];

  size_t size = DATA_SIZE;
  int num_instance = NUM_INSTANCE;
  float inc_value = INCR_VALUE;
  cl_int err;
  std::vector<cl::Kernel> krnl_add(num_instance);
  cl::CommandQueue q;
  cl::Context context;
  // Allocate Memory in Host Memory
  std::vector<float, aligned_allocator<float>> source_inout(size);
  std::vector<float, aligned_allocator<float>> source_sw_results(size);

  // Create the test data and Software Result
  for (size_t i = 0; i < size; i++) {
    source_inout[i] = rand() / (float)RAND_MAX;
    source_sw_results[i] = source_inout[i] + inc_value;
  }

  // OPENCL HOST CODE AREA START
  auto devices = xcl::get_xil_devices();
  // read_binary_file() is a utility API which will load the binaryFile
  // and will return the pointer to file buffer.
  auto fileBuf = xcl::read_binary_file(binaryFile);
  cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
  int valid_device = 0;
  for (unsigned int i = 0; i < devices.size(); i++) {
    auto device = devices[i];
    // Creating Context and Command Queue for selected Device
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, q = cl::CommandQueue(context, device,
                                        CL_QUEUE_PROFILING_ENABLE |
                                       CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err));

    std::cout << "Trying to program device[" << i
              << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    cl::Program program(context, {device}, bins, NULL, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
    } else {
      std::cout << "Device[" << i << "]: program successful!\n";
      for (int i = 0; i < num_instance; i++) {
        OCL_CHECK(err, krnl_add[i] = cl::Kernel(program, "vadd", &err));
      }
      valid_device++;
      break; // we break because we found a valid device
    }
  }
  if (valid_device == 0) {
    std::cout << "Failed to program any device found, exit!\n";
    exit(EXIT_FAILURE);
  }

  int t_CPU2Device, t_Device, t_Device2CPU;
  auto chunk_size = size / num_instance;
  size_t vector_size_bytes = sizeof(float) * chunk_size;
  std::vector<cl::Buffer> buffer_rw(num_instance);
  // Allocate Buffer in Global Memory
  for (int i = 0; i < num_instance; i++) {
    OCL_CHECK(err, buffer_rw[i] = cl::Buffer(
                      context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                      vector_size_bytes, source_inout.data()+i*chunk_size, &err));
  }

  for (int i = 0; i < num_instance; i++) {
    OCL_CHECK(err, err = krnl_add[i].setArg(0, buffer_rw[i]));
    OCL_CHECK(err, err = krnl_add[i].setArg(1, chunk_size));
    OCL_CHECK(err, err = krnl_add[i].setArg(2, inc_value));
  }

  printf("Finished config, start job\n");
  Timer timer;
  // Copy input data to device global memory
  for (int i = 0; i < num_instance; i++) {
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_rw[i]},
                                                    0 /* 0 means from host*/));
    // printf("execution done for %d, copy output\n", i);
  }
  OCL_CHECK(err, err = q.finish());
  t_CPU2Device = timer.elapsed();
  std::cout << "data copy elapsed time = " << t_CPU2Device << std::endl;

  timer.reset();
  // Launch the Kernel
  for (int i = 0; i < num_instance; i++) {
    OCL_CHECK(err, err = q.enqueueTask(krnl_add[i]));
  }
  OCL_CHECK(err, err = q.finish()); // synchronization
  t_Device = timer.elapsed();
  std::cout << "kernel run elapsed time = " << t_Device << std::endl;

  timer.reset();
  // Copy Result from Device Global Memory to Host Local Memory
  for (int i = 0; i < num_instance; i++) {
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_rw[i]},
                                                    CL_MIGRATE_MEM_OBJECT_HOST));
  }
  OCL_CHECK(err, err = q.finish());
  t_Device2CPU = timer.elapsed();
  std::cout << "data copy back elapsed time = " << t_Device2CPU << std::endl;
  // OPENCL HOST CODE AREA END

  // Compare the results of the Device to the simulation
  int match = 0;
  for (size_t i = 0; i < size; i++) {
    if (source_inout[i] != source_sw_results[i]) {
      std::cout << "Error: Result mismatch" << std::endl;
      std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
                << " Device result = " << source_inout[i] << std::endl;
      match = 1;
      break;
    } else {
      // std::cout << source_inout[i] << " ";
      // if (((i + 1) % 16) == 0)
      //   std::cout << std::endl;
    }
  }

  // std::ofstream myfile;
  // myfile.open (OUT_FILENAME);
  std::fstream myfile(OUT_FILENAME, std::fstream::out | std::fstream::app);
  myfile << NUM_INSTANCE << "," << DATA_SIZE/1024/1024 << "," <<  t_CPU2Device << "," << t_Device << "," << t_Device2CPU << "\n";
  myfile.close();

  std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
  return (match ? EXIT_FAILURE : EXIT_SUCCESS);
}
