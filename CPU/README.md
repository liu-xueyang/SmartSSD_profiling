# CPU equivalent examples

This repository contain CPU multithreaded implementations of the equivalent SmartSSD HLS implementations.

It's assumed that the data to process is originally stored on SSD and moved to the CPU for computation. CPU use fread to retrieve file from the SSD (note that the read should be from a file location where the SSD is mounted), process the data and store it back to the SSD location.