#include "utl/thread.h"
#include "utl/pe.h"

PeSectionHdr *pe64_first_section(Bytes pe)
{
    PeNtHdr64 *nt_hdr = pe64_nt_hdr(pe);
    if (nt_hdr == 0)
    {
        return 0;
    }
    return (PeSectionHdr *)((usize)nt_hdr + offset_of(PeNtHdr64, optional_header) +
                            nt_hdr->file_header.size_of_optional_header);
}

PeSectionHdr *pe64_find_section(Bytes pe, Str name)
{
    Temp scratch = scratch_begin(0, 0);
    PeSectionHdr *section = pe64_first_section(pe);
    PeSectionHdr *result = 0;
    u8 *ext_name_buf = push_array(scratch.arena, u8, 8);
    mem_copy(ext_name_buf, name.ptr, n_min(name.len, 8));
    name.ptr = ext_name_buf;
    name.len = 8;
    for (u16 i = 0; i < pe64_nt_hdr(pe)->file_header.number_of_sections; ++i, ++section)
    {
        if (str_eq(name, make_str(section->name, 8)))
        {
            result = section;
            break;
        }
    }
    scratch_end(scratch);
    return result;
}

PeNtHdr64 *pe64_nt_hdr(Bytes pe)
{
    if (pe.len < sizeof(PeDosHdr))
    {
        return 0;
    }
    PeDosHdr *dos_hdr = (PeDosHdr *)pe.ptr;
    if (dos_hdr->magic != PeDosHdr_Magic)
    {
        return 0;
    }
    if (pe.len < dos_hdr->lfanew + sizeof(PeNtHdr64))
    {
        return 0;
    }
    PeNtHdr64 *nt_hdr = (PeNtHdr64 *)(pe.ptr + dos_hdr->lfanew);
    if (nt_hdr->signature != PeNtHdr64_Signature)
    {
        return 0;
    }
    return nt_hdr;
}

bool pe64_rva2fo(Bytes pe, u32 rva, usize *fo)
{
    PeNtHdr64 *nt_hdr = pe64_nt_hdr(pe);
    if (nt_hdr == 0)
    {
        return false;
    }
    PeSectionHdr *section = pe64_first_section(pe);
    for (u16 i = 0; i < nt_hdr->file_header.number_of_sections; ++i, ++section)
    {
        u32 size = section->virtual_size;
        if (size == 0)
        {
            size = section->size_of_raw_data;
        }
        if (rva >= section->virtual_address && rva < section->virtual_address + size)
        {
            *fo = section->pointer_to_raw_data + rva - section->virtual_address;
            return true;
        }
    }
    return false;
}

void *pe64_rva2ptr(Bytes pe, u32 rva)
{
    usize fo;
    if (!pe64_rva2fo(pe, rva, &fo))
    {
        return 0;
    }
    return pe.ptr + fo;
}