/**
 * @file
 *
 * @author CCHyper
 * @author jonwil
 * @author LRFLEW
 * @author OmniBlade
 * @author Saberhawk
 *
 * @brief Hooking system to alter memory protection on binary in memory.
 *
 * @copyright JustBusiness is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            3 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "hooker.h"
#include "mapview.h"

#include <cstdio>

DWORD s_oldProtect1 = 0;
DWORD s_oldProtect2 = 0;

bool StartHooks()
{
    bool success = false;
    ImageSectionInfo info;
    DWORD maxPath = MAX_PATH;
    char app_path[MAX_PATH];

    char aaaaaaaaaaa[245];

    MEMORY_BASIC_INFORMATION mbi;

    if (GetModuleSectionInfo(info)) {
        success = true;
        HANDLE process = GetCurrentProcess();
        QueryFullProcessImageNameA(process, 0, app_path, &maxPath);
        MessageBoxA(nullptr, app_path, "Proxy", MB_ICONINFORMATION);

        char buf[8];
        SIZE_T read;
        if (ReadProcessMemory(process, info.BaseOfCode, buf, 8, &read) == FALSE) {
            success = false;
            MessageBoxA(nullptr, "ReadProcessMemory test failed", "Proxy", MB_ICONINFORMATION);
        } else {
            char re[32];
            sprintf(re,
                "%x %x %x %x %x %x %x %x",
                buf[0] & 0xFF,
                buf[1] & 0xFF,
                buf[2] & 0xFF,
                buf[3] & 0xFF,
                buf[4] & 0xFF,
                buf[5] & 0xFF,
                buf[6] & 0xFF,
                buf[7] & 0xFF);
            MessageBoxA(nullptr, re, "Proxy", MB_ICONINFORMATION);
        }

        if (VirtualProtectEx(process, info.BaseOfCode, info.SizeOfCode, PAGE_EXECUTE_READWRITE, &s_oldProtect1)) {
            //success = false;
            sprintf(aaaaaaaaaaa,
                "\n 1 VirtualProtectEx() error - %d\ninfo.BaseOfCode %x\ninfo.SizeOfCode %x\ns_oldProtect1 %x\n",
                GetLastError(),
                info.BaseOfCode,
                info.SizeOfCode,
                s_oldProtect1);
            MessageBoxA(nullptr, aaaaaaaaaaa, "Proxy", MB_ICONINFORMATION);
            VirtualProtectEx(process, info.BaseOfCode, info.SizeOfCode, s_oldProtect1, &s_oldProtect1);
        }
        //if (VirtualProtectEx(process, info.BaseOfData, info.SizeOfData, PAGE_EXECUTE_READWRITE, &s_oldProtect2) == FALSE) {
        //    success = false;
        //    sprintf(aaaaaaaaaaa, "\n 2 VirtualProtectEx() error - %d\n", GetLastError());
        //
        //    MessageBoxA(nullptr, aaaaaaaaaaa, "Proxy", MB_ICONINFORMATION);
        //}
    } else {
        MessageBoxA(nullptr, "GetModuleSectionInfo failed", "Proxy", MB_ICONINFORMATION);
    }

    return success;
}

bool StopHooks()
{
    bool success = false;
    DWORD old_protect;
    ImageSectionInfo info;

    if (GetModuleSectionInfo(info)) {
        success = true;
        HANDLE process = GetCurrentProcess();
        if (VirtualProtectEx(process, info.BaseOfCode, info.SizeOfCode, s_oldProtect1, &old_protect) == FALSE)
            success = false;
        if (VirtualProtectEx(process, info.BaseOfData, info.SizeOfData, s_oldProtect2, &old_protect) == FALSE)
            success = false;
    }

    return success;
}
