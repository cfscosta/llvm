//===- COFF.h - COFF object file implementation -----------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the COFFObjectFile class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_OBJECT_COFF_H
#define LLVM_OBJECT_COFF_H

#include "llvm/Object/ObjectFile.h"
#include "llvm/Support/COFF.h"
#include "llvm/Support/Endian.h"

namespace llvm {
  template <typename T>
  class ArrayRef;

namespace object {
class ImportDirectoryEntryRef;
class ExportDirectoryEntryRef;
typedef content_iterator<ImportDirectoryEntryRef> import_directory_iterator;
typedef content_iterator<ExportDirectoryEntryRef> export_directory_iterator;

/// The DOS compatible header at the front of all PE/COFF executables.
struct dos_header {
  support::ulittle16_t Magic;
  support::ulittle16_t UsedBytesInTheLastPage;
  support::ulittle16_t FileSizeInPages;
  support::ulittle16_t NumberOfRelocationItems;
  support::ulittle16_t HeaderSizeInParagraphs;
  support::ulittle16_t MinimumExtraParagraphs;
  support::ulittle16_t MaximumExtraParagraphs;
  support::ulittle16_t InitialRelativeSS;
  support::ulittle16_t InitialSP;
  support::ulittle16_t Checksum;
  support::ulittle16_t InitialIP;
  support::ulittle16_t InitialRelativeCS;
  support::ulittle16_t AddressOfRelocationTable;
  support::ulittle16_t OverlayNumber;
  support::ulittle16_t Reserved[4];
  support::ulittle16_t OEMid;
  support::ulittle16_t OEMinfo;
  support::ulittle16_t Reserved2[10];
  support::ulittle32_t AddressOfNewExeHeader;
};

struct coff_file_header {
  support::ulittle16_t Machine;
  support::ulittle16_t NumberOfSections;
  support::ulittle32_t TimeDateStamp;
  support::ulittle32_t PointerToSymbolTable;
  support::ulittle32_t NumberOfSymbols;
  support::ulittle16_t SizeOfOptionalHeader;
  support::ulittle16_t Characteristics;

  bool isImportLibrary() const { return NumberOfSections == 0xffff; }
};

/// The 32-bit PE header that follows the COFF header.
struct pe32_header {
  support::ulittle16_t Magic;
  uint8_t  MajorLinkerVersion;
  uint8_t  MinorLinkerVersion;
  support::ulittle32_t SizeOfCode;
  support::ulittle32_t SizeOfInitializedData;
  support::ulittle32_t SizeOfUninitializedData;
  support::ulittle32_t AddressOfEntryPoint;
  support::ulittle32_t BaseOfCode;
  support::ulittle32_t BaseOfData;
  support::ulittle32_t ImageBase;
  support::ulittle32_t SectionAlignment;
  support::ulittle32_t FileAlignment;
  support::ulittle16_t MajorOperatingSystemVersion;
  support::ulittle16_t MinorOperatingSystemVersion;
  support::ulittle16_t MajorImageVersion;
  support::ulittle16_t MinorImageVersion;
  support::ulittle16_t MajorSubsystemVersion;
  support::ulittle16_t MinorSubsystemVersion;
  support::ulittle32_t Win32VersionValue;
  support::ulittle32_t SizeOfImage;
  support::ulittle32_t SizeOfHeaders;
  support::ulittle32_t CheckSum;
  support::ulittle16_t Subsystem;
  support::ulittle16_t DLLCharacteristics;
  support::ulittle32_t SizeOfStackReserve;
  support::ulittle32_t SizeOfStackCommit;
  support::ulittle32_t SizeOfHeapReserve;
  support::ulittle32_t SizeOfHeapCommit;
  support::ulittle32_t LoaderFlags;
  support::ulittle32_t NumberOfRvaAndSize;
};

namespace clrtables{
	struct module{ // 0x00
		support::ulittle16_t Generation;
		support::ulittle16_t Name;
		support::ulittle16_t Mvid;
		support::ulittle16_t EndId;
		support::ulittle16_t EncBaseId;
	};

	struct typeRef{
		support::ulittle16_t ResolutionScope;
		support::ulittle16_t TypeName;
		support::ulittle16_t TypeNamespace;
	};

	struct typeDef{
		support::ulittle32_t Flags;
		support::ulittle16_t TypeName;
		support::ulittle16_t TypeNamespace;
		support::ulittle16_t Extends;
		support::ulittle16_t FieldList;
		support::ulittle16_t MethodList;
	};

	struct methodDef {
		support::ulittle32_t RVA;
		support::ulittle16_t ImplFlags;
		support::ulittle16_t Flags;
		support::ulittle16_t Name;
		support::ulittle16_t Signature;
		support::ulittle16_t ParamList;
	};

	struct memberRef {
		support::ulittle16_t Class;
		support::ulittle16_t Name;
		support::ulittle16_t Signature;
	};

	struct standAloneSig {
		support::ulittle16_t signature;
	};

	struct assemblyRef {
		support::ulittle16_t MajorVersion;
		support::ulittle16_t MinorVersion;
		support::ulittle16_t BuildNumber;
		support::ulittle16_t RevisionNumber;
		support::ulittle32_t Flags;
		support::ulittle16_t PublicKeyOrToken;
		support::ulittle16_t Name;
		support::ulittle16_t Culture;
		support::ulittle16_t HashValue;
	};

	struct clrTablePtr {
		module* Module; //0x00
		uint32_t ModuleSize;
		typeRef* TypeRef; //0x01
		uint32_t TypeRefSize;
		typeDef* TypeDef; //0x02
		uint32_t TypeDefSize;
		methodDef* MethodDef; //0x06
		uint32_t MethodDefSize;
		memberRef* MemberRef; // 0x0a
		uint32_t MemberRefSize;
		standAloneSig* StandAloneSig; //0x11
		uint32_t StandAloneSigSize;
		assemblyRef* AssemblyRef;  // 0x23
		uint32_t AssemblyRefSize;
	};
}


struct clr_header {
	support::ulittle32_t Cb;
	support::ulittle16_t MajorRuntimeVersion;
	support::ulittle16_t MinorRuntimeVersion;
	support::ulittle32_t MetadataRVA;
	support::ulittle32_t MetadataSize;
	support::ulittle32_t ImageFlags;
	support::ulittle32_t EntryToken;
	support::ulittle32_t ResourcesRVA;
	support::ulittle32_t ResourcesSize;
	support::ulittle64_t StrongNameSignature;
	support::ulittle64_t CodeManagerTable;
	support::ulittle64_t VTableFixups;
	support::ulittle64_t ExportAddressTableJumps;
	support::ulittle64_t ManagedNativeHeader;
};	

struct clrstream_header {
	support::ulittle32_t offset; //0
	support::ulittle32_t size; // <= 255 round up to multiple of 4
	char * name;
};

struct clrmeta_tables_head {
	support::ulittle32_t Reserved;
	support::ulittle8_t MajorVersion;
	support::ulittle8_t MinorVersion;
	support::ulittle8_t heapsizes;
	support::ulittle8_t Reservedbyte;
	support::ulittle64_t Valid;
	support::ulittle64_t Sorted;
	unsigned int rowsize;
	support::ulittle32_t * Rows;
	clrtables::clrTablePtr tablePtr;
};


struct clrmeta_header {
	uintptr_t MetadataInitPtr;
	support::ulittle32_t Signature; //0x424a5342
	support::ulittle16_t MajorRuntimeVersion; //1
	support::ulittle16_t MinorRuntimeVersion; //1
	support::ulittle32_t Reserved; //0
	support::ulittle32_t Length; // <= 255 round up to multiple of 4
	char * Version;
	support::ulittle16_t Flags;
	support::ulittle16_t Streams;
	clrstream_header * StreamHeaders;
	clrmeta_tables_head * MetaTables;
};	


error_code        initMetadataPtr(uintptr_t MetadataIntPtr, llvm::object::clrmeta_header **MetadataHeader);
error_code		  initMetadataTablesSetup(llvm::object::clrmeta_header *MetadataHeader);
void			  setupTablePointers(uintptr_t MetadataIntPtr, llvm::object::clrmeta_header *MetadataHeader);
		

struct stream_header {
	support::ulittle32_t Signature; //0x424a5342
	support::ulittle16_t MajorRuntimeVersion; //1
	support::ulittle16_t MinorRuntimeVersion; //1
	support::ulittle32_t Reserved; //0
	support::ulittle32_t Length; // <= 255 round up to multiple of 4
	char * Version;
	support::ulittle16_t Flags;
	support::ulittle16_t Streams;
	clrstream_header * StreamHeaders;
};

/// The 64-bit PE header that follows the COFF header.
struct pe32plus_header {
  support::ulittle16_t Magic;
  uint8_t  MajorLinkerVersion;
  uint8_t  MinorLinkerVersion;
  support::ulittle32_t SizeOfCode;
  support::ulittle32_t SizeOfInitializedData;
  support::ulittle32_t SizeOfUninitializedData;
  support::ulittle32_t AddressOfEntryPoint;
  support::ulittle32_t BaseOfCode;
  support::ulittle64_t ImageBase;
  support::ulittle32_t SectionAlignment;
  support::ulittle32_t FileAlignment;
  support::ulittle16_t MajorOperatingSystemVersion;
  support::ulittle16_t MinorOperatingSystemVersion;
  support::ulittle16_t MajorImageVersion;
  support::ulittle16_t MinorImageVersion;
  support::ulittle16_t MajorSubsystemVersion;
  support::ulittle16_t MinorSubsystemVersion;
  support::ulittle32_t Win32VersionValue;
  support::ulittle32_t SizeOfImage;
  support::ulittle32_t SizeOfHeaders;
  support::ulittle32_t CheckSum;
  support::ulittle16_t Subsystem;
  support::ulittle16_t DLLCharacteristics;
  support::ulittle64_t SizeOfStackReserve;
  support::ulittle64_t SizeOfStackCommit;
  support::ulittle64_t SizeOfHeapReserve;
  support::ulittle64_t SizeOfHeapCommit;
  support::ulittle32_t LoaderFlags;
  support::ulittle32_t NumberOfRvaAndSize;
};

struct data_directory {
  support::ulittle32_t RelativeVirtualAddress;
  support::ulittle32_t Size;
};

struct import_directory_table_entry {
  support::ulittle32_t ImportLookupTableRVA;
  support::ulittle32_t TimeDateStamp;
  support::ulittle32_t ForwarderChain;
  support::ulittle32_t NameRVA;
  support::ulittle32_t ImportAddressTableRVA;
};

struct import_address_table_entry{
	support::ulittle16_t Hint;
	char *name;
};

struct import_lookup_table_entry32 {
  support::ulittle32_t data;

  bool isOrdinal() const { return data & 0x80000000; }

  uint16_t getOrdinal() const {
    assert(isOrdinal() && "ILT entry is not an ordinal!");
    return data & 0xFFFF;
  }

  uint32_t getHintNameRVA() const {
    assert(!isOrdinal() && "ILT entry is not a Hint/Name RVA!");
    return data;
  }
};

struct export_directory_table_entry {
  support::ulittle32_t ExportFlags;
  support::ulittle32_t TimeDateStamp;
  support::ulittle16_t MajorVersion;
  support::ulittle16_t MinorVersion;
  support::ulittle32_t NameRVA;
  support::ulittle32_t OrdinalBase;
  support::ulittle32_t AddressTableEntries;
  support::ulittle32_t NumberOfNamePointers;
  support::ulittle32_t ExportAddressTableRVA;
  support::ulittle32_t NamePointerRVA;
  support::ulittle32_t OrdinalTableRVA;
};

union export_address_table_entry {
  support::ulittle32_t ExportRVA;
  support::ulittle32_t ForwarderRVA;
};

typedef support::ulittle32_t export_name_pointer_table_entry;
typedef support::ulittle16_t export_ordinal_table_entry;

struct coff_symbol {
  struct StringTableOffset {
    support::ulittle32_t Zeroes;
    support::ulittle32_t Offset;
  };

  union {
    char ShortName[8];
    StringTableOffset Offset;
  } Name;

  support::ulittle32_t Value;
  support::little16_t SectionNumber;

  support::ulittle16_t Type;

  support::ulittle8_t  StorageClass;
  support::ulittle8_t  NumberOfAuxSymbols;

  uint8_t getBaseType() const {
    return Type & 0x0F;
  }

  uint8_t getComplexType() const {
    return (Type & 0xF0) >> 4;
  }
};

struct coff_section {
  char Name[8];
  support::ulittle32_t VirtualSize;
  support::ulittle32_t VirtualAddress;
  support::ulittle32_t SizeOfRawData;
  support::ulittle32_t PointerToRawData;
  support::ulittle32_t PointerToRelocations;
  support::ulittle32_t PointerToLinenumbers;
  support::ulittle16_t NumberOfRelocations;
  support::ulittle16_t NumberOfLinenumbers;
  support::ulittle32_t Characteristics;
};

struct coff_relocation {
  support::ulittle32_t VirtualAddress;
  support::ulittle32_t SymbolTableIndex;
  support::ulittle16_t Type;
};

struct coff_aux_weak_external {
  support::ulittle32_t TagIndex;
  support::ulittle32_t Characteristics;
  char Unused[10];
};

struct coff_aux_section_definition {
  support::ulittle32_t Length;
  support::ulittle16_t NumberOfRelocations;
  support::ulittle16_t NumberOfLinenumbers;
  support::ulittle32_t CheckSum;
  support::ulittle16_t Number;
  support::ulittle8_t Selection;
  char Unused[3];
};

struct coff_load_configuration32 {
  support::ulittle32_t Characteristics;
  support::ulittle32_t TimeDateStamp;
  support::ulittle16_t MajorVersion;
  support::ulittle16_t MinorVersion;
  support::ulittle32_t GlobalFlagsClear;
  support::ulittle32_t GlobalFlagsSet;
  support::ulittle32_t CriticalSectionDefaultTimeout;
  support::ulittle32_t DeCommitFreeBlockThreshold;
  support::ulittle32_t DeCommitTotalFreeThreshold;
  support::ulittle32_t LockPrefixTable;
  support::ulittle32_t MaximumAllocationSize;
  support::ulittle32_t VirtualMemoryThreshold;
  support::ulittle32_t ProcessAffinityMask;
  support::ulittle32_t ProcessHeapFlags;
  support::ulittle16_t CSDVersion;
  uint16_t Reserved;
  support::ulittle32_t EditList;
  support::ulittle32_t SecurityCookie;
  support::ulittle32_t SEHandlerTable;
  support::ulittle32_t SEHandlerCount;
};

struct coff_runtime_function_x64 {
  support::ulittle32_t BeginAddress;
  support::ulittle32_t EndAddress;
  support::ulittle32_t UnwindInformation;
};

class COFFObjectFile : public ObjectFile {
private:
  friend class ImportDirectoryEntryRef;
  friend class ExportDirectoryEntryRef;
  const coff_file_header *COFFHeader;
  const pe32_header      *PE32Header;
  const pe32plus_header  *PE32PlusHeader;
  const clr_header       *CLRHeader;
  clrmeta_header   *MetadataHeader;
  const data_directory   *DataDirectory;
  const coff_section     *SectionTable;
  const coff_symbol      *SymbolTable;
  const char             *StringTable;
        uint32_t          StringTableSize;
  const import_directory_table_entry *ImportDirectory;
        uint32_t          NumberOfImportDirectory;
  const export_directory_table_entry *ExportDirectory;

        error_code        getString(uint32_t offset, StringRef &Res) const;

  const coff_symbol      *toSymb(DataRefImpl Symb) const;
  const coff_section     *toSec(DataRefImpl Sec) const;
  const coff_relocation  *toRel(DataRefImpl Rel) const;

		error_code        initExportTablePtr();
        error_code        initSymbolTablePtr();
        error_code        initImportTablePtr();
		error_code        initCLRHeaderPtr();
		bool isPureCil();

protected:
  void moveSymbolNext(DataRefImpl &Symb) const LLVM_OVERRIDE;
  error_code getSymbolName(DataRefImpl Symb, StringRef &Res) const
      LLVM_OVERRIDE;
  error_code getSymbolAddress(DataRefImpl Symb, uint64_t &Res) const
      LLVM_OVERRIDE;
  error_code getSymbolFileOffset(DataRefImpl Symb, uint64_t &Res) const
      LLVM_OVERRIDE;
  error_code getSymbolSize(DataRefImpl Symb, uint64_t &Res) const LLVM_OVERRIDE;
  uint32_t getSymbolFlags(DataRefImpl Symb) const LLVM_OVERRIDE;
  error_code getSymbolType(DataRefImpl Symb, SymbolRef::Type &Res) const
      LLVM_OVERRIDE;
  error_code getSymbolSection(DataRefImpl Symb, section_iterator &Res) const
      LLVM_OVERRIDE;
  error_code getSymbolValue(DataRefImpl Symb, uint64_t &Val) const
      LLVM_OVERRIDE;
  void moveSectionNext(DataRefImpl &Sec) const LLVM_OVERRIDE;
  error_code getSectionName(DataRefImpl Sec, StringRef &Res) const
      LLVM_OVERRIDE;
  error_code getSectionAddress(DataRefImpl Sec, uint64_t &Res) const
      LLVM_OVERRIDE;
  error_code getSectionSize(DataRefImpl Sec, uint64_t &Res) const LLVM_OVERRIDE;
  error_code getSectionContents(DataRefImpl Sec, StringRef &Res) const
      LLVM_OVERRIDE;
  error_code getSectionAlignment(DataRefImpl Sec, uint64_t &Res) const
      LLVM_OVERRIDE;
  error_code isSectionText(DataRefImpl Sec, bool &Res) const LLVM_OVERRIDE;
  error_code isSectionData(DataRefImpl Sec, bool &Res) const LLVM_OVERRIDE;
  error_code isSectionBSS(DataRefImpl Sec, bool &Res) const LLVM_OVERRIDE;
  error_code isSectionVirtual(DataRefImpl Sec, bool &Res) const LLVM_OVERRIDE;
  error_code isSectionZeroInit(DataRefImpl Sec, bool &Res) const LLVM_OVERRIDE;
  error_code isSectionReadOnlyData(DataRefImpl Sec, bool &Res) const
      LLVM_OVERRIDE;
  error_code isSectionRequiredForExecution(DataRefImpl Sec, bool &Res) const
      LLVM_OVERRIDE;
  error_code sectionContainsSymbol(DataRefImpl Sec, DataRefImpl Symb,
                                   bool &Result) const LLVM_OVERRIDE;
  relocation_iterator section_rel_begin(DataRefImpl Sec) const LLVM_OVERRIDE;
  relocation_iterator section_rel_end(DataRefImpl Sec) const LLVM_OVERRIDE;

  void moveRelocationNext(DataRefImpl &Rel) const LLVM_OVERRIDE;
  error_code getRelocationAddress(DataRefImpl Rel, uint64_t &Res) const
      LLVM_OVERRIDE;
  error_code getRelocationOffset(DataRefImpl Rel, uint64_t &Res) const
      LLVM_OVERRIDE;
  symbol_iterator getRelocationSymbol(DataRefImpl Rel) const LLVM_OVERRIDE;
  error_code getRelocationType(DataRefImpl Rel, uint64_t &Res) const
      LLVM_OVERRIDE;
  error_code getRelocationTypeName(DataRefImpl Rel,
                                   SmallVectorImpl<char> &Result) const
      LLVM_OVERRIDE;
  error_code getRelocationValueString(DataRefImpl Rel,
                                      SmallVectorImpl<char> &Result) const
      LLVM_OVERRIDE;

  error_code getLibraryNext(DataRefImpl LibData, LibraryRef &Result) const
      LLVM_OVERRIDE;
  error_code getLibraryPath(DataRefImpl LibData, StringRef &Result) const
      LLVM_OVERRIDE;

public:
  COFFObjectFile(MemoryBuffer *Object, error_code &EC, bool BufferOwned = true);
  basic_symbol_iterator symbol_begin_impl() const LLVM_OVERRIDE;
  basic_symbol_iterator symbol_end_impl() const LLVM_OVERRIDE;
  library_iterator needed_library_begin() const LLVM_OVERRIDE;
  library_iterator needed_library_end() const LLVM_OVERRIDE;
  section_iterator section_begin() const LLVM_OVERRIDE;
  section_iterator section_end() const LLVM_OVERRIDE;
  const clrmeta_header * getMetadataHeader() const { return MetadataHeader;}
  const coff_section *getCOFFSection(section_iterator &It) const;
  const coff_symbol *getCOFFSymbol(symbol_iterator &It) const;
  const coff_relocation *getCOFFRelocation(relocation_iterator &It) const;

  uint8_t getBytesInAddress() const LLVM_OVERRIDE;
  StringRef getFileFormatName() const LLVM_OVERRIDE;
  unsigned getArch() const LLVM_OVERRIDE;
  StringRef getLoadName() const LLVM_OVERRIDE;
  
  import_directory_iterator import_directory_begin() const;
  import_directory_iterator import_directory_end() const;
  export_directory_iterator export_directory_begin() const;
  export_directory_iterator export_directory_end() const;

  error_code getHeader(const coff_file_header *&Res) const;

  error_code getCOFFHeader(const coff_file_header *&Res) const;
  error_code getPE32Header(const pe32_header *&Res) const;
  error_code getPE32PlusHeader(const pe32plus_header *&Res) const;
  error_code getDataDirectory(uint32_t index, const data_directory *&Res) const;
  error_code getSection(int32_t index, const coff_section *&Res) const;
  error_code getSymbol(uint32_t index, const coff_symbol *&Res) const;
  template <typename T>
  error_code getAuxSymbol(uint32_t index, const T *&Res) const {
    const coff_symbol *s;
    error_code ec = getSymbol(index, s);
    Res = reinterpret_cast<const T*>(s);
    return ec;
  }
  error_code getSymbolName(const coff_symbol *symbol, StringRef &Res) const;
  ArrayRef<uint8_t> getSymbolAuxData(const coff_symbol *symbol) const;

  error_code getSectionName(const coff_section *Sec, StringRef &Res) const;
  error_code getSectionContents(const coff_section *Sec,
                                ArrayRef<uint8_t> &Res) const;

  error_code getVaPtr(uint64_t VA, uintptr_t &Res) const;
  error_code getRvaPtr(uint32_t Rva, uintptr_t &Res) const;
  error_code getMethodSize(uintptr_t method, unsigned int &size) const ;
  error_code getHintName(uint32_t Rva, uint16_t &Hint, StringRef &Name) const;

  static inline bool classof(const Binary *v) {
    return v->isCOFF();
  }
};

// The iterator for the import directory table.
class ImportDirectoryEntryRef {
public:
  ImportDirectoryEntryRef() : OwningObject(0) {}
  ImportDirectoryEntryRef(const import_directory_table_entry *Table, uint32_t I,
                          const COFFObjectFile *Owner)
      : ImportTable(Table), Index(I), OwningObject(Owner) {}

  bool operator==(const ImportDirectoryEntryRef &Other) const;
  void moveNext();
  error_code getName(StringRef &Result) const;

  error_code
  getImportTableEntry(const import_directory_table_entry *&Result) const;

  error_code
  getImportLookupEntry(const import_lookup_table_entry32 *&Result) const;

private:
  const import_directory_table_entry *ImportTable;
  uint32_t Index;
  const COFFObjectFile *OwningObject;
};

// The iterator for the export directory table entry.
class ExportDirectoryEntryRef {
public:
  ExportDirectoryEntryRef() : OwningObject(0) {}
  ExportDirectoryEntryRef(const export_directory_table_entry *Table, uint32_t I,
                          const COFFObjectFile *Owner)
      : ExportTable(Table), Index(I), OwningObject(Owner) {}

  bool operator==(const ExportDirectoryEntryRef &Other) const;
  void moveNext();

  error_code getDllName(StringRef &Result) const;
  error_code getOrdinalBase(uint32_t &Result) const;
  error_code getOrdinal(uint32_t &Result) const;
  error_code getExportRVA(uint32_t &Result) const;
  error_code getSymbolName(StringRef &Result) const;

private:
  const export_directory_table_entry *ExportTable;
  uint32_t Index;
  const COFFObjectFile *OwningObject;
};
} // end namespace object
} // end namespace llvm

#endif
