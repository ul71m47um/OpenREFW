export module disxx.disasm.DisassemblyError;

export import std;

export namespace disxx::disasm
{
	class __attribute__((visibility("default"))) [[nodiscard]] DisassemblyError : public std::exception
	{
	  private:
		std::string m_Error{};

	  public:
		explicit DisassemblyError(void) noexcept;
		// TODO: use disxx::disasm::Bytes instead
		explicit DisassemblyError(std::uint32_t) noexcept;

		bool operator==(const DisassemblyError &) noexcept;
		bool operator!=(const DisassemblyError &) noexcept;

		//virtual ~DisassemblyError(void) noexcept override = default;
		virtual const char *what(void) const noexcept override;
	};
} /* disxx::disasm */
