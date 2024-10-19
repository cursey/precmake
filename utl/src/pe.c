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