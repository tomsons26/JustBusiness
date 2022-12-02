/**
 * @file
 *
 * @author xezon
 *
 * @brief Map view helper to extract information from DOS executable.
 *
 * @copyright JustBusiness is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            3 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "mapview.h"

#include <cstdio>
#include <stdint.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif // !ARRAY_SIZE

MapViewOfFileClass::MapViewOfFileClass(const wchar_t *fileName) :
    m_hFile(INVALID_HANDLE_VALUE),
    m_hFileMapping(NULL),
    m_lpFileBase(NULL),
    m_dosHeader(NULL),
    m_ntHeader(NULL),
    m_optionalHeader(NULL),
    m_sectionHeaders(NULL)
{
    m_hFile = CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (m_hFile != INVALID_HANDLE_VALUE) {
        m_hFileMapping = CreateFileMapping(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);

        if (m_hFileMapping != NULL) {
            m_lpFileBase = MapViewOfFile(m_hFileMapping, FILE_MAP_READ, 0, 0, 0);

            if (m_lpFileBase != NULL) {
                m_dosHeader = (PIMAGE_DOS_HEADER)m_lpFileBase;

                if (m_dosHeader->e_magic == IMAGE_DOS_SIGNATURE) {
                    m_ntHeader = (PIMAGE_NT_HEADERS)((uint8_t *)m_dosHeader + m_dosHeader->e_lfanew);

                    if (m_ntHeader->Signature == IMAGE_NT_SIGNATURE) {
                        m_optionalHeader = (PIMAGE_OPTIONAL_HEADER)&m_ntHeader->OptionalHeader;
                        m_sectionHeaders = IMAGE_FIRST_SECTION(m_ntHeader);
                    } else {
                        MessageBoxA(nullptr, "Signature failed", "Proxy", MB_ICONINFORMATION);
                    }
                } else {
                    MessageBoxA(nullptr, "e_magic failed", "Proxy", MB_ICONINFORMATION);
                }
            } else {
                MessageBoxA(nullptr, "m_lpFileBase failed", "Proxy", MB_ICONINFORMATION);
            }
        } else {
            MessageBoxA(nullptr, "m_hFileMapping failed", "Proxy", MB_ICONINFORMATION);
        }
    } else {
        MessageBoxA(nullptr, "m_hFile failed", "Proxy", MB_ICONINFORMATION);
    }
}

MapViewOfFileClass::~MapViewOfFileClass()
{
    if (m_lpFileBase != NULL)
        UnmapViewOfFile(m_lpFileBase);
    if (m_hFileMapping != NULL)
        CloseHandle(m_hFileMapping);
    if (m_hFile != INVALID_HANDLE_VALUE)
        CloseHandle(m_hFile);
}

bool GetModuleSectionInfo(ImageSectionInfo &info)
{
    wchar_t fileName[MAX_PATH] = {0};
    char aaa[867];
    if (GetModuleFileNameW(NULL, fileName, ARRAY_SIZE(fileName)) != 0) {
        MapViewOfFileClass mapView(fileName);
        PIMAGE_OPTIONAL_HEADER optionalHeader = mapView.GetOptionalHeader();

        if (optionalHeader != NULL) {
            info.BaseOfCode = LPVOID(optionalHeader->ImageBase + optionalHeader->BaseOfCode);
            info.BaseOfData = LPVOID(optionalHeader->BaseOfData);
            info.SizeOfCode = SIZE_T(optionalHeader->SizeOfCode);
            info.SizeOfData = SIZE_T(optionalHeader->SizeOfInitializedData + optionalHeader->SizeOfUninitializedData);
            sprintf(aaa,
                "ImageBase %x\n"
                "SectionAlignment %x\n"
                "FileAlignment %x\n"
                "MajorOperatingSystemVersion %x\n"
                "MinorOperatingSystemVersion %x\n"
                "MajorImageVersion %x\n"
                "MinorImageVersion %x\n"
                "MajorSubsystemVersion %x\n"
                "MinorSubsystemVersion %x\n"
                "Win32VersionValue %x\n"
                "SizeOfImage %x\n"
                "SizeOfHeaders %x\n"
                "CheckSum %x\n"
                "Subsystem %x\n"
                "DllCharacteristics %x\n"
                "SizeOfStackReserve %x\n"
                "SizeOfStackCommit %x\n"
                "SizeOfHeapReserve %x\n"
                "SizeOfHeapCommit %x\n"
                "LoaderFlags %x\n"
                "NumberOfRvaAndSizes %x\n",
                optionalHeader->ImageBase,
                optionalHeader->SectionAlignment,
                optionalHeader->FileAlignment,
                optionalHeader->MajorOperatingSystemVersion,
                optionalHeader->MinorOperatingSystemVersion,
                optionalHeader->MajorImageVersion,
                optionalHeader->MinorImageVersion,
                optionalHeader->MajorSubsystemVersion,
                optionalHeader->MinorSubsystemVersion,
                optionalHeader->Win32VersionValue,
                optionalHeader->SizeOfImage,
                optionalHeader->SizeOfHeaders,
                optionalHeader->CheckSum,
                optionalHeader->Subsystem,
                optionalHeader->DllCharacteristics,
                optionalHeader->SizeOfStackReserve,
                optionalHeader->SizeOfStackCommit,
                optionalHeader->SizeOfHeapReserve,
                optionalHeader->SizeOfHeapCommit,
                optionalHeader->LoaderFlags,
                optionalHeader->NumberOfRvaAndSizes);
            MessageBoxA(nullptr, aaa, "Proxy", MB_ICONINFORMATION);
            return true;
        } else {
            MessageBoxA(nullptr, "optionalHeader failed", "Proxy", MB_ICONINFORMATION);
        }
    } else {
        MessageBoxA(nullptr, "GetModuleFileNameW failed", "Proxy", MB_ICONINFORMATION);
    }
    return false;
}
