#!/bin/bash
for i in {1..15}
do
   make test TARGET=hw DEVICE=xilinx_samsung_u2x4_201920_3
done