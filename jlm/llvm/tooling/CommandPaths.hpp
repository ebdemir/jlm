 #ifndef JLM_TOOLING_COMMANDPATHS_HPP
 #define JLM_TOOLING_COMMANDPATHS_HPP
 
 #include <jlm/util/file.hpp>
 
 namespace jlm {
 
 static inline filepath clangpath("/usr/bin/clang");
 static inline filepath llcpath("/usr/bin/llc");
 static inline filepath firtoolpath("/bin/firtool");
 static inline filepath verilatorpath("verilator_bin");
 static inline filepath verilatorrootpath("/usr/share/verilator");
 
 }
 
 #endif 