#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <wincodec.h>
#include <Windowsx.h>

#include "ldmicro.h"
#include "naminglist.h"
#include "components\components.h"
#include "advanceddialog.h"

LVITEM lvi;

void ShowNameListDialog();
double SaveNameListDialog(int PinId, int Index);
static BOOL DlgDone, DlgCancel;
static LRESULT CALLBACK SmplDlgProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam);

int NameList_AddName(LPCTSTR Name, int PinId, int Index)
{
    lvi.mask        = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
    lvi.state       = lvi.stateMask = 0;
    lvi.iItem       = Index;
    lvi.iSubItem    = 0;
    lvi.pszText     = (LPSTR)Name;
    lvi.lParam      = PinId;
    return ListView_InsertItem(NamingList, &lvi);
}

int NameList_AddVolt(LPCTSTR Name, int Index)
{
    ListView_SetItemText(NamingList, Index, 1 , (LPSTR)Name);
    return 0;
}

void PopulateNamingList()
{
    int ItmCount = Prog.io.count;
    int i;
    // ListView_DeleteAllItems(NamingList);
    for(i=0; i<ItmCount; i++)
    {
        switch(Prog.io.assignment[i].type){

            case IO_TYPE_DIG_INPUT:
            case IO_TYPE_DIG_OUTPUT:
            case IO_TYPE_READ_ADC:
            case IO_TYPE_UART_TX:
            case IO_TYPE_UART_RX:
            case IO_TYPE_PWM_OUTPUT:
            {
                LPCTSTR Name = (LPCTSTR)Prog.io.assignment[i].name;
                // MessageBox(NULL,
                //     Name, "Name", MB_OK | MB_ICONWARNING);
                SetMcu(RegisterPinName(Name), Prog.io.assignment[i].type);
                LVFINDINFO temp;
                temp.psz = Name;
                temp.flags = LVFI_STRING;
                int index = ListView_FindItem(NamingList, -1, &temp);
                /*char vx[3],vy[3];
    _itoa(10,vy,10);
    _itoa(index,vx,10);
    MessageBox(NULL,
        _(vx), _(vy), MB_OK | MB_ICONWARNING);*/
                break;
            }
            default:
                break;
        }
    }
    /*char vx[3],vy[3];
    _itoa(NameCount,vy,10);
    _itoa(ItmCount,vx,10);
    MessageBox(NULL,
        _(vx), _(vy), MB_OK | MB_ICONWARNING);*/
}
void RefreshNamingList()
{
    UINT i=0;
    ListView_DeleteAllItems(NamingList);
    for(i=0; i<NameCount; i++)
    {
        /*char vx[3];
        _itoa(i,vx,10);
        MessageBox(NULL, NameId[i].Name, (LPCTSTR)vx, MB_OK |MB_ICONWARNING);*/
        int NameIndex = NameList_AddName((LPCTSTR)NameId[i].Name, NameId[i].PinId, i);
        int index = IsMCUPin(NameId[i].PinId);
        if(index >=0)
        {
            if(McuPin[index].InternalPullup)
            {
                NameList_AddVolt((LPCTSTR)"Pullup", NameIndex);
                NameId[i].PinData.Volt = VOLT_5;
            }
            else
            {
                NameList_AddVolt((LPCTSTR)"V_OPEN", NameIndex);
                NameId[i].PinData.Volt = V_OPEN;
            }
            // MessageBox(NULL, (LPCTSTR)NameId[i].Name,"Test", MB_OK | MB_ICONWARNING);
        }
        else
        {
            char Voltage[15];
            sprintf((char*)Voltage,"%f", NameId[i].PinData.Volt);
            NameList_AddVolt(LPCTSTR(Voltage), NameIndex);
            /*char Debug[256];
            sprintf_s(Debug, "Nameid[%d].PinData.Volt = %f \t Index= %d\n",
        i,NameId[i].PinData.Volt, NameIndex);
    OutputDebugString(Debug);*/
        }
        NameId[i].PinData.ProgVolt = NameId[i].PinData.Volt;
    }
}

void NamingListProc(NMHDR *h)
{
    switch(h->code) {
        case LVN_ITEMACTIVATE:
        {
            if(!SimulationStarted)
            {
                int PinId;
                // NMITEMACTIVATE *i = (NMITEMACTIVATE *)h;
                LVITEM lvi;
                lvi.mask = LVIF_PARAM;
                lvi.iItem = ListView_GetNextItem(NamingList, -1,  LVNI_SELECTED); 
                ListView_GetItem(NamingList, &lvi);
                PinId = (int) lvi.lParam;
                int index = IsMCUPin(PinId);
                if(index >= 0)
                {
                    ToggleInternalPullup(PinId);
                    McuPin[index].InternalPullup ?
                        NameList_AddVolt((LPCTSTR)"Pullup", lvi.iItem):
                        NameList_AddVolt((LPCTSTR)"V_OPEN", lvi.iItem);
                    // MessageBox(NULL, "Pullup","NamingListProc" , MB_OK | MB_ICONWARNING):
                    // MessageBox(NULL, "V_OPEN", "NamingListProc", MB_OK | MB_ICONWARNING);
                    /*char vx[3],vy[3];
                _itoa(PinId,vy,10);
                _itoa(index,vx,10);
                MessageBox(NULL,
                    "Selected", _(vx), MB_OK | MB_ICONWARNING);*/
                }
                else
                    SaveNameListDialog(PinId,lvi.iItem);
                // Set voltage for PinId if it is not mcupin
                /*char vx[3],vy[3];
                _itoa(10,vy,10);
                _itoa(PinId,vx,10);
                MessageBox(NULL,
                    "Selected", _(vx), MB_OK | MB_ICONWARNING);*/
            }
            break;
        }
        default:
        break;
    }
}

void MakeSmplDialogClass(void)
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                          CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)SmplDlgProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)COLOR_BTNSHADOW;
    wc.lpszClassName    = "SmplDialog";
    wc.lpszMenuName     = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_SIZEWE);
    wc.hIcon            = (HICON)LoadImage(Instance, MAKEINTRESOURCE(4000),
                            IMAGE_ICON, 32, 32, 0);
    wc.hIconSm          = (HICON)LoadImage(Instance, MAKEINTRESOURCE(4000),
                            IMAGE_ICON, 16, 16, 0);

    RegisterClassEx(&wc);

    AdvNiceFont = CreateFont(16, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        FF_DONTCARE, "Tahoma");
    if(!AdvNiceFont)
        AdvNiceFont = (HFONT)GetStockObject(SYSTEM_FONT);

    AdvFixedFont = CreateFont(14, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        FF_DONTCARE, "Lucida Console");
    if(!AdvFixedFont)
        AdvFixedFont = (HFONT)GetStockObject(SYSTEM_FONT);
}

static LRESULT CALLBACK SmplDlgProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    switch (msg) {
        case WM_NOTIFY:
            break;

        case WM_COMMAND: {
            HWND h = (HWND)lParam;
            if(h == OkButton && wParam == BN_CLICKED) {
                DlgDone = TRUE;
            } else if(h == CancelButton && wParam == BN_CLICKED) {
                DlgDone = TRUE;
                DlgCancel = TRUE;
            }
            break;
        }

        case WM_CLOSE:
        case WM_DESTROY:
            DlgDone = TRUE;
            DlgCancel = TRUE;
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 1;
}

double SaveNameListDialog(int PinId, int Index)
{
    double volt;
    for(UINT i=0;i<NameCount;i++)
    {
        if(NameId[i].PinId == PinId)
        {
            volt = NameId[i].PinData.Volt;
            TCHAR Voltage[15];
            if(volt == (double)GND)
                strcpy((char*)Voltage, "GND");
            else if(volt == (double)VOLT_5)
                strcpy((char*)Voltage, "VOLT_5");
            else if(volt == (double)V_OPEN)
                strcpy((char*)Voltage, "V_OPEN");
            else
                sprintf((char*)Voltage,"%f",volt);
            HWND NameVoltDialog = CreateWindowClient(0, "SmplDialog",
                NameId[i].Name, WS_OVERLAPPED | WS_SYSMENU | WS_POPUP | WS_CAPTION,
                100, 100, 304, 15 + 30*2, NULL, NULL, Instance, NULL);

            HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Volt:"),
                WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
                10, 20, 50, 21, NameVoltDialog, NULL, Instance, NULL);
            NiceFont(textLabel);

            HWND ValueTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
                WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
                65, 20, 115, 21, NameVoltDialog, NULL, Instance, NULL);
            FixedFont(ValueTextbox);

            OkButton = CreateWindowEx(0, WC_BUTTON, ("OK"),
                WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
                220, 10, 70, 23, NameVoltDialog, NULL, NULL, NULL); 
            NiceFont(OkButton);

            CancelButton = CreateWindowEx(0, WC_BUTTON, ("Cancel"),
                WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
                220, 40, 70, 23, NameVoltDialog, NULL, NULL, NULL); 
            FixedFont(CancelButton);

            Edit_SetText(ValueTextbox,Voltage);
            EnableWindow(AdvancedDialog, FALSE);
            EnableWindow(MainWindow, FALSE);
            ShowWindow(NameVoltDialog, TRUE);
            SetFocus(ValueTextbox);
            SendMessage(ValueTextbox, EM_SETSEL, 0, -1);

            MSG msg;
            DWORD ret;
            DlgDone = FALSE;
            DlgCancel = FALSE;
            while((ret = GetMessage(&msg, NULL, 0, 0)) && !DlgDone) {
                if(msg.message == WM_KEYDOWN) {
                    if(msg.wParam == VK_RETURN) {
                        DlgDone = TRUE;
                        break;
                    } else if(msg.wParam == VK_ESCAPE) {
                        DlgDone = TRUE;
                        DlgCancel = TRUE;
                        break;
                    }
                }

                if(IsDialogMessage(NameVoltDialog, &msg)) continue;
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            if(!DlgCancel) {
                // Set Voltage here
                Edit_GetText(ValueTextbox, Voltage, 15);
                volt = atof(Voltage);
                if(strcmp((char*)Voltage, "GND") == 0)
                {
                    volt = GND;
                }
                else if(strcmp((char*)Voltage, "VOLT_5") == 0)
                {
                    volt = 5;
                }
                else if(strcmp((char*)Voltage, "V_OPEN") == 0)
                {
                    volt = V_OPEN;
                }
                else if(volt == 0.0f)
                {
                    EnableWindow(AdvancedDialog, TRUE);
                    EnableWindow(MainWindow, TRUE);
                    DestroyWindow(NameVoltDialog);
                    volt = V_OPEN;
                }
                NameList_AddVolt((LPCTSTR)Voltage, Index);
                NameList_SetVolt(i, volt);
            }
            NameId[i].PinData.Volt = volt;
            NameId[i].PinData.ProgVolt = volt;
            //CreateVoltRequest(i,volt);
            EnableWindow(AdvancedDialog, TRUE);
            EnableWindow(MainWindow, TRUE);
            DestroyWindow(NameVoltDialog);
        }
    }
    return volt;
}

double NameList_SetVolt(int Index, double volt)
{
    NameId[Index].PinData.Volt = volt;
    return volt;
}
