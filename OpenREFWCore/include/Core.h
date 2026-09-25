// Use libc++ module if the file is included in Core.cpp
#ifndef __INTERNAL_CORE_INCLUDE
#   include <functional>
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
    using Callback = std::function<void(void *, std::string)>;
    
  private:
    void *m_pContext{nullptr};
    Callback m_Callback{};
    
  public:
    struct Section
    {
      public:
        std::vector<std::string> labels{};
        std::string name{};
    };
    
  public:
    explicit Core(void) noexcept;
    explicit Core(void *, Callback) noexcept;
    
    Core(const Core &) noexcept = default;
    Core &operator=(const Core &) noexcept = default;
    
    std::vector<std::string> Disassemble(const std::string) const noexcept;
    std::vector<Section> ParseSections(const std::string) const noexcept;
};

#endif /* Core_h */
