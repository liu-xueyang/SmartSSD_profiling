#ifndef DATADEPENDENCEGRAPH_H_
#define DATADEPENDENCEGRAPH_H_
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/Analysis/MemoryLocation.h"

namespace ddg
{
  class DataDependenceGraph : public llvm::ModulePass
  {
  public:
    static char ID;
    DataDependenceGraph() : llvm::ModulePass(ID) {};
    void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
    virtual bool doInitialization(llvm::Module &M);
    llvm::StringRef getPassName() const override { return "Data Dependence Graph"; }
    virtual bool runOnModule(llvm::Module &M) override;
    virtual bool doFinalization(llvm::Module &M);
    // void addDefUseEdges(llvm::Instruction &inst);
    // void addRAWEdges(llvm::Instruction &inst);
    // void addAliasEdges(llvm::Instruction &inst);
    // llvm::AliasResult queryAliasUnderApproximate(llvm::Value &v1, llvm::Value &v2);

  private:
    llvm::MemoryDependenceResults *_mem_dep_res;
    int n, totalbb;
  };
} // namespace ddg
#endif