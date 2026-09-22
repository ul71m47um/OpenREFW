// Use libc++ module if the file is included in Core.cpp
#ifndef __INTERNAL_CORE_INCLUDE
#   include <vector>
#   include <string>
#else
    import std;
#endif

#ifndef Core_h
#define Core_h

// Internal class, working with C++ core,
// which manage the disassembling process
class __attribute__((visibility("default"))) [[nodiscard]] Core
{
  public:
    // Returns disassembled code
    static std::vector<std::string> Disassemble(const std::string) noexcept;
};

#endif /* Core_h */
