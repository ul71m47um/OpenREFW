#define __INTERNAL_CORE_INCLUDE
#include <Core.h>

#define MKHEX(x) std::format("{:#x}", x)

import disxx.utility.error.NullPointerError;
import disxx.utility.ini.Parser;

import disxx.loader.executable.ExecutableFile;
import disxx.loader.macho.Loader;

import disxx.disasm.Disassembler;
import disxx.disasm.Printer;

import std;

std::vector<std::string> Core::Disassemble(const std::string path) noexcept
{
	std::vector<std::string> strings{};

	// Load the executable
    disxx::loader::macho::Loader ldr{};
	if (!ldr.LoadFile(path)) [[unlikely]]
		std::vector<std::string>{};
	
	// Load metadata of the executable
	auto metadataResult{ldr.LoadMetadata()};
	if (!metadataResult) [[unlikely]]
		std::vector<std::string>{};

	const auto dataResult{ldr.LoadData()};
	if (!dataResult) [[unlikely]]
		std::vector<std::string>{};

	for (auto &section : dataResult->GetSections())
	{
		const auto name{section.GetName()};
		strings.emplace_back(std::format(".section {}", name));

		// these sections are considered as executable
		if (name == "__TEXT,__text" || name == "__TEXT,__stubs")
		{
    		std::unordered_map<std::uint64_t, std::string> names;
    		for (const auto &label : section.GetLabels())
    		    names[label.GetAddress()] = label.GetName();
		
			for (const auto &label : section.GetLabels())
		    {
				strings.emplace_back(std::format("{}:", label.GetName()));

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
										"{} ; {:#x}",
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

						strings.push_back(mnemonic);
					}
					else
						strings.emplace_back(insn.error().what());
				}
    		}
		}
		else
		{
			for (const auto &label : section.GetLabels())
			{
				strings.emplace_back(std::format("{}:", label.GetName()));
				for (const auto &byte : label.GetData<std::uint8_t>())
				{
					strings.emplace_back
					(
						std::format
						(
							".byte {:#02x}{}",
							byte,
							std::isprint(byte) && !std::isspace(byte)
								? std::format(" ; \'{:c}\'", byte)
								: std::string{}
						)
					);
				}
			}
		}
	}

	return strings;
}
