#include "../include/lab_macroses.hpp"

#include <windows.h>
#include <cstdio>

void ErrorOutputWindows(const char *chFuncFailMessage, int iErr, int iFlag)
{
    DWORD n = 0; // Код ошибки
    const DWORD size = LAB_MAX_BUF_SIZE; // Максимальный размер сообщения об ошибке
    char buf[LAB_MAX_BUF_SIZE]; // Буфер сообщения

    switch ( iFlag ) // Обработка флага
    {
        case LAB_FLAG_WSA:
            n = WSAGetLastError(); // Получение ошибки и получение сообщения о нем
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
                            NULL,
                            n,
                            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                            buf,
                            size - 1,
                            NULL
                            );
        break;
        
        case LAB_FLAG_GET:
            n = GetLastError();
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
                            NULL,
                            n,
                            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                            buf,
                            size - 1,
                            NULL
                            );
        break;

        case LAB_FLAG_ERRNO:
            n = errno;
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
                            NULL,
                            n,
                            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                            buf,
                            size - 1,
                            NULL
                            );
        break;

        case LAB_FLAG_RESULT:
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
                            NULL,
                            iErr,
                            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                            buf,
                            size - 1,
                            NULL
                            );
            break;

        default :
            sprintf(buf, "Error in programmer's function code is %d", iErr);
        break;
    }

    // Вывод сообщения об ошибке
    MessageBoxA(0, buf, chFuncFailMessage, MB_ICONERROR | MB_OK);
}