#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <vector>
#include <iostream>

#include <string>

#include <algorithm>
#include <iterator>

template <class InIter1, class InIter2, class OutIter>
void find_all(unsigned char* base, InIter1 buf_start, InIter1 buf_end, InIter2 pat_start, InIter2 pat_end, OutIter res) {
    for (InIter1 pos = buf_start;
        buf_end != (pos = std::search(pos, buf_end, pat_start, pat_end));
        ++pos)
    {
        *res++ = base + (pos - buf_start);
    }
}


int main(void)
{
    std::string pattern = "hello";
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return 1;
    }
    cProcesses = cbNeeded / sizeof(DWORD);//count processes...
    for (i = 0; i < cProcesses; i++)
    {
        if (aProcesses[i] != 0)
        {
            TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");



            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                PROCESS_VM_READ,
                FALSE, aProcesses[i]);

            if (NULL != hProcess)
            {
                HMODULE hMod;
                DWORD cbNeeded;

                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
                    &cbNeeded))
                {
                    GetModuleBaseName(hProcess, hMod, szProcessName,
                        sizeof(szProcessName) / sizeof(TCHAR));
                }
            }



            _tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, aProcesses[i]);
            LPCVOID AddrPointer = new LPCVOID;
            MEMORY_BASIC_INFORMATION BasicMemptr;
            unsigned char* p = NULL;

            for (p = NULL;
                VirtualQueryEx(hProcess, AddrPointer, &BasicMemptr, sizeof(BasicMemptr)) == sizeof(BasicMemptr);
                p += BasicMemptr.RegionSize) {
                std::vector<char> buffer;
                if (BasicMemptr.State == MEM_COMMIT &&
                    (BasicMemptr.Type == MEM_MAPPED || BasicMemptr.Type == MEM_PRIVATE)) {
                    SIZE_T bytes_read;
                    buffer.resize(BasicMemptr.RegionSize);
                    ReadProcessMemory(hProcess, p, &buffer[0], BasicMemptr.RegionSize, &bytes_read);
                    buffer.resize(bytes_read);
                    find_all(p, buffer.begin(), buffer.end(), pattern.begin(), pattern.end(), std::ostream_iterator<void*>(std::cout, "\n"));
                }
            }



            CloseHandle(hProcess);
        }
    }

    return 0;
}

//int main() {
//	_MEMORY_BASIC_INFORMATION *info =new _MEMORY_BASIC_INFORMATION;
//	_SYSTEM_INFO* pageInfo = new _SYSTEM_INFO;
//	GetSystemInfo(pageInfo);
//	
//	VirtualQuery(pageInfo->lpMinimumApplicationAddress, info, 200);
//
//
//
//	
//	return 0;
//
//}
