#include "globals.h"
#include "mapview.h"
#include <cstddef>
#if 0
// temp
HWND new_dword_446698;
extern char *__cdecl Get_TRR_Pointer(int line_num);
INT_PTR __cdecl dialog_box(HINSTANCE a1, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc)
{
    return 0;
}

char *Generic_Message;
char *Caption_Message;
int Disable_Generic_Okay_Button;

int __stdcall Generic_Window_Msg_Dlg_Proc(HWND hDlg, UINT a2, WPARAM a3, LPARAM a4);

void __cdecl Generic_Window_Message(int message_string_num, int caption_string_num)
{
    Generic_Message = Get_TRR_Pointer(message_string_num);
    Caption_Message = Get_TRR_Pointer(caption_string_num);
    Disable_Generic_Okay_Button = 0;
    dialog_box(Global_Instance_Handle, "GenericMessageDialogBox", Main_Window_Handle, Generic_Window_Msg_Dlg_Proc);
    srand(1);
    srand(GetTickCount());
}

void __cdecl Generic_Window_Message(int message_string_num, int caption_string_num, int disable_button)
{
    Generic_Message = Get_TRR_Pointer(message_string_num);
    Caption_Message = Get_TRR_Pointer(caption_string_num);
    Disable_Generic_Okay_Button = disable_button;
    dialog_box(Global_Instance_Handle, "GenericMessageDialogBox", Main_Window_Handle, Generic_Window_Msg_Dlg_Proc);
    Disable_Generic_Okay_Button = 0;
    srand(1);
    srand(GetTickCount());
}

int __stdcall Generic_Window_Msg_Dlg_Proc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    HWND button_handle;
    RECT rect;

    switch (message) {
        case WM_INITDIALOG: {
            SetWindowTextA(window_handle, Caption_Message);
            SetDlgItemTextA(window_handle, 3, Generic_Message);
            button_handle = GetDlgItem(window_handle, 1);
            if (String_TRR_Data) {
                SetWindowTextA(button_handle, Get_TRR_Pointer(334));
            } else {
                SetWindowTextA(button_handle, "Okay");
            }
            if (Disable_Generic_Okay_Button) {
                button_handle = GetDlgItem(window_handle, 1);
                DestroyWindow(button_handle);
            }
            if (new_dword_446698) {
                GetWindowRect(window_handle, &rect);
                SetWindowPos(window_handle, NULL, rect.left, rect.top + 120, rect.right, rect.bottom, 1);
                PostMessageA(window_handle, 1051, 0, 0);
            }
            return 1;
        }
        case WM_COMMAND: {
            unsigned short id = LOWORD(w_param);
            if (id == IDOK || id == IDCANCEL) {
                EndDialog(window_handle, id);
            }
            break;
        }
        case 1051: {
            if (new_dword_446698) {
                SetForegroundWindow(new_dword_446698);
            }
            break;
        }
    }

    return 0;
}

void __cdecl Standard_Window_Message(char *message, char *caption)
{
    Generic_Message = message;
    Caption_Message = caption;
    dialog_box(Global_Instance_Handle, "GenericMessageDialogBox", Main_Window_Handle, Generic_Window_Msg_Dlg_Proc);

    srand(1);
    srand(GetTickCount());
}

void __cdecl Standard_Window_Message(char *message, char *caption, int disable_button)
{
    Generic_Message = message;
    Caption_Message = caption;
    Disable_Generic_Okay_Button = disable_button;
    dialog_box(Global_Instance_Handle, "GenericMessageDialogBox", Main_Window_Handle, Generic_Window_Msg_Dlg_Proc);
    Disable_Generic_Okay_Button = 0;

    srand(1);
    srand(GetTickCount());
}

void __cdecl Real_Error_Window_Message(int message_string_num)
{
    if (String_TRR_Data) {
        Caption_Message = Get_TRR_Pointer(400);
        Generic_Message = Get_TRR_Pointer(message_string_num);
    } else {
        Caption_Message = "Error!";
        Generic_Message = "Unknown error, text not loaded yet.";
    }

    dialog_box(Global_Instance_Handle, "GenericMessageDialogBox", Main_Window_Handle, Generic_Window_Msg_Dlg_Proc);
}

void __cdecl Real_Error_Window_Message(char *message)
{
    if (String_TRR_Data) {
        Caption_Message = Get_TRR_Pointer(400);
    } else {
        Caption_Message = "Error!";
    }

    Generic_Message = message;

    dialog_box(
        Global_Instance_Handle, "GenericMessageDialogBox", Main_Window_Handle, Generic_Window_Msg_Dlg_Proc);
}
#endif