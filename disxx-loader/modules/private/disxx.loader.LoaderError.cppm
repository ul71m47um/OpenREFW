export module disxx.loader.LoaderError;

export import std;

export namespace disxx::loader
{
    class __attribute__((visibility("default"))) [[nodiscard]] LoaderError final : public std::runtime_error
    {
      public:
        explicit LoaderError(void) noexcept = default;
       
		explicit LoaderError(std::string_view) noexcept;
 
        LoaderError(const LoaderError &) noexcept = default;
        LoaderError &operator=(const LoaderError &) noexcept = default;
    };
} /* disxx::loader */
