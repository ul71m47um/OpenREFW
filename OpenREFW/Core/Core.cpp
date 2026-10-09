#include "Core.h"

#define MKHEX(x) std::format("{:#x}", x)

import disxx.loader.macho.Loader;
import disxx.disasm.Disassembler;
import disxx.disasm.Printer;

import std;

Core::Core(void *pContext, Callback callback) noexcept
    : m_pContext{pContext}
    , m_Callback{callback}
{}

std::vector<std::string> Core::Disassemble(const std::string path) const noexcept
{
	std::vector<std::string> strings{};

	// Load the executable
    disxx::loader::macho::Loader ldr{};
    const auto result{ldr.Load(path)};
    if (!result) [[unlikely]]
    {
        this->m_Callback
        (
            this->m_pContext,
            std::format
            (
                "Error loading {}: {}",
                path,
                result.error().what()
            )
        );
        
        return std::vector<std::string>{};
    }
    
	for (auto &section : result->GetSections())
	{
		const auto name{section.GetName()};
		strings.emplace_back(std::format("{:#016x}: .section {}", section.GetAddress(), name));

		// these sections are considered as executable
		if (name == "__TEXT,__text" || name == "__TEXT,__stubs")
		{
    		std::unordered_map<std::uint64_t, std::string> names;
    		for (const auto &label : section.GetLabels())
    		    names[label.GetAddress()] = label.GetName();
		
			for (const auto &label : section.GetLabels())
		    {
				strings.emplace_back(std::format("{:#016x}: {}:", label.GetAddress(), label.GetName()));

				disxx::disasm::Disassembler disasm{};
				const auto vec
				{
					label.GetData<std::uint32_t>()
						| std::views::all
						| std::views::transform([](const auto &bytes) -> auto { return disxx::disasm::Bytes{bytes}; })
						| std::ranges::to<std::vector<disxx::disasm::Bytes>>()
				};
	   		    for (disxx::disasm::Address addr{label.GetAddress()}; const auto &bytes : vec)
    		    {
					if (const auto &insn{disasm.Disassemble(bytes, addr++)}) [[likely]]
					{
						auto mnemonic
						{
							[&insn] -> std::string
							{
								std::string str{};
								disxx::disasm::Printer<std::back_insert_iterator<std::string>> printer{std::back_inserter(str)};
								printer.Print(*insn);
					
								return str;
							}()
						};

						if (auto insnAddr{insn->GetProgramCounterRelevantAddress()})
    		        	{
							#pragma clang diagnostic push
							#pragma clang diagnostic ignored "-Wsign-conversion"
    		            	if (auto it{names.find(*insnAddr)}; it != names.end())
    		            	#pragma clang diagnostic pop
							{
								strings.emplace_back
    		            	    (
									std::format
									(
										"{:#016x}:\t{}\t; {:#x}",
    		            	        	integer(addr) - 4,
                                        std::regex_replace
										(
											mnemonic,
											std::regex
											{
												std::string{"#"}
													+ MKHEX(*insnAddr)
											},
											it->second
										),
										label.GetAddress()
									)
								);
	   		            	    
								continue;
	   		            	}
	   		        	}

						strings.push_back(std::format("{:#016x}:\t{}", integer(addr) - 4, mnemonic));
					}
					else
						strings.emplace_back(std::format("{:#016x}:\t{}", integer(addr) - 4, insn.error().what()));
				}
    		}
		}
		else
		{
			for (const auto &label : section.GetLabels())
			{
				strings.emplace_back(std::format("{:#016x}: {}:", label.GetAddress(), label.GetName()));
                for (std::uint64_t addr{label.GetAddress()}; const auto &byte : label.GetData<std::uint8_t>())
				{
					strings.emplace_back
					(
						std::format
						(
							"{:#016x}:\t.byte {:#02x}{}",
							addr++,
                            byte,
							std::isprint(byte) && !std::isspace(byte)
								? std::format("\t; \'{:c}\'", byte)
								: std::string{}
						)
					);
				}
			}
		}
        
        this->m_Callback(this->m_pContext, std::format("Disassembled: {} section", section.GetName()));
	}

    this->m_Callback(this->m_pContext, std::format("Disassembling of {} done!", path));
    
	return std::move(strings);
}

std::vector<Core::Section> Core::ParseSections(const std::string path) const noexcept
{
    std::vector<Section> sects{};

    // Load the executable
    disxx::loader::macho::Loader ldr{};
    const auto result{ldr.Load(path)};
    if (!result) [[unlikely]]
        std::vector<std::string>{};
   
    const auto &sections{result->GetSections()};
    for (auto parsed{0ul}; auto &section : sections)
    {
        std::vector<std::string> labels{};
        for (const auto &label : section.GetLabels())
            labels.emplace_back(std::string{label.GetName()});
        sects.emplace_back
        (
            Section
            {
                .labels = std::move(labels),
                .name = std::string{section.GetName()},
            }
        );
        
        this->m_Callback
        (
            this->m_pContext,
            std::format
            (
                "Parsed {} section(s) out of {}",
                ++parsed,
                sections.size()
            )
        );
    }
    
    return std::move(sects);
}
