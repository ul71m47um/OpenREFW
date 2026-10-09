module;

#if !__has_include(<mach/machine.h>)
#	define CPU_TYPE_ARM64 0x0000000100000c
#endif

#if __has_include(<mach-o/loader.h>)
#	include <mach-o/loader.h>
#else
#	define	MH_OBJECT		0x1
#	define	MH_EXECUTE		0x2
#	define	MH_CORE			0x4
#	define	MH_DYLIB		0x6
#	define	LC_SEGMENT		0x1
#	define	LC_SYMTAB		0x2
#	define	LC_SYMSEG		0x3
#	define	LC_THREAD		0x4
#	define	LC_UNIXTHREAD	0x5
#	define	LC_LOADFVMLIB	0x6
#	define	LC_IDFVMLIB		0x7
#	define	LC_IDENT		0x8
#	define 	LC_FVMFILE		0x9
#	define 	LC_PREPAGE     	0xa
#	define	LC_DYSYMTAB		0xb
#	define	LC_LOAD_DYLIB	0xc
#	define	LC_ID_DYLIB		0xd
#	define 	LC_LOAD_DYLINKER 0xe
#	define 	LC_ID_DYLINKER	0xf
#	define 	LC_PREBOUND_DYLIB 0x10
#	define	LC_ROUTINES		0x11
#	define	LC_SUB_FRAMEWORK 0x12
#	define	LC_SUB_UMBRELLA	0x13
#	define	LC_SUB_CLIENT	0x14
#	define	LC_SUB_LIBRARY  0x15
#	define	LC_TWOLEVEL_HINTS 0x16
#	define	LC_PREBIND_CKSUM 0x17
	extern "C"
	{
		struct mach_header_64
		{
		  public:
			std::uint32_t	magic;
			std::int32_t	cputype;
			std::int32_t	cpusubtype;
			std::uint32_t	filetype;
			std::uint32_t	ncmds;
			std::uint32_t	sizeofcmds;
			std::uint32_t	flags;
			std::uint32_t	reserved;
		};

		struct load_command
		{
		  public:
			uint32_t cmd;
			uint32_t cmdsize;
		};

		struct segment_command_64
		{
		  public:
			std::uint32_t	cmd;
			std::uint32_t	cmdsize;
			char			segname[16];
			std::uint64_t	vmaddr;
			std::uint64_t	vmsize;
			std::uint64_t	fileoff;
			std::uint64_t	filesize;
			std::int32_t	maxprot;
			std::int32_t	initprot;
			std::uint32_t	nsects;
			std::uint32_t	flags;
		};

		struct section_64
		{
		  public:
			char			sectname[16];
			char			segname[16];
			std::uint64_t	addr;
			std::uint64_t	size;
			std::uint32_t	offset;
			std::uint32_t	align;
			std::uint32_t	reloff;
			std::uint32_t	nreloc;
			std::uint32_t	flags;
			std::uint32_t	reserved1;
			std::uint32_t	reserved2;
			std::uint32_t	reserved3;
		};

		struct symtab_command
		{
		  public:
			std::uint32_t	cmd;
			std::uint32_t	cmdsize;
			std::uint32_t	symoff;
			std::uint32_t	nsyms;
			std::uint32_t	stroff;
			std::uint32_t	strsize;
		};
	}
#endif

#if __has_include(<mach-o/nlist.h>)
#	include <mach-o/nlist.h>
#else
#	define	N_STAB	0xe0
#	define	N_PEXT	0x10
#	define	N_TYPE	0x0e
#	define	N_EXT	0x01
#	define	N_UNDF	0x0
#	define	N_ABS	0x2
#	define	N_SECT	0xe
#	define	N_PBUD	0xc
#	define	N_INDR	0xa
#	define	NO_SECT	0
#	define MAX_SECT	255
	extern "C"
	{
		struct nlist_64
		{
		  public:
	    	union { std::uint32_t n_strx; } n_un;
	    	std::uint8_t n_type;
	    	std::uint8_t n_sect;
	    	std::uint16_t n_desc;
	    	std::uint64_t n_value;
		};
	}
#endif

#if __has_include(<mach-o/fat.h>)
#	include <mach-o/fat.h>
#else
#	define FAT_MAGIC	0xcafebabe
#	define FAT_CIGAM	0xbebafeca
	extern "C"
	{
		struct fat_header
		{
		  public:
			std::uint32_t	magic;
			std::uint32_t	nfat_arch;
		};

		struct fat_arch
		{
		  public:
			std::int32_t		cputype;
			std::int32_t		cpusubtype;
			std::uint32_t		offset;
			std::uint32_t		size;
			std::uint32_t		align;
		};
	}
#endif

#ifndef _WIN32
#	include <arpa/inet.h>
#else
#	include <winsock2.h>
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpedantic"
#define MKARGS(...) this, __VA_ARGS__
#pragma clang diagnostic pop

module disxx.loader.macho.Loader;

import disxx.utility.file.MappedFile;

import std;

namespace disxx::loader::macho
{
	Loader::Loader(void) noexcept
		: abstract::Loader{}
	{}

	Loader::Loader(const Loader &other) noexcept
		: abstract::Loader{other}
	{}

	Loader &Loader::operator=(const Loader &other) noexcept
	{
		if (this != &other) [[likely]]
			this->m_Mapper = other.m_Mapper;
		return *this;
	}

	Loader::Loader(Loader &&other) noexcept
		: abstract::Loader{std::move(other)}
	{}

	Loader &Loader::operator=(Loader &&other) noexcept
	{
		if (this != &other) [[likely]]
			this->m_Mapper = std::move(other.m_Mapper);
		return *this;
	}

	bool Loader::Loadable(const std::filesystem::path path) noexcept
	{
		if (!this->m_Mapper.Open(path)) [[unlikely]]
			return false;
		if (const auto magicResult{this->m_Mapper.Read<std::uint32_t>(0)}) [[likely]]
			return magicResult->get() == FAT_CIGAM || magicResult->get() == MH_MAGIC_64;
		return false;
	}

	std::expected<executable::ExecutableFile, LoaderError> Loader::Load(const std::filesystem::path path) noexcept
	{ 
		disxx::loader::executable::ExecutableFile exec{};

		if (!this->Loadable(path)) [[unlikely]]
            return std::unexpected{LoaderError{"LoaderError"}};

		if (!this->m_Mapper.Open(path)) [[unlikely]]
            return std::unexpected{LoaderError{"LoaderError"}};
        
		// I really like that 0xCAFEBABE magic number :D
		auto headerOffset{0ull};
		if (const auto magicResult{this->m_Mapper.Read<std::uint32_t>(0)})
		{
			if (magicResult->get() == FAT_CIGAM)
			{
				const auto hdrResult{this->m_Mapper.Read<fat_header>(0)};
				if (!hdrResult) [[unlikely]]
                    return std::unexpected{LoaderError{"LoaderError"}};
                
				std::vector<fat_arch> vec{ntohl(hdrResult->get().nfat_arch)};
				for (auto i{sizeof(fat_header)}; i < ntohl(hdrResult->get().nfat_arch) * sizeof(fat_arch) + sizeof(fat_header); i += sizeof(fat_arch))
				{
					const auto archResult{this->m_Mapper.Read<fat_arch>(i)};
					if (!archResult) [[unlikely]]
                        return std::unexpected{LoaderError{"LoaderError"}};
                    
					vec.emplace_back
					(
						[](const auto &arch) -> fat_arch
						{
							// Translating integers to little endian byte order
							return fat_arch
							{
								static_cast<std::int32_t>(ntohl(arch.cputype)),
								static_cast<std::int32_t>(ntohl(arch.cpusubtype)),
								ntohl(arch.offset),
								ntohl(arch.size),
								ntohl(arch.align)
							};
						}(archResult->get())
					);
				}
	
				const auto it 
				{
					std::ranges::find_if
					(
						vec,
						[](const auto &arch) -> bool
						{ return arch.cputype == CPU_TYPE_ARM64; }
					)
				};
				if (it == vec.end()) [[unlikely]]
                    return std::unexpected{LoaderError{"LoaderError"}};
				
				headerOffset = it->offset;
			}
		}
		else
            return std::unexpected{LoaderError{"LoaderError"}};
		const auto hdrResult{this->m_Mapper.Read<mach_header_64>(headerOffset)};
		if (!hdrResult) [[unlikely]]
            return std::unexpected{LoaderError{"LoaderError"}};

		const auto &header{hdrResult->get()};
		if (header.magic != MH_MAGIC_64) [[unlikely]]
            return std::unexpected{LoaderError{"LoaderError"}};
		else if (header.cputype != CPU_TYPE_ARM64) [[unlikely]]
            return std::unexpected{LoaderError{"LoaderError"}};
		
		exec.SetDescription
		(
			[&] -> std::string_view
			{
				static const std::flat_map<std::uint32_t, std::string_view> fileTypeTable
				{
					{MH_OBJECT,		"relocatable object file"},
					{MH_EXECUTE,	"demand paged executable file"},
					{MH_CORE,		"core file"},
					{MH_DYLIB,		"dynamically bound shared library"}
				};

				if (fileTypeTable.contains(header.filetype)) [[likely]]
					return fileTypeTable.at(header.filetype);
				return "unknown";
			}()
		);
		exec.SetMagic(header.magic);
		
		auto offset{headerOffset + sizeof(mach_header_64)};
		unsigned short int sectionIndex{1};
		typename std::invoke_result
		<
			decltype(&utility::file::MappedFile::Read<load_command>),
			decltype(this->m_Mapper),
			decltype(offset)
		>::type loadCommandResult{std::cref(*std::make_unique<load_command>())};
		for (const auto _ : std::views::iota(0u, header.ncmds))
		{
			loadCommandResult = this->m_Mapper.Read<load_command>(offset);
			if (!loadCommandResult) [[unlikely]]
                return std::unexpected{LoaderError{"LoaderError"}};
            
			if (loadCommandResult->get().cmd == LC_SEGMENT_64)
			{
				const auto segmentCommandResult{this->m_Mapper.Read<segment_command_64>(offset)};
				if (!segmentCommandResult) [[unlikely]]
                    return std::unexpected{LoaderError{"LoaderError"}};

				auto pSects{std::make_unique<section_64[]>(segmentCommandResult->get().nsects)};
				for (const auto j : std::views::iota(0u, segmentCommandResult->get().nsects))
				{
					auto nSectionResult{this->m_Mapper.Read<section_64>((offset + sizeof(segmentCommandResult->get())) + (j * sizeof(section_64)))};
					if (!nSectionResult) [[unlikely]]
                        return std::unexpected{LoaderError{"LoaderError"}};

					if (!nSectionResult->get().offset) [[unlikely]]
					{
						sectionIndex++;
						continue;
					}

					disxx::loader::executable::Section section{};
					section.SetName
					(
						std::format
						(
							"{},{}",
							// Cut some null-terminators by wrapping ptr into a str
							std::string{nSectionResult->get().segname},
							// The same thing here
							std::string{nSectionResult->get().sectname}
						)
					);
					section.SetAddress(nSectionResult->get().addr);
					section.SetOffset(headerOffset + nSectionResult->get().offset);
					section.SetSize(nSectionResult->get().size);
					section.SetIndex(sectionIndex++);
				
					exec.AddSection(std::move(section));
				}
			}
			else if (loadCommandResult->get().cmd == LC_SYMTAB)
			{
				const auto symtabCommandResult{this->m_Mapper.Read<symtab_command>(offset)};
				if (!symtabCommandResult) [[unlikely]]
                    return std::unexpected{LoaderError{"LoaderError"}};

				std::vector<nlist_64> symbols{};
				symbols.reserve(symtabCommandResult->get().nsyms);
				for (const auto j : std::views::iota(0u, symtabCommandResult->get().nsyms))
				{
					const auto nlistResult{this->m_Mapper.Read<nlist_64>(symtabCommandResult->get().symoff + j * sizeof(nlist_64) + headerOffset)};
					if (!nlistResult) [[unlikely]]
                        return std::unexpected{LoaderError{"LoaderError"}};
	
					symbols.push_back(nlistResult->get());
				}

				std::vector<char> strtab{};
				strtab.reserve(symtabCommandResult->get().strsize);
				for (const auto j : std::views::iota(0u, symtabCommandResult->get().strsize))
				{
					const auto byteResult{this->m_Mapper.Read<char>(symtabCommandResult->get().stroff + j + headerOffset)};
					if (!byteResult) [[unlikely]]
                        return std::unexpected{LoaderError{"LoaderError"}};

					strtab.push_back(*byteResult);
				}

				for (const auto j : std::views::iota(0u, symtabCommandResult->get().nsyms))
				{
					if (!symbols[j].n_un.n_strx)
						continue;
					else if ((symbols[j].n_type & N_TYPE) != N_SECT || (symbols[j].n_type & N_STAB))
						continue;
					
					const auto it
					{
						// Find the section with the same number as an argument
						std::ranges::find_if
						(
							exec.GetSections(),
							[&symbols, j](const auto &section) -> bool
							{ return section.GetIndex() == symbols[j].n_sect; }
						)
					};
				
					// If not found, just pass it
					if (it == exec.GetSections().end()) [[unlikely]]
						continue;
	
					auto start{0ull};
					if (const auto addr{it->GetAddress()}; symbols[j].n_value >= addr && symbols[j].n_value < addr + it->GetSize())
						start = it->GetOffset() + (symbols[j].n_value - addr);
				
					disxx::loader::executable::Label label{};
					label.SetName(&strtab.at(symbols[j].n_un.n_strx));
					label.SetAddress(symbols.at(j).n_value);
					label.SetOffset(start);
					it->AddLabel(std::move(label));
				}

				break;
			}
			
			offset += loadCommandResult->get().cmdsize;
		}

		for (auto &sect : exec.GetSections())
		{
			for (auto it{sect.GetLabels().begin()}; it != sect.GetLabels().end(); ++it)
			{
				std::vector<uint8_t> data{};
				for (const auto i : std::views::iota(it->GetOffset(), std::next(it) != sect.GetLabels().end() ? std::next(it)->GetOffset() : sect.GetSize() + sect.GetOffset()))
				{
					const auto byteResult{this->m_Mapper.Read<std::uint8_t>(i)};
					if (!byteResult) [[unlikely]]
                        return std::unexpected{LoaderError{"LoaderError"}};

					data.push_back(*byteResult);
				}

				it->SetData(std::move(data));
			}
		}

		return std::move(exec);
	}

	/*
	std::expected
	<
		disxx::loader::utility::BinaryInfo,
		std::variant
		<
			std::out_of_range,
			std::range_error
		>
	>
	Loader::LoadMetadata(void) const noexcept
	{
		disxx::loader::utility::BinaryInfo metadata{};

		std::expected
		<
			std::reference_wrapper<const load_command>,
			std::variant
			<
				std::out_of_range,
				std::range_error
			>
		> loadCommandResult{std::cref(*std::make_unique<load_command>())};
		std::uint64_t offset{this->m_Offset + sizeof(mach_header_64)};
		for (const auto _ : std::views::iota(0u, header.ncmds))
		{
			if (loadCommandResult = this->m_Mapper.Read<load_command>(offset); !loadCommandResult) [[unlikely]]
				return mkerr<&Loader::LoadMetadata>(loadCommandResult.error(), this);

			if (loadCommandResult->get().cmd == LC_BUILD_VERSION)
			{
				const auto buildVersionCommandResult{this->m_Mapper.Read<build_version_command>(offset)};
				if (!buildVersionCommandResult) [[unlikely]]
					mkerr<&Loader::LoadMetadata>(buildVersionCommandResult.error(), this);
				metadata.SetBuildVersion(buildVersionCommandResult->get());
				
				for (auto index{offset + sizeof(build_version_command)}; index < offset + buildVersionCommandResult->get().cmdsize; index += sizeof(build_tool_version))
				{
					const auto buildToolVersionResult{this->m_Mapper.Read<build_tool_version>(index)};
					if (!buildToolVersionResult) [[unlikely]]
						mkerr<&Loader::LoadMetadata>(buildToolVersionResult.error(), this);
					metadata.SetBuildTool(*buildToolVersionResult);
				}

				return metadata;
			}

			offset += loadCommandResult->get().cmdsize;
		}

		return metadata;
	}

	std::optional<std::uint64_t> Loader::LoadImageBase(void) const noexcept
	{
		std::expected
		<
			std::reference_wrapper<const load_command>,
			std::variant
			<
				std::out_of_range,
				std::range_error
			>
		> loadCommandResult{std::cref(*std::make_unique<load_command>())};
		std::uint64_t offset{this->m_Offset + sizeof(mach_header_64)};
		for (const auto _ : std::views::iota(0u, header.ncmds))
		{
			if (loadCommandResult = this->m_Mapper.Read<load_command>(offset); !loadCommandResult) [[unlikely]]
				return std::nullopt;
			if (loadCommandResult->get().cmd == LC_SEGMENT_64)
				if (const auto segmentCommandResult{this->m_Mapper.Read<segment_command_64>(offset)}; segmentCommandResult && std::string{segmentCommandResult->get().segname} == "__TEXT")
					return segmentCommandResult->get().vmaddr - segmentCommandResult->get().fileoff;
			offset += loadCommandResult->get().cmdsize;
		}

		return std::nullopt;
	}

	std::string_view Loader::GetFileType(void) const noexcept
	{
		const static std::unordered_map<std::uint32_t, std::string_view> fileTypeTable
		{
			{MH_OBJECT,		"relocatable object file"},
			{MH_EXECUTE,	"demand paged executable file"},
			{MH_CORE,		"core file"},
			{MH_DYLIB,		"dynamically bound shared library"}
		};

		if (!this->m_pHeader) [[unlikely]]
			return "unknown";
		else if (!fileTypeTable.contains(header.filetype)) [[unlikely]]
			return "unknown";
		else [[likely]]
			return fileTypeTable.at(header.filetype);
	}
	*/
} /* disxx::loader::macho */
