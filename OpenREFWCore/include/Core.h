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
    struct Section
    {
      public:
        std::vector<std::string> labels{};
        std::string name{};
    };
    
  public:
    // Returns disassembled code
    static std::vector<std::string> Disassemble(const std::string) noexcept;
    static std::vector<Section> ParseSections(const std::string) noexcept;
};

#endif /* Core_h */
