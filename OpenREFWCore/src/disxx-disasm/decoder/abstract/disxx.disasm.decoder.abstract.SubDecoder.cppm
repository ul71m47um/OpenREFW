export module disxx.disasm.decoder.abstract.SubDecoder;

export import disxx.utility.error.DisassemblyError;

export import disxx.disasm.InstructionIdentifier;
export import disxx.disasm.operand.IOperand;

export using DisassemblyResult = std::expected
<
	std::pair
	<
		InstructionIdentifier,
		std::vector
		<
			std::unique_ptr
			<disxx::disasm::operand::IOperand>
		>
	>,
	disxx::utility::error::DisassemblyError
>;

export namespace disxx::disasm::decoder::abstract
{
	class __attribute__((visibility("default"))) [[nodiscard]] SubDecoder
	{
	  protected:
		// m_Operands will be moved from this class after Decode call!
		[[maybe_unused]] mutable std::vector
		<
			// These pointers CAN'T be zero - 
			// - there is no opportunity somewhere in the code to
			// create a nullptr for this vector
			std::unique_ptr
			<disxx::disasm::operand::IOperand>
		> m_Operands{};

		// Possible reference to one of immediate operands
		mutable std::optional<std::reference_wrapper<disxx::disasm::operand::IOperand>>
		m_ProgramCounterRelevantAddress{};
	
		// Program counter address	
		[[maybe_unused]] std::uint64_t m_ProgramCounter{};
	
		// Bytes of the instruction (4 bytes for AArch64)
		std::uint32_t m_Insn{};
		
		// explicit padding
		[[maybe_unused]] std::uint32_t m_Padding{};

	  public:
		explicit SubDecoder(void) noexcept;
		explicit SubDecoder(std::uint32_t, std::uint64_t) noexcept;

		virtual ~SubDecoder(void) noexcept = default;

		explicit SubDecoder(const SubDecoder &) noexcept;
		SubDecoder &operator=(const SubDecoder &) noexcept;

		explicit SubDecoder(SubDecoder &&) noexcept;
		SubDecoder &operator=(SubDecoder &&) noexcept;

		const std::optional<std::reference_wrapper<disxx::disasm::operand::IOperand>> &
		GetProgramCounterRelevantAddress(void) const noexcept;

		virtual std::unique_ptr<SubDecoder> Clone(void) const noexcept = 0;
		virtual DisassemblyResult Decode(void) const = 0;
	};
 
    SubDecoder::SubDecoder(void) noexcept
        : m_Operands{}
        , m_ProgramCounterRelevantAddress{std::nullopt}
        , m_ProgramCounter{0x0000000000000000}
        , m_Insn{0x00000000}
        , m_Padding{0x00000000}
    {}

    SubDecoder::SubDecoder(std::uint32_t insn, std::uint64_t addr) noexcept
        : m_Operands{}
        , m_ProgramCounterRelevantAddress{std::nullopt}
        , m_ProgramCounter{addr}
        , m_Insn{insn}
        , m_Padding{0x00000000}
    {}

    SubDecoder::SubDecoder(const SubDecoder &other) noexcept
        : m_Operands{}
        , m_ProgramCounterRelevantAddress{std::nullopt}
        , m_ProgramCounter{other.m_ProgramCounter}
        , m_Insn{other.m_Insn}
        , m_Padding{0x00000000}
    {
        this->m_Operands.reserve(other.m_Operands.size());
        for (auto i{0ul}; const auto &opr : other.m_Operands)
        {
            this->m_Operands.at(i++) = opr->Clone();
            if (other.m_ProgramCounterRelevantAddress)
                if (&other.m_ProgramCounterRelevantAddress->get() == opr.get())
                    this->m_ProgramCounterRelevantAddress.value() = std::ref(*this->m_Operands.at(i - 1).get());
        }
    }

    SubDecoder &SubDecoder::operator=(const SubDecoder &other) noexcept
    {
        if (this != &other)
        {
            for (auto &opr : this->m_Operands)
                opr.release();
            this->m_Operands.resize(other.m_Operands.size());
            for (auto i{0ul}; const auto &opr : other.m_Operands)
            {
                this->m_Operands.at(i++) = opr->Clone();
                if (other.m_ProgramCounterRelevantAddress)
                    if (&other.m_ProgramCounterRelevantAddress.value().get() == opr.get())
                        this->m_ProgramCounterRelevantAddress.value() = std::ref(*this->m_Operands.at(i - 1).get());
            }

            this->m_ProgramCounterRelevantAddress = other.m_ProgramCounterRelevantAddress;
            this->m_ProgramCounter = other.m_ProgramCounter;
            this->m_Insn = other.m_Insn;
        }

        return *this;
    }

    // TODO: Check if this works
    SubDecoder::SubDecoder(SubDecoder &&other) noexcept
        : m_Operands{std::move(other.m_Operands)}
        , m_ProgramCounterRelevantAddress{std::move(other.m_ProgramCounterRelevantAddress)}
        , m_ProgramCounter{std::move(other.m_ProgramCounter)}
        , m_Insn{std::move(other.m_Insn)}
        , m_Padding{0x00000000}
    {}

    // TODO: Check if this works
    SubDecoder &SubDecoder::operator=(SubDecoder &&other) noexcept
    {
        for (auto &opr : this->m_Operands)
            opr.release();
        this->m_Operands = std::move(other.m_Operands);
        this->m_ProgramCounterRelevantAddress = std::move(other.m_ProgramCounterRelevantAddress);
        this->m_ProgramCounter = std::move(other.m_ProgramCounter);
        this->m_Insn = std::move(other.m_Insn);
    
        return *this;
    }

    const std::optional<std::reference_wrapper<disxx::disasm::operand::IOperand>> &
    SubDecoder::GetProgramCounterRelevantAddress(void) const noexcept
    { return this->m_ProgramCounterRelevantAddress; }
} /* disxx::disasm::analyzer::abstract */
