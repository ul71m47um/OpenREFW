export module disxx.loader.macho.Loader;

export import disxx.loader.abstract.Loader;

export namespace disxx::loader::macho
{
	class __attribute__((visibility("default"))) [[nodiscard]] Loader final : public abstract::Loader
	{
	  public:
		explicit Loader(void) noexcept;
		
		Loader(const Loader &) noexcept;
		Loader &operator=(const Loader &) noexcept;

		Loader(Loader &&) noexcept;
		Loader &operator=(Loader &&) noexcept;

		virtual ~Loader(void) noexcept override = default;
	
		virtual bool Loadable(const std::filesystem::path) noexcept override;

		virtual std::expected<executable::ExecutableFile, LoaderError> Load(const std::filesystem::path) noexcept override;
	};
} /* disxx::loader::macho */
