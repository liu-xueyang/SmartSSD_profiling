#include "DataDependenceGraph.h"


using namespace llvm;
using namespace ddg;

void DataDependenceGraph::getAnalysisUsage(AnalysisUsage &AU) const {
  // TODO : Add whatever additional passes that you think is neccessary
  AU.addRequired<MemoryDependenceWrapperPass>();
  AU.setPreservesAll();
}

bool DataDependenceGraph::doInitialization(Module &M) {
  errs () << "################# Generating Data Dependence Graph #################" << "\n";
  /* Initializing the values of helper variables */
  n = 0; 
  totalbb = 0;  
  
  return false;
} 

bool DataDependenceGraph::runOnModule(Module &M) {
  //################# TODO : Implement your solution here ###################
  return false;
}

//this function is for displaying the stats. You are not expected to modify anything here. 
bool DataDependenceGraph::doFinalization(Module &M) {
  errs() << "=============Stats for Entire Module=====================\n";
  errs() << "The Total basic blocks present in this module was " << totalbb << "\n";
  errs() << "The Total functions present in this module was " << n << "\n";
  errs() << "===========================================================\n";
  return false;   
}


char DataDependenceGraph::ID = 0;

static RegisterPass<DataDependenceGraph> X("DDG", "Registering DDG");