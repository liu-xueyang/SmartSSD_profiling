dir=$1
echo "**********COMPILING Test "$1" with Custom Pass**********:"
cd $dir
echo "CLANG"
rm *.bc *.s *.ll
clang -emit-llvm -fopenmp -c *.cpp -w -lm
echo "LLVM-LINK"
echo "$dir"_link.bc
llvm-link *.bc -o "$dir"_link.bc
llvm-dis "$dir"_link.bc
echo "OPT"
opt -load ../llvm/build/libddg.so "$dir"_link.bc > "$dir"_pass.bc

# opt -load ../../../../build/HW1p2/libHW1p2Pass.so --hw1p2 "$dir"_link.bc >> "$dir"_pass.bc 
llvm-dis "$dir"_pass.bc
echo "LLC"
llc -o "$dir"_pass.s "$dir"_pass.bc
echo "Clang"
clang++ -O3 "$dir"_pass.s -fopenmp -o "$dir"_exe
cd ../
echo "***********************************************************"