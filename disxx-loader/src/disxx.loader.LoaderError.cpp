module disxx.loader.LoaderError;

namespace disxx::loader
{
	LoaderError::LoaderError(std::string_view err) noexcept
		: std::runtime_error{err.data()}
	{}
} /* disxx::loader */
