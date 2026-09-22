export module disxx.disasm.decoder.abstract.Decoder;

export import disxx.utility.error.DisassemblyError;
import disxx.utility.pointer.NonNull;

export import disxx.disasm.InstructionIdentifier;
export import disxx.disasm.operand.IOperand;

export import disxx.disasm.decoder.abstract.SubDecoder;

export import std;

export namespace disxx::disasm::decoder::abstract
{
	class __attribute__((visibility("default"))) [[nodiscard]] Decoder
	{
      protected:
		// Subdecoder (in some cases may become nullptr)
		disxx::utility::pointer::NonNull<SubDecoder> m_pSubDecoder{};
		
		// Instruction's address
		std::uint64_t m_ProgramCounter{};
		
		// Instruction's bytes
		std::uint32_t m_Insn{};

		// Explicit padding
		std::uint32_t m_Pad{};

	  protected:
		virtual std::expected
		<
			std::unique_ptr<SubDecoder>,
			disxx::utility::error::DisassemblyError
		> __GetDecoder(void) const noexcept = 0;

  	  public:
		explicit Decoder(void) noexcept;
		explicit Decoder(std::uint32_t, std::uint64_t) noexcept;
		explicit Decoder(std::unique_ptr<SubDecoder> &&) noexcept;

		explicit Decoder(Decoder &&) noexcept;
		Decoder &operator=(Decoder &&) noexcept;

		virtual ~Decoder(void) noexcept;

		bool HasProgramCounterRelevantAddress(void) const noexcept;

		DisassemblyResult Decode(void) noexcept;
	};
 
    Decoder::Decoder(void) noexcept
        : m_pSubDecoder{}
        , m_ProgramCounter{}
        , m_Insn{}
    {}

    Decoder::Decoder(std::uint32_t insn, std::uint64_t programCounter) noexcept
        : m_pSubDecoder{}
        , m_ProgramCounter{programCounter}
        , m_Insn{insn}
    {}

    Decoder::Decoder(Decoder &&other) noexcept
        : m_pSubDecoder{std::move(other.m_pSubDecoder)}
        , m_ProgramCounter{std::move(other.m_ProgramCounter)}
        , m_Insn{std::move(other.m_Insn)}
    {}

    Decoder &Decoder::operator=(Decoder &&other) noexcept
    {
        if (this != &other) [[likely]]
        {
            if (this->m_pSubDecoder) [[likely]]
                this->m_pSubDecoder.Delete();
            this->m_pSubDecoder = std::move(other.m_pSubDecoder);
            this->m_ProgramCounter = std::move(other.m_ProgramCounter);
            this->m_Insn = std::move(other.m_Insn);
        }

        return *this;
    }

    Decoder::~Decoder(void) noexcept
    {
        if (this->m_pSubDecoder) [[likely]]
            this->m_pSubDecoder.Delete();
    }

    bool Decoder::HasProgramCounterRelevantAddress(void) const noexcept
    {
        return this
            ->m_pSubDecoder
            ->GetProgramCounterRelevantAddress()
            .has_value();
    }

    DisassemblyResult Decoder::Decode(void) noexcept
    {
        if (auto decoder{this->__GetDecoder()}) [[likely]]
            this->m_pSubDecoder = decoder->release();
        else
            return std::unexpected{disxx::utility::error::DisassemblyError{this->m_Insn}};

        return this
            ->m_pSubDecoder
            ->Decode();
    }
} /* disxx::disasm::decoder::abstract */
