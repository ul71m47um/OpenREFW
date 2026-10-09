#ifndef Core_h
#define Core_h

#include <functional>
#include <vector>
#include <string>

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
