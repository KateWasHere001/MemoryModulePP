#include "../MemoryModule/stdafx.h"
#include <cstdio>
#pragma warning(disable:4996)

static PVOID ReadDllFile(LPCSTR FileName) {
    LPVOID buffer;
    size_t size;
    FILE* f = fopen(FileName, "rb");
    if (!f)return 0;
    _fseeki64(f, 0, SEEK_END);
    if (!(size = _ftelli64(f))) {
        fclose(f);
        return 0;
    }
    _fseeki64(f, 0, SEEK_SET);
    fread(buffer = new char[size], 1, size, f);
    fclose(f);
    return buffer;
}

int test() {
    HMODULE hModule;
    NTSTATUS status;
    PVOID buffer = ReadDllFile("a.dll");
    if (!buffer) return 0;

    status = LdrLoadDllMemoryExW(
        &hModule,                               // ModuleHandle
        nullptr,                                // LdrEntry
        0,                                      // Flags
        buffer,                                 // Buffer
        0,                                      // Reserved
        L"a.dll",                               // DllBaseName
        L"C:\\Windows\\System32\\a.dll"         // DllFullName
    );
    if (NT_SUCCESS(status) && status != STATUS_IMAGE_MACHINE_TYPE_MISMATCH) {

        typedef int(__stdcall* func)();
        func test_user32 = (func)GetProcAddress(hModule, "test_user32");
        test_user32();

        //
        // After calling MessageBox, we can't free it.
        // 
        //LdrUnloadDllMemory(hModule);
    }

    return 0;
}

int main() {
    test();
    return 0;
}
