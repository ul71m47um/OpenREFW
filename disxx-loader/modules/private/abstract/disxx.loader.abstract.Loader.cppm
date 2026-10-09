export module disxx.loader.abstract.Loader;

export import disxx.loader.executable.ExecutableFile;
export import disxx.loader.LoaderError;

import disxx.utility.file.MappedFile;

export namespace disxx::loader::abstract
{
	class __attribute__((visibility("hidden"))) [[nodiscard]] Loader
	{
	  protected:
		utility::file::MappedFile m_Mapper{};

	  public:
		explicit Loader(void) noexcept = default;

		explicit Loader(const Loader &) noexcept = default;
		Loader &operator=(const Loader &) noexcept = default;

		virtual ~Loader(void) noexcept = default;
  
        virtual bool Loadable(const std::filesystem::path) noexcept = 0;

		virtual std::expected<executable::ExecutableFile, LoaderError> Load(const std::filesystem::path) noexcept = 0;
	};
} /* disxx::loader::abstract */
