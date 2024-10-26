#pragma once

#include "utl/type.h"
#include "utl/bytes.h"
#include "utl/macro.h"

typedef struct PeDosHdr PeDosHdr;
struct PeDosHdr
{
    u16 magic;
    u16 cblp;
    u16 cp;
    u16 crlc;
    u16 cparhdr;
    u16 minalloc;
    u16 maxalloc;
    u16 ss;
    u16 sp;
    u16 csum;
    u16 ip;
    u16 cs;
    u16 lfarlc;
    u16 ovno;
    u16 res[4];
    u16 oemid;
    u16 oeminfo;
    u16 res2[10];
    u32 lfanew;
};

typedef struct PeFileHdr PeFileHdr;
struct PeFileHdr
{
    u16 machine;
    u16 number_of_sections;
    u32 time_date_stamp;
    u32 pointer_to_symbol_table;
    u32 number_of_symbols;
    u16 size_of_optional_header;
    u16 characteristics;
};

typedef struct PeDataDir PeDataDir;
struct PeDataDir
{
    u32 virtual_address;
    u32 size;
};

typedef struct PeOptHdr64 PeOptHdr64;
struct PeOptHdr64
{
    u16 magic;
    u8 major_linker_version;
    u8 minor_linker_version;
    u32 size_of_code;
    u32 size_of_initialized_data;
    u32 size_of_uninitialized_data;
    u32 address_of_entry_point;
    u32 base_of_code;
    u64 image_base;
    u32 section_alignment;
    u32 file_alignment;
    u16 major_operating_system_version;
    u16 minor_operating_system_version;
    u16 major_image_version;
    u16 minor_image_version;
    u16 major_subsystem_version;
    u16 minor_subsystem_version;
    u32 win32_version_value;
    u32 size_of_image;
    u32 size_of_headers;
    u32 check_sum;
    u16 subsystem;
    u16 dll_characteristics;
    u64 size_of_stack_reserve;
    u64 size_of_stack_commit;
    u64 size_of_heap_reserve;
    u64 size_of_heap_commit;
    u32 loader_flags;
    u32 number_of_rva_and_sizes;
    PeDataDir export_directory;
    PeDataDir import_directory;
    PeDataDir resource_directory;
    PeDataDir exception_directory;
    PeDataDir certificate_directory;
    PeDataDir relocation_directory;
    PeDataDir debug_directory;
    PeDataDir architecture_directory;
    PeDataDir global_ptr_directory;
    PeDataDir tls_directory;
    PeDataDir load_config_directory;
    PeDataDir bound_import_directory;
    PeDataDir iat_directory;
    PeDataDir delay_import_directory;
    PeDataDir clr_directory;
    PeDataDir reserved_directory;
};

typedef struct PeNtHdr64 PeNtHdr64;
struct PeNtHdr64
{
    u32 signature;
    PeFileHdr file_header;
    PeOptHdr64 optional_header;
};

typedef struct PeSectionHdr PeSectionHdr;
struct PeSectionHdr
{
    u8 name[8];
    u32 virtual_size;
    u32 virtual_address;
    u32 size_of_raw_data;
    u32 pointer_to_raw_data;
    u32 pointer_to_relocations;
    u32 pointer_to_line_numbers;
    u16 number_of_relocations;
    u16 number_of_line_numbers;
    u32 characteristics;
};

typedef struct PeRuntimeFunctionEntry PeRuntimeFunctionEntry;
struct PeRuntimeFunctionEntry
{
    u32 begin;
    u32 end;
    u32 unwind_info;
};

#define PeDosHdr_Magic 0x5A4D
#define PeNtHdr64_Signature 0x00004550

// Returns a pointer to the first section header of the given PE image.
PeSectionHdr *pe64_first_section(Bytes pe);

// Returns a pointer to the section header with the given name of the given PE image.
PeSectionHdr *pe64_find_section(Bytes pe, Str name);

// Returns a pointer to the NT header of the given PE image.
PeNtHdr64 *pe64_nt_hdr(Bytes pe);

// Convert an RVA to a file offset within the given PE image.
bool pe64_rva2fo(Bytes pe, u32 rva, usize *fo);

// Convert an RVA to a pointer within the given PE image.
void *pe64_rva2ptr(Bytes pe, u32 rva);
