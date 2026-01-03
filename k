CMake Warning (dev) at /Users/agrossman27/homebrew/share/cmake/Modules/CheckIncludeFile.cmake:154 (message):
  Policy CMP0075 is not set: Include file check macros honor
  CMAKE_REQUIRED_LIBRARIES.  Run "cmake --help-policy CMP0075" for policy
  details.  Use the cmake_policy command to set the policy and suppress this
  warning.

  CMAKE_REQUIRED_LIBRARIES is set to:

    /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib/libedit.tbd

  For compatibility with CMake 3.11 and below this check is ignoring it.
Call Stack (most recent call first):
  /Users/agrossman27/homebrew/opt/llvm/lib/cmake/llvm/FindLibEdit.cmake:28 (check_include_file)
  /Users/agrossman27/homebrew/opt/llvm/lib/cmake/llvm/LLVMConfig.cmake:276 (find_package)
  CMakeLists.txt:31 (find_package)
This warning is for project developers.  Use -Wno-dev to suppress it.

In file included from /Users/agrossman27/coding/c/tent/src/ast.cpp:4:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/IRBuilder.h:22:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/BasicBlock.h:19:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/ADT/ilist.h:58:34: warning: unused parameter 'V' [-Wunused-parameter]
   58 |   static void deleteNode(NodeTy *V) {}
      |                                  ^
In file included from /Users/agrossman27/coding/c/tent/src/ast.cpp:4:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/IRBuilder.h:22:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/BasicBlock.h:23:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/DebugProgramInstruction.h:54:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/DebugLoc.h:18:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/TrackingMDRef.h:16:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Metadata.h:21:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/ADT/PointerUnion.h:21:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Support/Casting.h:279:45: warning: unused parameter 'f' [-Wunused-parameter]
  279 |   static inline bool isPossible(const From &f) { return true; }
      |                                             ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Support/Casting.h:604:41: warning: unused parameter 't' [-Wunused-parameter]
  604 |   static inline bool isPresent(const T &t) { return true; }
      |                                         ^
In file included from /Users/agrossman27/coding/c/tent/src/ast.cpp:4:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/IRBuilder.h:24:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/ConstantFolder.h:22:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Constants.h:61:41: warning: unused parameter 'From' [-Wunused-parameter]
   61 |   Value *handleOperandChangeImpl(Value *From, Value *To) {
      |                                         ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Constants.h:61:54: warning: unused parameter 'To' [-Wunused-parameter]
   61 |   Value *handleOperandChangeImpl(Value *From, Value *To) {
      |                                                      ^
In file included from /Users/agrossman27/coding/c/tent/src/ast.cpp:4:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/IRBuilder.h:24:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/ConstantFolder.h:88:37: warning: unused parameter 'FMF' [-Wunused-parameter]
   88 |                       FastMathFlags FMF) const override {
      |                                     ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/ConstantFolder.h:93:36: warning: unused parameter 'FMF' [-Wunused-parameter]
   93 |                      FastMathFlags FMF) const override {
      |               [ 17%] Building CXX object CMakeFiles/tent.dir/src/args.cpp.o
/usr/bin/c++  -I/Users/agrossman27/coding/c/tent/include -I/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include -std=gnu++17 -arch arm64 -O3 -Wall -Wextra -Wpedantic -MD -MT CMakeFiles/tent.dir/src/args.cpp.o -MF CMakeFiles/tent.dir/src/args.cpp.o.d -o CMakeFiles/tent.dir/src/args.cpp.o -c /Users/agrossman27/coding/c/tent/src/args.cpp
[ 20%] Building CXX object CMakeFiles/tent.dir/src/lexer.cpp.o
/usr/bin/c++  -I/Users/agrossman27/coding/c/tent/include -I/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include -std=gnu++17 -arch arm64 -O3 -Wall -Wextra -Wpedantic -MD -MT CMakeFiles/tent.dir/src/lexer.cpp.o -MF CMakeFiles/tent.dir/src/lexer.cpp.o.d -o CMakeFiles/tent.dir/src/lexer.cpp.o -c /Users/agrossman27/coding/c/tent/src/lexer.cpp
[ 24%] Building CXX object CMakeFiles/tent.dir/src/evaluator.cpp.o
/usr/bin/c++  -I/Users/agrossman27/coding/c/tent/include -I/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include -std=gnu++17 -arch arm64 -O3 -Wall -Wextra -Wpedantic -MD -MT CMakeFiles/tent.dir/src/evaluator.cpp.o -MF CMakeFiles/tent.dir/src/evaluator.cpp.o.d -o CMakeFiles/tent.dir/src/evaluator.cpp.o -c /Users/agrossman27/coding/c/tent/src/evaluator.cpp
[ 27%] Building CXX object CMakeFiles/tent.dir/src/compiler.cpp.o
/usr/bin/c++  -I/Users/agrossman27/coding/c/tent/include -I/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include -std=gnu++17 -arch arm64 -O3 -Wall -Wextra -Wpedantic -MD -MT CMakeFiles/tent.dir/src/compiler.cpp.o -MF CMakeFiles/tent.dir/src/compiler.cpp.o.d -o CMakeFiles/tent.dir/src/compiler.cpp.o -c /Users/agrossman27/coding/c/tent/src/compiler.cpp
[ 31%] Building CXX object CMakeFiles/tent.dir/src/parser.cpp.o
/usr/bin/c++  -I/Users/agrossman27/coding/c/tent/include -I/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include -std=gnu++17 -arch arm64 -O3 -Wall -Wextra -Wpedantic -MD -MT CMakeFiles/tent.dir/src/parser.cpp.o -MF CMakeFiles/tent.dir/src/parser.cpp.o.d -o CMakeFiles/tent.dir/src/parser.cpp.o -c /Users/agrossman27/coding/c/tent/src/parser.cpp
[ 34%] Building CXX object CMakeFiles/tent.dir/src/diagnostics.cpp.o
/usr/bin/c++  -I/Users/agrossman27/coding/c/tent/include -I/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include -std=gnu++17 -arch arm64 -O3 -Wall -Wextra -Wpedantic -MD -MT CMakeFiles/tent.dir/src/diagnostics.cpp.o -MF CMakeFiles/tent.dir/src/diagnostics.cpp.o.d -o CMakeFiles/tent.dir/src/diagnostics.cpp.o -c /Users/agrossman27/coding/c/tent/src/diagnostics.cpp
[ 37%] Building CXX object CMakeFiles/tent.dir/src/errors.cpp.o
/usr/bin/c++  -I/Users/agrossman27/coding/c/tent/include -I/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include -std=gnu++17 -arch arm64 -O3 -Wall -Wextra -Wpedantic -MD -MT CMakeFiles/tent.dir/src/errors.cpp.o -MF CMakeFiles/tent.dir/src/errors.cpp.o.d -o CMakeFiles/tent.dir/src/errors.cpp.o -c /Users/agrossman27/coding/c/tent/src/errors.cpp
[ 41%] Building CXX object CMakeFiles/tent.dir/src/token.cpp.o
/usr/bin/c++  -I/Users/agrossman27/coding/c/tent/include -I/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include -std=gnu++17 -arch arm64 -O3 -Wall -Wextra -Wpedantic -MD -MT CMakeFiles/tent.dir/src/token.cpp.o -MF CMakeFiles/tent.dir/src/token.cpp.o.d -o CMakeFiles/tent.dir/src/token.cpp.o -c /Users/agrossman27/coding/c/tent/src/token.cpp
[ 44%] Building CXX object CMakeFiles/tent.dir/src/esc_codes.cpp.o
/usr/bin/c++  -I/Users/agrossman27/coding/c/tent/include -I/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include -std=gnu++17 -arch arm64 -O3 -Wall -Wextra -Wpedantic -MD -MT CMakeFiles/tent.dir/src/esc_codes.cpp.o -MF CMakeFiles/tent.dir/src/esc_codes.cpp.o.d -o CMakeFiles/tent.dir/src/esc_codes.cpp.o -c /Users/agrossman27/coding/c/tent/src/esc_codes.cpp
[ 48%] Linking CXX executable tent
/Users/agrossman27/homebrew/bin/cmake -E cmake_link_script CMakeFiles/tent.dir/link.txt --verbose=1
/usr/bin/c++  -arch arm64 -Wl,-search_paths_first -Wl,-headerpad_max_install_names -L/Users/agrossman27/homebrew/opt/llvm/lib CMakeFiles/tent.dir/src/main.cpp.o CMakeFiles/tent.dir/src/types.cpp.o CMakeFiles/tent.dir/src/native.cpp.o CMakeFiles/tent.dir/src/ast.cpp.o CMakeFiles/tent.dir/src/args.cpp.o CMakeFiles/tent.dir/src/lexer.cpp.o CMakeFiles/tent.dir/src/evaluator.cpp.o CMakeFiles/tent.dir/src/compiler.cpp.o CMakeFiles/tent.dir/src/parser.cpp.o CMakeFiles/tent.dir/src/diagnostics.cpp.o CMakeFiles/tent.dir/src/errors.cpp.o CMakeFiles/tent.dir/src/token.cpp.o CMakeFiles/tent.dir/src/esc_codes.cpp.o -o tent  -L/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/lib -Wl,-search_paths_first -Wl,-headerpad_max_install_names
sed-parameter]
 1307 |                            ArrayRef<OperandBundleDef> OpBundles,
      |                                                       ^
In file included from /Users/agrossman27/coding/c/tent/src/ast.cpp:7:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Verifier.h:24:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/PassManager.h:630:23: warning: unused parameter 'IR' [-Wunused-parameter]
  630 |   Result run(IRUnitT &IR, AnalysisManager<IRUnitT, ExtraArgTs...> &AM,
      |                       ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/PassManager.h:630:68: warning: unused parameter 'AM' [-Wunused-parameter]
  630 |   Result run(IRUnitT &IR, AnalysisManager<IRUnitT, ExtraArgTs...> &AM,
      |                                                                    ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/PassManager.h:916:34: warning: unused parameter 'Arg' [-Wunused-parameter]
  916 |   PreservedAnalyses run(IRUnitT &Arg, AnalysisManagerT &AM, ExtraArgTs &&...) {
      |                                  ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/PassManager.h:916:57: warning: unused parameter 'AM' [-Wunused-parameter]
  916 |   PreservedAnalyses run(IRUnitT &Arg, AnalysisManagerT &AM, ExtraArgTs &&...) {
      |                                                         ^
/Users/agrossman27/coding/c/tent/src/ast.cpp:64:34: warning: 'getPointerTo' is deprecated: Use PointerType::get instead [-Wdeprecated-declarations]
   64 |                                 {llvm::Type::getInt8Ty(ctx)->getPointerTo()},
      |                                                              ^~~~~~~~~~~~
      |                                                              PointerType::get
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Type.h:484:12: note: 'getPointerTo' has been explicitly marked deprecated here
  484 |   LLVM_ABI LLVM_DEPRECATED("Use PointerType::get instead", "PointerType::get")
      |            ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Support/Compiler.h:250:50: note: expanded from macro 'LLVM_DEPRECATED'
  250 | #define LLVM_DEPRECATED(MSG, FIX) __attribute__((deprecated(MSG, FIX)))
      |                                                  ^
/Users/agrossman27/coding/c/tent/src/ast.cpp:96:79: warning: unused parameter 'builderBase' [-Wunused-parameter]
   96 | llvm::Value* IntLiteral::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase& builderBase, llvm::Module&) {
      |                                                                               ^
/Users/agrossman27/coding/c/tent/src/ast.cpp:120:53: warning: unused parameter 'ctx' [-Wunused-parameter]
  120 | llvm::Value* StrLiteral::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase& builderBase, llvm::Module& module) {
      |                                                     ^
/Users/agrossman27/coding/c/tent/src/ast.cpp:120:106: warning: unused parameter 'module' [-Wunused-parameter]
  120 | llvm::Value* StrLiteral::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase& builderBase, llvm::Module& module) {
      |                                                                                                          ^
/Users/agrossman27/coding/c/tent/src/ast.cpp:211:104: warning: unused parameter 'module' [-Wunused-parameter]
  211 | llvm::Value* Variable::codegen(llvm::LLVMContext& ctx, llvm::IRBuilderBase& builderBase, llvm::Module& module) {
      |                                                                                                        ^
/Users/agrossman27/coding/c/tent/src/ast.cpp:585:25: warning: 'get' is deprecated: PointerType::get with pointee type is pending removal. Use Context overload. [-Wdeprecated-declarations]
  585 |                                         llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0),
      |                                                            ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/DerivedTypes.h:709:5: note: 'get' has been explicitly marked deprecated here
  709 |   [[deprecated("PointerType::get with pointee type is pending removal. Use "
      |     ^
33 warnings generated.
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:4:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/LLVMContext.h:19:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Support/CBindingWrapping.h:17:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Support/Casting.h:279:45: warning: unused parameter 'f' [-Wunused-parameter]
  279 |   static inline bool isPossible(const From &f) { return true; }
      |                                             ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Support/Casting.h:604:41: warning: unused parameter 't' [-Wunused-parameter]
  604 |   static inline bool isPresent(const T &t) { return true; }
      |                                         ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:5:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/IRBuilder.h:22:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/BasicBlock.h:19:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/ADT/ilist.h:58:34: warning: unused parameter 'V' [-Wunused-parameter]
   58 |   static void deleteNode(NodeTy *V) {}
      |                                  ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:5:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/IRBuilder.h:24:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/ConstantFolder.h:22:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Constants.h:61:41: warning: unused parameter 'From' [-Wunused-parameter]
   61 |   Value *handleOperandChangeImpl(Value *From, Value *To) {
      |                                         ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Constants.h:61:54: warning: unused parameter 'To' [-Wunused-parameter]
   61 |   Value *handleOperandChangeImpl(Value *From, Value *To) {
      |                                                      ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:5:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/IRBuilder.h:24:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/ConstantFolder.h:88:37: warning: unused parameter 'FMF' [-Wunused-parameter]
   88 |                       FastMathFlags FMF) const override {
      |                                     ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/ConstantFolder.h:93:36: warning: unused parameter 'FMF' [-Wunused-parameter]
   93 |                      FastMathFlags FMF) const override {
      |                                    ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/ConstantFolder.h:184:44: warning: unused parameter 'ID' [-Wunused-parameter]
  184 |   Value *FoldBinaryIntrinsic(Intrinsic::ID ID, Value *LHS, Value *RHS, Type *Ty,
      |                                            ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/ConstantFolder.h:184:55: warning: unused parameter 'LHS' [-Wunused-parameter]
  184 |   Value *FoldBinaryIntrinsic(Intrinsic::ID ID, Value *LHS, Value *RHS, Type *Ty,
      |                                                       ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/ConstantFolder.h:184:67: warning: unused parameter 'RHS' [-Wunused-parameter]
  184 |   Value *FoldBinaryIntrinsic(Intrinsic::ID ID, Value *LHS, Value *RHS, Type *Ty,
      |                                                                   ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/ConstantFolder.h:184:78: warning: unused parameter 'Ty' [-Wunused-parameter]
  184 |   Value *FoldBinaryIntrinsic(Intrinsic::ID ID, Value *LHS, Value *RHS, Type *Ty,
      |                                                                              ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/ConstantFolder.h:185:43: warning: unused parameter 'FMFSource' [-Wunused-parameter]
  185 |                              Instruction *FMFSource) const override {
      |                                           ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:5:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/IRBuilder.h:26:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/DataLayout.h:34:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Support/TrailingObjects.h:266:40: warning: unused parameter 'Obj' [-Wunused-parameter]
  266 |   callNumTrailingObjects(const BaseTy *Obj,
      |                                        ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:5:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/IRBuilder.h:34:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Instructions.h:3036:37: warning: unused parameter 'idx' [-Wunused-parameter]
 3036 |   BasicBlock *getSuccessor(unsigned idx) const {
      |                                     ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Instructions.h:3040:30: warning: unused parameter 'idx' [-Wunused-parameter]
 3040 |   void setSuccessor(unsigned idx, BasicBlock *B) {
      |                              ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Instructions.h:3040:47: warning: unused parameter 'B' [-Wunused-parameter]
 3040 |   void setSuccessor(unsigned idx, BasicBlock *B) {
      |                                               ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Instructions.h:4083:37: warning: unused parameter 'idx' [-Wunused-parameter]
 4083 |   BasicBlock *getSuccessor(unsigned idx) const {
      |                                     ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Instructions.h:4087:30: warning: unused parameter 'idx' [-Wunused-parameter]
 4087 |   void setSuccessor(unsigned idx, BasicBlock *NewSucc) {
      |                              ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Instructions.h:4087:47: warning: unused parameter 'NewSucc' [-Wunused-parameter]
 4087 |   void setSuccessor(unsigned idx, BasicBlock *NewSucc) {
      |                                               ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Instructions.h:4541:37: warning: unused parameter 'idx' [-Wunused-parameter]
 4541 |   BasicBlock *getSuccessor(unsigned idx) const {
      |                                     ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Instructions.h:4545:30: warning: unused parameter 'idx' [-Wunused-parameter]
 4545 |   void setSuccessor(unsigned idx, BasicBlock *B) {
      |                              ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/Instructions.h:4545:47: warning: unused parameter 'B' [-Wunused-parameter]
 4545 |   void setSuccessor(unsigned idx, BasicBlock *B) {
      |                                               ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:5:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/IRBuilder.h:1307:55: warning: unused parameter 'OpBundles' [-Wunused-parameter]
 1307 |                            ArrayRef<OperandBundleDef> OpBundles,
      |                                                       ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:7:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:18:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/CGSCCPassManager.h:92:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/LazyCallGraph.h:46:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/TargetLibraryInfo.h:16:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/PassManager.h:630:23: warning: unused parameter 'IR' [-Wunused-parameter]
  630 |   Result run(IRUnitT &IR, AnalysisManager<IRUnitT, ExtraArgTs...> &AM,
      |                       ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/PassManager.h:630:68: warning: unused parameter 'AM' [-Wunused-parameter]
  630 |   Result run(IRUnitT &IR, AnalysisManager<IRUnitT, ExtraArgTs...> &AM,
      |                                                                    ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/PassManager.h:916:34: warning: unused parameter 'Arg' [-Wunused-parameter]
  916 |   PreservedAnalyses run(IRUnitT &Arg, AnalysisManagerT &AM, ExtraArgTs &&...) {
      |                                  ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/IR/PassManager.h:916:57: warning: unused parameter 'AM' [-Wunused-parameter]
  916 |   PreservedAnalyses run(IRUnitT &Arg, AnalysisManagerT &AM, ExtraArgTs &&...) {
      |                                                         ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:7:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:18:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/CGSCCPassManager.h:92:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/LazyCallGraph.h:46:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/TargetLibraryInfo.h:17:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Pass.h:350:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/PassSupport.h:31:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Support/Error.h:304:24: warning: unused parameter 'V' [-Wunused-parameter]
  304 |   void setChecked(bool V) {
      |                        ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:7:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:18:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/CGSCCPassManager.h:92:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/LazyCallGraph.h:48:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Support/Allocator.h:226:31: warning: unused parameter 'Ptr' [-Wunused-parameter]
  226 |   void Deallocate(const void *Ptr, size_t Size, size_t /*Alignment*/) {
      |                               ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Support/Allocator.h:226:43: warning: unused parameter 'Size' [-Wunused-parameter]
  226 |   void Deallocate(const void *Ptr, size_t Size, size_t /*Alignment*/) {
      |                                           ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:7:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:18:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/CGSCCPassManager.h:530:24: warning: unused parameter 'F' [-Wunused-parameter]
  530 |   Result run(Function &F, FunctionAnalysisManager &FAM) { return Result(); }
      |                        ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/CGSCCPassManager.h:530:52: warning: unused parameter 'FAM' [-Wunused-parameter]
  530 |   Result run(Function &F, FunctionAnalysisManager &FAM) { return Result(); }
      |                                                    ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:7:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:19:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/CodeGen/MachinePassManager.h:26:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/CodeGen/MachineFunction.h:26:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/CodeGen/MachineBasicBlock.h:22:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/CodeGen/MachineInstr.h:31:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/MC/MCSymbol.h:18:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/MC/MCExpr.h:477:40: warning: unused parameter 'x' [-Wunused-parameter]
  477 |   virtual bool isEqualTo(const MCExpr *x) const { return false; }
      |                                        ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:7:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:19:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/CodeGen/MachinePassManager.h:26:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/CodeGen/MachineFunction.h:127:27: warning: unused parameter 'Allocator' [-Wunused-parameter]
  127 |   clone(BumpPtrAllocator &Allocator, MachineFunction &DestMF,
      |                           ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/CodeGen/MachineFunction.h:127:55: warning: unused parameter 'DestMF' [-Wunused-parameter]
  127 |   clone(BumpPtrAllocator &Allocator, MachineFunction &DestMF,
      |                                                       ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/CodeGen/MachineFunction.h:128:67: warning: unused parameter 'Src2DstMBB' [-Wunused-parameter]
  128 |         const DenseMap<MachineBasicBlock *, MachineBasicBlock *> &Src2DstMBB)
      |                                                                   ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/CodeGen/MachineFunction.h:499:52: warning: unused parameter 'MI' [-Wunused-parameter]
  499 |     virtual void MF_HandleChangeDesc(MachineInstr &MI, const MCInstrDesc &TID) {
      |                                                    ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/CodeGen/MachineFunction.h:499:75: warning: unused parameter 'TID' [-Wunused-parameter]
  499 |     virtual void MF_HandleChangeDesc(MachineInstr &MI, const MCInstrDesc &TID) {
      |                                                                           ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:7:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:27:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Transforms/IPO/Inliner.h:13:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/InlineAdvisor.h:119:67: warning: unused parameter 'Result' [-Wunused-parameter]
  119 |   virtual void recordUnsuccessfulInliningImpl(const InlineResult &Result) {}
      |                                                                   ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/InlineAdvisor.h:186:48: warning: unused parameter 'SCC' [-Wunused-parameter]
  186 |   virtual void onPassEntry(LazyCallGraph::SCC *SCC = nullptr) {}
      |                                                ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/InlineAdvisor.h:191:47: warning: unused parameter 'SCC' [-Wunused-parameter]
  191 |   virtual void onPassExit(LazyCallGraph::SCC *SCC = nullptr) {}
      |                                               ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/InlineAdvisor.h:300:22: warning: unused parameter 'M' [-Wunused-parameter]
  300 |   Result run(Module &M, ModuleAnalysisManager &MAM) { return {Factory}; }
      |                      ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/InlineAdvisor.h:300:48: warning: unused parameter 'MAM' [-Wunused-parameter]
  300 |   Result run(Module &M, ModuleAnalysisManager &MAM) { return {Factory}; }
      |                                                ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:7:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:29:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Transforms/Scalar/LoopPassManager.h:41:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/LoopInfo.h:21:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Support/GenericLoopInfoImpl.h:17:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/ADT/DepthFirstIterator.h:159:40: warning: unused parameter 'G' [-Wunused-parameter]
  159 |   static df_iterator end(const GraphT &G) { return df_iterator(); }
      |                                        ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/ADT/DepthFirstIterator.h:165:40: warning: unused parameter 'G' [-Wunused-parameter]
  165 |   static df_iterator end(const GraphT &G, SetType &S) { return df_iterator(S); }
      |                                        ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:7:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:29:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Transforms/Scalar/LoopPassManager.h:41:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/LoopInfo.h:21:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Support/GenericLoopInfoImpl.h:18:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/ADT/PostOrderIterator.h:65:42: warning: unused parameter 'From' [-Wunused-parameter]
   65 |   bool insertEdge(std::optional<NodeRef> From, NodeRef To) {
      |                                          ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/ADT/PostOrderIterator.h:70:60: warning: unused parameter 'BB' [-Wunused-parameter]
   70 |   template <typename NodeRef> void finishPostorder(NodeRef BB) {}
      |                                                            ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/ADT/PostOrderIterator.h:86:42: warning: unused parameter 'From' [-Wunused-parameter]
   86 |   bool insertEdge(std::optional<NodeRef> From, NodeRef To) {
      |                                          ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/ADT/PostOrderIterator.h:91:57: warning: unused parameter 'BB' [-Wunused-parameter]
   91 |   template <class NodeRef> void finishPostorder(NodeRef BB) {}
      |                                                         ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/ADT/PostOrderIterator.h:155:40: warning: unused parameter 'G' [-Wunused-parameter]
  155 |   static po_iterator end(const GraphT &G) { return po_iterator(); }
      |                                        ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/ADT/PostOrderIterator.h:160:40: warning: unused parameter 'G' [-Wunused-parameter]
  160 |   static po_iterator end(const GraphT &G, SetType &S) { return po_iterator(S); }
      |                                        ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:7:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:29:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Transforms/Scalar/LoopPassManager.h:41:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/LoopInfo.h:21:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Support/GenericLoopInfoImpl.h:145:46: warning: unused parameter 'BB' [-Wunused-parameter]
  145 |                             [](const BlockT *BB) { return true; });
      |                                              ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:7:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:29:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Transforms/Scalar/LoopPassManager.h:48:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Transforms/Utils/LoopUtils.h:17:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/LoopAccessAnalysis.h:18:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/ScalarEvolution.h:187:74: warning: unused parameter 'IDHash' [-Wunused-parameter]
  187 |   static bool Equals(const SCEV &X, const FoldingSetNodeID &ID, unsigned IDHash,
      |                                                                          ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/ScalarEvolution.h:188:40: warning: unused parameter 'TempID' [-Wunused-parameter]
  188 |                      FoldingSetNodeID &TempID) {
      |                                        ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/ScalarEvolution.h:192:64: warning: unused parameter 'TempID' [-Wunused-parameter]
  192 |   static unsigned ComputeHash(const SCEV &X, FoldingSetNodeID &TempID) {
      |                                                                ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/ScalarEvolution.h:266:31: warning: unused parameter 'IDHash' [-Wunused-parameter]
  266 |                      unsigned IDHash, FoldingSetNodeID &TempID) {
      |                               ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/ScalarEvolution.h:266:57: warning: unused parameter 'TempID' [-Wunused-parameter]
  266 |                      unsigned IDHash, FoldingSetNodeID &TempID) {
      |                                                         ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Analysis/ScalarEvolution.h:271:49: warning: unused parameter 'TempID' [-Wunused-parameter]
  271 |                               FoldingSetNodeID &TempID) {
      |                                                 ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:7:
In file included from /Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:29:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Transforms/Scalar/LoopPassManager.h:255:28: warning: unused parameter 'L' [-Wunused-parameter]
  255 |   void setParentLoop(Loop *L) {
      |                            ^
In file included from /Users/agrossman27/coding/c/tent/src/compiler.cpp:7:
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:905:33: warning: unused parameter 'M' [-Wunused-parameter]
  905 |   PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
      |                                 ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:922:45: warning: unused parameter 'C' [-Wunused-parameter]
  922 |   PreservedAnalyses run(LazyCallGraph::SCC &C, CGSCCAnalysisManager &,
      |                                             ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:923:61: warning: unused parameter 'UR' [-Wunused-parameter]
  923 |                         LazyCallGraph &, CGSCCUpdateResult &UR) {
      |                                                             ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:935:75: warning: unused parameter 'G' [-Wunused-parameter]
  935 |   Result run(LazyCallGraph::SCC &, CGSCCAnalysisManager &, LazyCallGraph &G) {
      |                                                                           ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:942:35: warning: unused parameter 'F' [-Wunused-parameter]
  942 |   PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
      |                                   ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:959:35: warning: unused parameter 'L' [-Wunused-parameter]
  959 |   PreservedAnalyses run(LoopNest &L, LoopAnalysisManager &,
      |                                   ^
/Users/agrossman27/homebrew/Cellar/llvm/21.1.7/include/llvm/Passes/PassBuilder.h:967:31: warning: unused parameter 'L' [-Wunused-parameter]
  967 |   PreservedAnalyses run(Loop &L, LoopAnalysisManager &,
      |                               ^
66 warnings generated.
Undefined symbols for architecture arm64:
  "llvm::AllocaInst::AllocaInst(llvm::Type*, unsigned int, llvm::Value*, llvm::Align, llvm::Twine const&, llvm::InsertPosition)", referenced from:
      llvm::IRBuilderBase::CreateAlloca(llvm::Type*, llvm::Value*, llvm::Twine const&) in ast.cpp.o
  "llvm::BasicBlock::BasicBlock(llvm::LLVMContext&, llvm::Twine const&, llvm::Function*, llvm::BasicBlock*)", referenced from:
      IfStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      IfStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      IfStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      WhileStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      WhileStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      WhileStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      ExpressionStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      ExpressionStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      ...
  "llvm::BranchInst::BranchInst(llvm::BasicBlock*, llvm::User::AllocInfo, llvm::InsertPosition)", referenced from:
      llvm::IRBuilderBase::CreateBr(llvm::BasicBlock*) in ast.cpp.o
  "llvm::BranchInst::BranchInst(llvm::BasicBlock*, llvm::BasicBlock*, llvm::Value*, llvm::User::AllocInfo, llvm::InsertPosition)", referenced from:
      llvm::IRBuilderBase::CreateCondBr(llvm::Value*, llvm::BasicBlock*, llvm::BasicBlock*, llvm::MDNode*, llvm::MDNode*) in ast.cpp.o
  "llvm::ConstantFP::get(llvm::LLVMContext&, llvm::APFloat const&)", referenced from:
      FloatLiteral::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::PGOOptions::~PGOOptions()", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      llvm::PassBuilder::~PassBuilder() in compiler.cpp.o
  "llvm::ReturnInst::ReturnInst(llvm::LLVMContext&, llvm::Value*, llvm::User::AllocInfo, llvm::InsertPosition)", referenced from:
      llvm::IRBuilderBase::CreateRet(llvm::Value*) in ast.cpp.o
  "llvm::StructType::create(llvm::LLVMContext&, llvm::ArrayRef<llvm::Type*>, llvm::StringRef, bool)", referenced from:
      getDynamicValueType(llvm::LLVMContext&) in ast.cpp.o
      CreateEntryBlockAlloca(llvm::Function*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in ast.cpp.o
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      Variable::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      IfStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      ...
  "llvm::APFloatBase::IEEEdouble()", referenced from:
      FloatLiteral::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::APFloatBase::PPCDoubleDouble()", referenced from:
      FloatLiteral::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      llvm::APFloat::~APFloat() in ast.cpp.o
      llvm::detail::DoubleAPFloat::~DoubleAPFloat() in ast.cpp.o
  "llvm::ConstantInt::get(llvm::IntegerType*, unsigned long long, bool)", referenced from:
      IntLiteral::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      BoolLiteral::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      Program::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::Instruction::setIsExact(bool)", referenced from:
      llvm::IRBuilderBase::CreateSDiv(llvm::Value*, llvm::Value*, llvm::Twine const&, bool) in ast.cpp.o
  "llvm::Instruction::setMetadata(unsigned int, llvm::MDNode*)", referenced from:
      llvm::IRBuilderBase::CreateAlloca(llvm::Type*, llvm::Value*, llvm::Twine const&) in ast.cpp.o
      llvm::IRBuilderBase::CreateSDiv(llvm::Value*, llvm::Value*, llvm::Twine const&, bool) in ast.cpp.o
      llvm::IRBuilderBase::CreateSDiv(llvm::Value*, llvm::Value*, llvm::Twine const&, bool) in ast.cpp.o
      llvm::IRBuilderBase::CreateCondBr(llvm::Value*, llvm::BasicBlock*, llvm::BasicBlock*, llvm::MDNode*, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateCondBr(llvm::Value*, llvm::BasicBlock*, llvm::BasicBlock*, llvm::MDNode*, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateCondBr(llvm::Value*, llvm::BasicBlock*, llvm::BasicBlock*, llvm::MDNode*, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateBr(llvm::BasicBlock*) in ast.cpp.o
      ...
  "llvm::Instruction::setFastMathFlags(llvm::FastMathFlags)", referenced from:
      llvm::IRBuilderBase::CreateCall(llvm::FunctionType*, llvm::Value*, llvm::ArrayRef<llvm::Value*>, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateCast(llvm::Instruction::CastOps, llvm::Value*, llvm::Type*, llvm::Twine const&, llvm::MDNode*, llvm::FMFSource) in ast.cpp.o
      llvm::IRBuilderBase::CreateFAddFMF(llvm::Value*, llvm::Value*, llvm::FMFSource, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateFSubFMF(llvm::Value*, llvm::Value*, llvm::FMFSource, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateFMulFMF(llvm::Value*, llvm::Value*, llvm::FMFSource, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateFDivFMF(llvm::Value*, llvm::Value*, llvm::FMFSource, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
  "llvm::Instruction::setHasNoSignedWrap(bool)", referenced from:
      llvm::IRBuilderBase::CreateInsertNUWNSWBinOp(llvm::Instruction::BinaryOps, llvm::Value*, llvm::Value*, llvm::Twine const&, bool, bool) in ast.cpp.o
  "llvm::Instruction::setHasNoUnsignedWrap(bool)", referenced from:
      llvm::IRBuilderBase::CreateInsertNUWNSWBinOp(llvm::Instruction::BinaryOps, llvm::Value*, llvm::Value*, llvm::Twine const&, bool, bool) in ast.cpp.o
  "llvm::Instruction::Instruction(llvm::Type*, unsigned int, llvm::User::AllocInfo, llvm::InsertPosition)", referenced from:
      llvm::CallInst::Create(llvm::FunctionType*, llvm::Value*, llvm::ArrayRef<llvm::Value*>, llvm::ArrayRef<llvm::OperandBundleDefT<llvm::Value*>>, llvm::Twine const&, llvm::InsertPosition) in ast.cpp.o
  "llvm::Instruction::~Instruction()", referenced from:
      llvm::CallInst::Create(llvm::FunctionType*, llvm::Value*, llvm::ArrayRef<llvm::Value*>, llvm::ArrayRef<llvm::OperandBundleDefT<llvm::Value*>>, llvm::Twine const&, llvm::InsertPosition) in ast.cpp.o
  "llvm::LLVMContext::LLVMContext()", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::LLVMContext::~LLVMContext()", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::PassBuilder::crossRegisterProxies(llvm::AnalysisManager<llvm::Loop, llvm::LoopStandardAnalysisResults&>&, llvm::AnalysisManager<llvm::Function>&, llvm::AnalysisManager<llvm::LazyCallGraph::SCC, llvm::LazyCallGraph&>&, llvm::AnalysisManager<llvm::Module>&, llvm::AnalysisManager<llvm::MachineFunction>*)", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::PassBuilder::registerLoopAnalyses(llvm::AnalysisManager<llvm::Loop, llvm::LoopStandardAnalysisResults&>&)", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::PassBuilder::registerCGSCCAnalyses(llvm::AnalysisManager<llvm::LazyCallGraph::SCC, llvm::LazyCallGraph&>&)", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::PassBuilder::registerModuleAnalyses(llvm::AnalysisManager<llvm::Module>&)", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::PassBuilder::registerFunctionAnalyses(llvm::AnalysisManager<llvm::Function>&)", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::PassBuilder::buildPerModuleDefaultPipeline(llvm::OptimizationLevel, llvm::ThinOrFullLTOPhase)", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::PassBuilder::PassBuilder(llvm::TargetMachine*, llvm::PipelineTuningOptions, std::__1::optional<llvm::PGOOptions>, llvm::PassInstrumentationCallbacks*)", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::PassManager<llvm::Module, llvm::AnalysisManager<llvm::Module>>::run(llvm::Module&, llvm::AnalysisManager<llvm::Module>&)", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::PointerType::get(llvm::Type*, unsigned int)", referenced from:
      FunctionCall::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::PointerType::get(llvm::LLVMContext&, unsigned int)", referenced from:
      getDynamicValueType(llvm::LLVMContext&) in ast.cpp.o
      CreateEntryBlockAlloca(llvm::Function*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in ast.cpp.o
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      Variable::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      IfStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      ...
  "llvm::FunctionType::get(llvm::Type*, llvm::ArrayRef<llvm::Type*>, bool)", referenced from:
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      IfStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      ...
  "llvm::FunctionType::get(llvm::Type*, bool)", referenced from:
      Program::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::verifyModule(llvm::Module const&, llvm::raw_ostream*, bool*)", referenced from:
      Program::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::IRBuilderBase::CreateSelect(llvm::Value*, llvm::Value*, llvm::Value*, llvm::Twine const&, llvm::Instruction*)", referenced from:
      FunctionCall::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::IRBuilderBase::CreateGlobalString(llvm::StringRef, llvm::Twine const&, unsigned int, llvm::Module*, bool)", referenced from:
      StrLiteral::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      FunctionCall::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      FunctionCall::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      FunctionCall::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      FunctionCall::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      FunctionCall::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      FunctionCall::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      ...
  "llvm::IRBuilderBase::CreateConstrainedFPCast(unsigned int, llvm::Value*, llvm::Type*, llvm::FMFSource, llvm::Twine const&, llvm::MDNode*, std::__1::optional<llvm::RoundingMode>, std::__1::optional<llvm::fp::ExceptionBehavior>)", referenced from:
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::IRBuilderBase::CreateConstrainedFPBinOp(unsigned int, llvm::Value*, llvm::Value*, llvm::FMFSource, llvm::Twine const&, llvm::MDNode*, std::__1::optional<llvm::RoundingMode>, std::__1::optional<llvm::fp::ExceptionBehavior>)", referenced from:
      llvm::IRBuilderBase::CreateFAddFMF(llvm::Value*, llvm::Value*, llvm::FMFSource, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateFSubFMF(llvm::Value*, llvm::Value*, llvm::FMFSource, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateFMulFMF(llvm::Value*, llvm::Value*, llvm::FMFSource, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateFDivFMF(llvm::Value*, llvm::Value*, llvm::FMFSource, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
  "llvm::BinaryOperator::Create(llvm::Instruction::BinaryOps, llvm::Value*, llvm::Value*, llvm::Twine const&, llvm::InsertPosition)", referenced from:
      llvm::IRBuilderBase::CreateSDiv(llvm::Value*, llvm::Value*, llvm::Twine const&, bool) in ast.cpp.o
      llvm::IRBuilderBase::CreateSDiv(llvm::Value*, llvm::Value*, llvm::Twine const&, bool) in ast.cpp.o
      llvm::IRBuilderBase::CreateInsertNUWNSWBinOp(llvm::Instruction::BinaryOps, llvm::Value*, llvm::Value*, llvm::Twine const&, bool, bool) in ast.cpp.o
      llvm::IRBuilderBase::CreateFAddFMF(llvm::Value*, llvm::Value*, llvm::FMFSource, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateFSubFMF(llvm::Value*, llvm::Value*, llvm::FMFSource, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateFMulFMF(llvm::Value*, llvm::Value*, llvm::FMFSource, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateFDivFMF(llvm::Value*, llvm::Value*, llvm::FMFSource, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
      ...
  "llvm::raw_fd_ostream::close()", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::raw_fd_ostream::raw_fd_ostream(llvm::StringRef, std::__1::error_code&, llvm::sys::fs::OpenFlags)", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::raw_fd_ostream::~raw_fd_ostream()", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::verifyFunction(llvm::Function const&, llvm::raw_ostream*)", referenced from:
      Program::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::AnalysisManager<llvm::LazyCallGraph::SCC, llvm::LazyCallGraph&>::AnalysisManager()", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::AnalysisManager<llvm::Loop, llvm::LoopStandardAnalysisResults&>::AnalysisManager()", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::AnalysisManager<llvm::Module>::AnalysisManager()", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::AnalysisManager<llvm::Function>::AnalysisManager()", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::IRBuilderFolder::~IRBuilderFolder()", referenced from:
      CreateEntryBlockAlloca(llvm::Function*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in ast.cpp.o
      CreateEntryBlockAlloca(llvm::Function*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in ast.cpp.o
      llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>::~IRBuilder() in ast.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::MetadataTracking::track(void*, llvm::Metadata&, llvm::PointerUnion<llvm::MetadataAsValue*, llvm::Metadata*, llvm::DebugValueUser*>)", referenced from:
      llvm::IRBuilderBase::SetInsertPoint(llvm::BasicBlock*, llvm::ilist_iterator_w_bits<llvm::ilist_detail::node_options<llvm::Instruction, false, false, void, true, llvm::BasicBlock>, false, false>) in ast.cpp.o
  "llvm::MetadataTracking::retrack(void*, llvm::Metadata&, void*)", referenced from:
      llvm::IRBuilderBase::SetInsertPoint(llvm::BasicBlock*, llvm::ilist_iterator_w_bits<llvm::ilist_detail::node_options<llvm::Instruction, false, false, void, true, llvm::BasicBlock>, false, false>) in ast.cpp.o
  "llvm::MetadataTracking::untrack(void*, llvm::Metadata&)", referenced from:
      CreateEntryBlockAlloca(llvm::Function*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in ast.cpp.o
      llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>::~IRBuilder() in ast.cpp.o
      llvm::IRBuilderBase::~IRBuilderBase() in ast.cpp.o
      llvm::DebugLoc::~DebugLoc() in ast.cpp.o
      llvm::IRBuilderBase::SetInsertPoint(llvm::BasicBlock*, llvm::ilist_iterator_w_bits<llvm::ilist_detail::node_options<llvm::Instruction, false, false, void, true, llvm::BasicBlock>, false, false>) in ast.cpp.o
      llvm::IRBuilderBase::SetInsertPoint(llvm::BasicBlock*, llvm::ilist_iterator_w_bits<llvm::ilist_detail::node_options<llvm::Instruction, false, false, void, true, llvm::BasicBlock>, false, false>) in ast.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      ...
  "llvm::OptimizationLevel::O3", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::deallocate_buffer(void*, unsigned long, unsigned long)", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      ...
  "llvm::PipelineTuningOptions::PipelineTuningOptions()", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::SymbolTableListTraits<llvm::BasicBlock>::addNodeToList(llvm::BasicBlock*)", referenced from:
      IfStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      WhileStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::DisableABIBreakingChecks", referenced from:
      llvm::VerifyDisableABIBreakingChecks in ast.cpp.o
  "llvm::IRBuilderDefaultInserter::~IRBuilderDefaultInserter()", referenced from:
      CreateEntryBlockAlloca(llvm::Function*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in ast.cpp.o
      CreateEntryBlockAlloca(llvm::Function*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in ast.cpp.o
      llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>::~IRBuilder() in ast.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::Type::getInt32Ty(llvm::LLVMContext&)", referenced from:
      getDynamicValueType(llvm::LLVMContext&) in ast.cpp.o
      CreateEntryBlockAlloca(llvm::Function*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in ast.cpp.o
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      Variable::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      IfStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      ...
  "llvm::Type::getInt64Ty(llvm::LLVMContext&)", referenced from:
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      IntLiteral::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::Type::getDoubleTy(llvm::LLVMContext&)", referenced from:
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::Type::getInt1Ty(llvm::LLVMContext&)", referenced from:
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      BoolLiteral::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      IfStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      WhileStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::Type::getInt8Ty(llvm::LLVMContext&)", referenced from:
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      FunctionCall::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::Type::getVoidTy(llvm::LLVMContext&)", referenced from:
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      FunctionCall::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::User::operator delete(void*)", referenced from:
      llvm::IRBuilderBase::CreateAlloca(llvm::Type*, llvm::Value*, llvm::Twine const&) in ast.cpp.o
      llvm::IRBuilderBase::CreateCondBr(llvm::Value*, llvm::BasicBlock*, llvm::BasicBlock*, llvm::MDNode*, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateBr(llvm::BasicBlock*) in ast.cpp.o
      Program::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      llvm::IRBuilderBase::CreateRet(llvm::Value*) in ast.cpp.o
      llvm::CallInst::Create(llvm::FunctionType*, llvm::Value*, llvm::ArrayRef<llvm::Value*>, llvm::ArrayRef<llvm::OperandBundleDefT<llvm::Value*>>, llvm::Twine const&, llvm::InsertPosition) in ast.cpp.o
      llvm::CallInst::Create(llvm::FunctionType*, llvm::Value*, llvm::ArrayRef<llvm::Value*>, llvm::ArrayRef<llvm::OperandBundleDefT<llvm::Value*>>, llvm::Twine const&, llvm::InsertPosition) in ast.cpp.o
      ...
  "llvm::User::operator new(unsigned long, llvm::User::HungOffOperandsAllocMarker)", referenced from:
      Program::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::User::operator new(unsigned long, llvm::User::IntrusiveOperandsAllocMarker)", referenced from:
      llvm::IRBuilderBase::CreateAlloca(llvm::Type*, llvm::Value*, llvm::Twine const&) in ast.cpp.o
      llvm::IRBuilderBase::CreateCondBr(llvm::Value*, llvm::BasicBlock*, llvm::BasicBlock*, llvm::MDNode*, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateBr(llvm::BasicBlock*) in ast.cpp.o
      llvm::IRBuilderBase::CreateRet(llvm::Value*) in ast.cpp.o
      llvm::IRBuilderBase::CreateAlignedLoad(llvm::Type*, llvm::Value*, llvm::MaybeAlign, bool, llvm::Twine const&) in ast.cpp.o
      llvm::IRBuilderBase::CreateAlignedStore(llvm::Value*, llvm::Value*, llvm::MaybeAlign, bool) in ast.cpp.o
  "llvm::User::operator new(unsigned long, llvm::User::IntrusiveOperandsAndDescriptorAllocMarker)", referenced from:
      llvm::CallInst::Create(llvm::FunctionType*, llvm::Value*, llvm::ArrayRef<llvm::Value*>, llvm::ArrayRef<llvm::OperandBundleDefT<llvm::Value*>>, llvm::Twine const&, llvm::InsertPosition) in ast.cpp.o
  "llvm::Module::getOrInsertFunction(llvm::StringRef, llvm::FunctionType*)", referenced from:
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      IfStmt::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      ...
  "llvm::Module::Module(llvm::StringRef, llvm::LLVMContext&)", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::Module::~Module()", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::detail::IEEEFloat::IEEEFloat(double)", referenced from:
      FloatLiteral::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::detail::IEEEFloat::~IEEEFloat()", referenced from:
      FloatLiteral::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      FloatLiteral::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      FloatLiteral::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      llvm::APFloat::~APFloat() in ast.cpp.o
      llvm::detail::DoubleAPFloat::~DoubleAPFloat() in ast.cpp.o
  "llvm::APFloat::Storage::Storage(llvm::detail::IEEEFloat, llvm::fltSemantics const&)", referenced from:
      FloatLiteral::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::CallInst::init(llvm::FunctionType*, llvm::Value*, llvm::ArrayRef<llvm::Value*>, llvm::ArrayRef<llvm::OperandBundleDefT<llvm::Value*>>, llvm::Twine const&)", referenced from:
      llvm::CallInst::Create(llvm::FunctionType*, llvm::Value*, llvm::ArrayRef<llvm::Value*>, llvm::ArrayRef<llvm::OperandBundleDefT<llvm::Value*>>, llvm::Twine const&, llvm::InsertPosition) in ast.cpp.o
  "llvm::CastInst::Create(llvm::Instruction::CastOps, llvm::Value*, llvm::Type*, llvm::Twine const&, llvm::InsertPosition)", referenced from:
      llvm::IRBuilderBase::CreateCast(llvm::Instruction::CastOps, llvm::Value*, llvm::Type*, llvm::Twine const&, llvm::MDNode*, llvm::FMFSource) in ast.cpp.o
  "llvm::Function::Function(llvm::FunctionType*, llvm::GlobalValue::LinkageTypes, unsigned int, llvm::Twine const&, llvm::Module*)", referenced from:
      Program::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
  "llvm::LoadInst::LoadInst(llvm::Type*, llvm::Value*, llvm::Twine const&, bool, llvm::Align, llvm::InsertPosition)", referenced from:
      llvm::IRBuilderBase::CreateAlignedLoad(llvm::Type*, llvm::Value*, llvm::MaybeAlign, bool, llvm::Twine const&) in ast.cpp.o
  "llvm::StoreInst::StoreInst(llvm::Value*, llvm::Value*, bool, llvm::Align, llvm::InsertPosition)", referenced from:
      llvm::IRBuilderBase::CreateAlignedStore(llvm::Value*, llvm::Value*, llvm::MaybeAlign, bool) in ast.cpp.o
  "llvm::BasicBlock::getContext() const", referenced from:
      CreateEntryBlockAlloca(llvm::Function*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in ast.cpp.o
  "llvm::BasicBlock::getDataLayout() const", referenced from:
      llvm::IRBuilderBase::CreateAlloca(llvm::Type*, llvm::Value*, llvm::Twine const&) in ast.cpp.o
      llvm::IRBuilderBase::CreateAlignedLoad(llvm::Type*, llvm::Value*, llvm::MaybeAlign, bool, llvm::Twine const&) in ast.cpp.o
      llvm::IRBuilderBase::CreateAlignedStore(llvm::Value*, llvm::Value*, llvm::MaybeAlign, bool) in ast.cpp.o
  "llvm::DataLayout::getABITypeAlign(llvm::Type*) const", referenced from:
      llvm::IRBuilderBase::CreateAlignedLoad(llvm::Type*, llvm::Value*, llvm::MaybeAlign, bool, llvm::Twine const&) in ast.cpp.o
      llvm::IRBuilderBase::CreateAlignedStore(llvm::Value*, llvm::Value*, llvm::MaybeAlign, bool) in ast.cpp.o
  "llvm::DataLayout::getPrefTypeAlign(llvm::Type*) const", referenced from:
      llvm::IRBuilderBase::CreateAlloca(llvm::Type*, llvm::Value*, llvm::Twine const&) in ast.cpp.o
  "llvm::StructType::containsHomogeneousTypes() const", referenced from:
      llvm::FPMathOperator::isComposedOfHomogeneousFloatingPointTypes(llvm::Type*) in ast.cpp.o
  "llvm::Instruction::getStableDebugLoc() const", referenced from:
      llvm::IRBuilderBase::SetInsertPoint(llvm::BasicBlock*, llvm::ilist_iterator_w_bits<llvm::ilist_detail::node_options<llvm::Instruction, false, false, void, true, llvm::BasicBlock>, false, false>) in ast.cpp.o
  "llvm::AttributeList::addAttributeAtIndex(llvm::LLVMContext&, unsigned int, llvm::Attribute::AttrKind) const", referenced from:
      llvm::IRBuilderBase::CreateCall(llvm::FunctionType*, llvm::Value*, llvm::ArrayRef<llvm::Value*>, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
  "llvm::IRBuilderBase::SetInstDebugLocation(llvm::Instruction*) const", referenced from:
      llvm::IRBuilderBase::CreateAlloca(llvm::Type*, llvm::Value*, llvm::Twine const&) in ast.cpp.o
      llvm::IRBuilderBase::CreateSDiv(llvm::Value*, llvm::Value*, llvm::Twine const&, bool) in ast.cpp.o
      llvm::IRBuilderBase::CreateCondBr(llvm::Value*, llvm::BasicBlock*, llvm::BasicBlock*, llvm::MDNode*, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateBr(llvm::BasicBlock*) in ast.cpp.o
      llvm::IRBuilderBase::CreateRet(llvm::Value*) in ast.cpp.o
      llvm::IRBuilderBase::CreateCall(llvm::FunctionType*, llvm::Value*, llvm::ArrayRef<llvm::Value*>, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
      llvm::IRBuilderBase::CreateAlignedLoad(llvm::Type*, llvm::Value*, llvm::MaybeAlign, bool, llvm::Twine const&) in ast.cpp.o
      ...
  "llvm::Type::isIntegerTy(unsigned int) const", referenced from:
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      BinaryOp::codegen(llvm::LLVMContext&, llvm::IRBuilderBase&, llvm::Module&) in ast.cpp.o
      ...
  "llvm::Type::getPointerTo(unsigned int) const", referenced from:
      boxPrimitive(llvm::Value*, llvm::LLVMContext&, llvm::IRBuilder<llvm::ConstantFolder, llvm::IRBuilderDefaultInserter>&, llvm::Module&) in ast.cpp.o
  "llvm::Value::getContext() const", referenced from:
      llvm::IRBuilderBase::CreateCall(llvm::FunctionType*, llvm::Value*, llvm::ArrayRef<llvm::Value*>, llvm::Twine const&, llvm::MDNode*) in ast.cpp.o
  "llvm::Module::print(llvm::raw_ostream&, llvm::AssemblyAnnotationWriter*, bool, bool) const", referenced from:
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
  "llvm::Function::getContext() const", referenced from:
      CreateEntryBlockAlloca(llvm::Function*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in ast.cpp.o
  "vtable for llvm::ConstantFolder", referenced from:
      CreateEntryBlockAlloca(llvm::Function*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in ast.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
   NOTE: a missing vtable usually means the first non-inline virtual member function has no definition.
  "vtable for llvm::IRBuilderDefaultInserter", referenced from:
      CreateEntryBlockAlloca(llvm::Function*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in ast.cpp.o
      Compiler::compile(Program*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in compiler.cpp.o
   NOTE: a missing vtable usually means the first non-inline virtual member function has no definition.
ld: symbol(s) not found for architecture arm64
clang++: error: linker command failed with exit code 1 (use -v to see invocation)
make[2]: *** [tent] Error 1
make[1]: *** [CMakeFiles/tent.dir/all] Error 2
make: *** [all] Error 2
