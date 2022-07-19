#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"


#include <limits>

using namespace llvm;

namespace {
  struct DataDependenceGraph : public FunctionPass {
    static char ID;

    //These variables are provided for you. You should store your results in them.
    //You are free to add any more variables/helper unctions as you want

    int n; //variable to keep track of number of functions
    int totalbb;
    

    DataDependenceGraph() : FunctionPass(ID) {}

     void getAnalysisUsage(AnalysisUsage &AU) const {
       // TODO : Add whatever additional passes that you think is neccessary
       AU.setPreservesAll();
    }

    virtual bool doInitialization(Module &M) {
      errs () << "################# CS 6241: Compiler Optimizations Homework 1 - Parts 2.1 - 2.4 #################" << "\n";
      /* Initializing the values of helper variables */
      n = 0; 
      totalbb = 0;  
      
      return false;
    } 

    virtual bool runOnFunction(Function &F) {
  
      //################# TODO : Implement your solution here ###################
      return false;
    }

     //this function is for displaying the stats. You are not expected to modify anything here. 
    virtual bool doFinalization(Module &M) {
       errs() << "=============Stats for Entire Module=====================\n";
       errs() << "The Total basic blocks present in this module was " << totalbb << "\n";
       errs() << "The Total functions present in this module was " << n << "\n";
       errs() << "===========================================================\n";
       return false;       
    }
  };
}

char DataDependenceGraph::ID = 0;
//the pass has already been registered for you


static RegisterPass<DataDependenceGraph> X("DDG", "Registering DDG");