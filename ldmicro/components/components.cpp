#include <wincodec.h>
#include <commctrl.h>
#include <stdio.h>
#include <Windowsx.h>

#include "components.h"
#include "componentstructs.h"
#include "componentfunctions.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <setjmp.h>
#include "componentimages.h"
#include "../advanceddialog.h"

//Component Structure
/*To add am entry in Componentlist on right pane, add entry to this structure as follows:
1. Index of the Component
2. ComponentId created in componentimages.h
3. Text to be displayed
4. No. of pins*/

HWND  ComponentDialog =  NULL;
HWND* MainWindowHandle;
static BOOL  DlgDone = FALSE;
static BOOL  DlgCancel =FALSE;
static HWND  Textboxes[MAX_PIN_COUNT];
static HWND  Labels[MAX_PIN_COUNT];
int *PinIds;

void ShowNameDialog(int Index, void* PinName, void* ImageId);
void MakeControls(int Index, int Pins);

void FontNice(HWND h)
{
    SendMessage(h, WM_SETFONT, (WPARAM)MyNiceFont, TRUE);
}
void FontFixed(HWND h)
{
    SendMessage(h, WM_SETFONT, (WPARAM)MyFixedFont, TRUE);
}

/*Step : return size of memory required for the component to work.*/
size_t GetStructSize(int ComponentId)
{
    switch(ComponentId){
        case COMPONENT_SWITCH:
            return sizeof(SwitchStruct);
        break;
        case COMPONENT_RELAY:
            return sizeof(RelayStruct);
        break;
		case COMPONENT_SPDT_switch:
			return sizeof(SPDTStruct);
		break;
		case COMPONENT_DPST_switch:
			return sizeof(DPSTStruct);
		break;
		case COMPONENT_DPDT_switch:
			return sizeof(DPDTStruct);
		break;
    }
    return (size_t)-1;
}

/*Step : Initialize Respective component memory with initial parameters before drawing*/

int InitializeComponentProperties(void *ComponentAddress, int ComponentId)
{
    switch(ComponentId){
        case COMPONENT_SWITCH:
            return InitSwitch(ComponentAddress);
        break;
        case COMPONENT_RELAY:
            return InitRelay(ComponentAddress);
        break;
		case COMPONENT_SPDT_switch:
			return Init_SPDT(ComponentAddress);
		break;
		case COMPONENT_DPST_switch:
			return Init_DPST(ComponentAddress);
		break;
		/*case COMPONENT_DPDT_switch:
			return Init_DPDT(ComponentAddress);
		break;*/
    }
    return 0;
}

// Step : Main Program will Call this function if voltage change event occurs on a pin assigned to a component

double VoltSet(void* ComponentAddress, BOOL SimulationStarted, int ImageType, int Index, double Volt,
    int Source, void* ImageLocation)
{
    
    switch(ImageType)
    {
        case COMPONENT_SWITCH:
            return SwitchVoltChanged(ComponentAddress, SimulationStarted, Index, Volt, Source, ImageLocation);
        break;
        case COMPONENT_RELAY:
            return RelayVoltChanged(ComponentAddress, SimulationStarted, Index, Volt, Source, ImageLocation);
		case COMPONENT_SPDT_switch:
			return SPDTVoltChanged(ComponentAddress, SimulationStarted, Index, Volt, Source, ImageLocation);
		break;
		case COMPONENT_DPST_switch:
			return DPSTVoltChanged(ComponentAddress, SimulationStarted, Index, Volt, Source, ImageLocation);
		break;
		/*case COMPONENT_DPDT_switch:
			return DPDTVoltChanged(ComponentAddress, SimulationStarted, Index, Volt, Source, ImageLocation);
		break;*/
    }
    return Volt;
}

void  	SetPinIds(int Index, void *PinName,int ComponentId, void *ComponentAddress, int Event)
{
    if(Event == EVENT_MOUSE_RCLICK && !SimulationStarted)
    {
        ShowNameDialog(Index, PinName, ComponentAddress);
        switch(ComponentId)
        {
            case  COMPONENT_SWITCH:
                SetSwitchIds(PinIds, ComponentAddress);
            break;
			case COMPONENT_SPDT_switch:
				SetSPDTIds(PinIds, ComponentAddress);
			break;
			case COMPONENT_DPST_switch:
				SetDPSTIds(PinIds, ComponentAddress);
			break;
			/*case COMPONENT_DPDT_switch:
				SetDPDTIds(PinIds, ComponentAddress);
			break;
			*/
            case COMPONENT_RELAY:
                SetRelayIds(PinIds,ComponentAddress);
        }
    }
}

/*Step: Handle events to images displayed on the Dialog*/

int NotifyComponent(void *ComponentAddress, void *PinName, int ComponentId,
    int Event, BOOL SimulationStarted, HWND* h, int Index, UINT ImageId, void* ImageLocation)
{
    MainWindowHandle = h;
    SetPinIds(Index, PinName, ComponentId, ComponentAddress, Event);
    /*char Debug[256];
    sprintf_s(Debug, "Notify Component Address: %p\n",ComponentAddress);
    OutputDebugString(Debug);*/
    switch(ComponentId){
        case COMPONENT_SWITCH:
            HandleSwitchEvent(ComponentAddress, Event, SimulationStarted, ImageLocation, ImageId, h);
            /*char vx[10],vy[10];
            _itoa((int) ComponentAddress,vy,10);
            _itoa((int)ImageLocation,vx,10);
            MessageBox(NULL,
               (vx), (vy), MB_OK | MB_ICONWARNING);*/
            return 0;
        break;
        case COMPONENT_RELAY:
            HandleRelayEvent(ComponentAddress, Event, SimulationStarted, ImageLocation, ImageId, h);
            // return InitRelay(ComponentAddress);
        break;
		case COMPONENT_SPDT_switch :
			HandleSPDTEvent(ComponentAddress, Event, SimulationStarted, ImageLocation, ImageId, h);
		break;
		case COMPONENT_DPST_switch:
			HandleDPSTEvent(ComponentAddress, Event, SimulationStarted, ImageLocation, ImageId, h);
		break;
		/*case COMPONENT_DPDT_switch:
			HandleDPDTEvent(ComponentAddress, Event, SimulationStarted, ImageLocation, ImageId, h);
		break;*/
    }
    // return voltage
    return 0;
}


//-----------------------------------------------------------------------------
// Window procedure to handle DialogBox events
//-----------------------------------------------------------------------------
static LRESULT CALLBACK ComponentDialogProc(HWND hwnd, UINT msg, WPARAM wParam,
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
HWND* CreateDialogWindow(LPCTSTR title, int x, int y, int width, int height, int style)
{
    switch (style) {
        case STYLE_VERTICAL:
            {
            if(height < V_DIALOG_HEIGHT)
            {
                height = V_DIALOG_HEIGHT;
            }
            ComponentDialog = CreateWindowEx(0, "ComponentDialog", title, 
                WS_OVERLAPPED | WS_SYSMENU, x, y, width + V_DIALOG_WIDTH, height, NULL, NULL, 
                NULL, NULL);
            SetWindowPos(ComponentDialog, HWND_TOP, x, y, width + V_DIALOG_WIDTH, height, 0);

            OkButton = CreateWindowEx(0, WC_BUTTON, ("OK"),
                WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
                (width + 13), 10, 70, 23, ComponentDialog, NULL, NULL, NULL); 
            FontNice(OkButton);

            CancelButton = CreateWindowEx(0, WC_BUTTON, ("Cancel"),
                WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
                (width + 13), 40, 70, 23, ComponentDialog, NULL, NULL, NULL); 
            FontNice(CancelButton);

            return &ComponentDialog;
        }
        case STYLE_HORIZONTAL:
        {
            if(width < H_DIALOG_WIDTH)
            {
                width = H_DIALOG_WIDTH;
            }
            ComponentDialog = CreateWindowEx(0, "ComponentDialog", title, 
                WS_OVERLAPPED | WS_SYSMENU, x, y, width, height + H_DIALOG_HEIGHT, NULL, NULL, 
                NULL, NULL);
            SetWindowPos(ComponentDialog, HWND_TOP, x, y, width, height + H_DIALOG_HEIGHT, 0);

            OkButton = CreateWindowEx(0, WC_BUTTON, ("OK"),
                WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
                13, (height + 10), 70, 23, ComponentDialog, NULL, NULL, NULL); 
            FontNice(OkButton);

            CancelButton = CreateWindowEx(0, WC_BUTTON, ("Cancel"),
                WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
                96, (height + 10), 70, 23, ComponentDialog, NULL, NULL, NULL); 
            FontNice(CancelButton);
            return &ComponentDialog;
        }
        default:
            return NULL;
    }

}
void ShowDialogWindow()
{
    EnableWindow(*MainWindowHandle, FALSE);
    ShowWindow(ComponentDialog, SW_SHOW);
}

BOOL ProcessDialogWindow()
{
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

        if(IsDialogMessage(ComponentDialog , &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    EnableWindow(*MainWindowHandle, TRUE);
    /*if(DlgCancel == FALSE)
    {
        MessageBox(ComponentDialog,
            ("Saved"), ("Mouse click"), MB_OK | MB_ICONWARNING);
    }*/
    return DlgCancel;
}

void MakeComponentDialogBoxClass(void)
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                          CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)ComponentDialogProc;
    wc.hInstance        = *ComponentInstance;
    wc.hbrBackground    = (HBRUSH)COLOR_BTNSHADOW;
    wc.lpszClassName    = "ComponentDialog";
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon            = (HICON)LoadImage(*ComponentInstance, MAKEINTRESOURCE(4000),        //Check This Today
                            IMAGE_ICON, 32, 32, 0);
    wc.hIconSm          = (HICON)LoadImage(*ComponentInstance, MAKEINTRESOURCE(4000),
                            IMAGE_ICON, 16, 16, 0);

    RegisterClassEx(&wc);

    MyNiceFont = CreateFont(16, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        FF_DONTCARE, "Tahoma");
    if(!MyNiceFont)
        MyNiceFont = (HFONT)GetStockObject(SYSTEM_FONT);

    MyFixedFont = CreateFont(14, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        FF_DONTCARE, "Lucida Console");
    if(!MyFixedFont)
        MyFixedFont = (HFONT)GetStockObject(SYSTEM_FONT);
}

void InitComponents()
{
    MyNiceFont = CreateFont(16, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        FF_DONTCARE, "Tahoma");
    if(!MyNiceFont)
        MyNiceFont = (HFONT)GetStockObject(SYSTEM_FONT);

    MyFixedFont = CreateFont(14, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        FF_DONTCARE, "Lucida Console");
    if(!MyFixedFont)
        MyFixedFont = (HFONT)GetStockObject(SYSTEM_FONT);
    MakeComponentDialogBoxClass();
}

size_t GetNameSize(int Index)
{
    int Pins;
    Pins = rgCompData[Index].PinCount;
    return sizeof(TCHAR) * Pins * MAX_PIN_NAME;
}


void MakeControls(int Index, int Pins)
{
    int i;
    HDC hdc = GetDC(ComponentDialog);
    SelectObject(hdc, MyNiceFont);
    SIZE si;

    int maxLen = 0;
    for(i = 0; i < Pins; i++) {
        // MessageBox(ComponentDialog,
        //         rgCompData[Index]->PinNames[i], ("Mouse click"), MB_OK | MB_ICONWARNING);
        GetTextExtentPoint32(hdc, rgCompData[Index].PinNames[i],
         strlen(rgCompData[Index].PinNames[i]), &si);
        if(si.cx > maxLen) maxLen = si.cx;
    }
    int adj;
    if(maxLen > 70) {
        adj = maxLen - 70;
    } else {
        adj = 0;
    }
    for(i = 0; i < Pins; i++)
    {
        GetTextExtentPoint32(hdc, rgCompData[Index].PinNames[i],
            strlen(rgCompData[Index].PinNames[i]), &si);

        Labels[i] = CreateWindowEx(0, WC_STATIC, rgCompData[Index].PinNames[i],
                WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
                (80 + adj) - si.cx - 4, 13 + i*30, si.cx, 21,
                ComponentDialog, NULL, NULL, NULL); 
            FontNice(Labels[i]);
        SendMessage(Textboxes[i], WM_SETTEXT, 0, (LPARAM)rgCompData[Index].PinNames[i]);

        Textboxes[i] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
            WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS |
            WS_VISIBLE, 80 + adj, 12 + 30*i, 120 - adj, 21,
            ComponentDialog, NULL, NULL, NULL);
        FontFixed(Textboxes[i]);
    }
        ReleaseDC(ComponentDialog, hdc);
}
void LoadNameDialog(int Pins, void* Loc)
{
    TCHAR* values = (TCHAR*)Loc;
    int i;
    for (i = 0; i < Pins; ++i)
    {
        Edit_SetText(Textboxes[i], (LPSTR)(values + (i * MAX_PIN_NAME)));
    }
}

BOOL SaveNameDialog(int Pins, void* Loc, void* ImageId, int ImageType)
{
    TCHAR* values = (TCHAR*)Loc;
    int i;
    for (i = 0; i < Pins; ++i)
    {
        int PinId;
        char PinValues[MAX_PIN_NAME];
        if (sizeof(TCHAR) == sizeof(char))  // String is non-unicode
            strcpy(PinValues, (char*)(values + (i * MAX_PIN_NAME)));
        else                                // String is unicode
            wcstombs(PinValues, (wchar_t*)(values + (i * MAX_PIN_NAME)), MAX_PIN_NAME);
        Edit_GetText(Textboxes[i], (LPSTR)(values + (i * MAX_PIN_NAME)), MAX_PIN_NAME);
        char NewValue[MAX_PIN_NAME];
        if (sizeof(TCHAR) == sizeof(char))  // String is non-unicode
            strcpy(NewValue, (char*)(values + (i * MAX_PIN_NAME)));
        else                                // String is unicode
            wcstombs(NewValue, (wchar_t*)(values + (i * MAX_PIN_NAME)), MAX_PIN_NAME);
        /*char Debug[256];
        sprintf_s(Debug, "SaveNameDialog: %s %s\n",PinValues, NewValue);
        OutputDebugString(Debug);*/
        if (lstrcmp(PinValues, NewValue) != 0)
        {
            /*sprintf_s(Debug, "Deleting: %s %s\n",PinValues, NewValue);
            OutputDebugString(Debug);*/
            PinId = DeletePinImage((LPCTSTR)PinValues, ImageId, i);
            // DeleteComponentPin(PinId, ImageId);
        }
        PinId = RegisterPinName((LPCTSTR)NewValue);
        if(PinId)
        {
            PinIds[i] = PinId;
        }
        SetPinImage(PinId, ImageId, ImageType, i);
        // if(PinId)
        // {
        //     AddComponentPin(PinId, ImageId);
        // }
    }
    return TRUE;
}
void ShowNameDialog(int Index, void* PinName, void* ImageId)
{
    HWND *NameDialog;
    BOOL Canceled, Complete = FALSE;
    int w, h, Pins;
    Pins = rgCompData[Index].PinCount;
    // char vx[3],vy[3];
    //             _itoa(Pins,vy,10);
                // _itoa(x,vx,10);
                // MessageBox(AdvancedDialog, _(vx), _(vy), MB_OK | MB_ICONWARNING);
    // MessageBox(ComponentDialog,
    //             (vy), ("Mouse click"), MB_OK | MB_ICONWARNING);
    // if(Pins >MAX_PIN_COUNT) oops();
    w = 304 - V_DIALOG_WIDTH;
    h = 15 + 30 * (Pins + 1);
    PinIds = (int*)AllocImageHeap(sizeof(int)*Pins);
    NameDialog = CreateDialogWindow("Pin Naming", 100, 100, w, h, STYLE_VERTICAL);
    MakeControls(Index, Pins);
    LoadNameDialog(Pins, PinName);
    ShowDialogWindow();
    SetFocus(Textboxes[0]);
    SendMessage(Textboxes[0], EM_SETSEL, 0, -1);
    Canceled = ProcessDialogWindow();
    while(Canceled == FALSE)
    {
        Complete = SaveNameDialog(Pins, PinName, ImageId, rgCompData[Index].ComponentId);
        if(Complete == TRUE)
        {
            // MessageBox(*SwitchDialog,
            //     ("Saved"), ("Mouse click"), MB_OK | MB_ICONWARNING);
            break;
        }else
        {
            Complete =TRUE;
            Canceled = ProcessDialogWindow();
        }
    }
    DestroyWindow(*NameDialog);
}

double VoltRequest(int PinId, void* ComponentAddress)
{
    return GetGlobalVoltage(PinId,ComponentAddress);
}

// double GlobalVoltChange(int PinId, void *ComponentAddress, double volt)
// {
//     return RequestVoltChange(PinId,ComponentAddress,volt);
// }

double VoltChange(int PinId, int Index, void* ComponentAddress, double Volt)
{
    return RequestVoltChange(PinId, Index, ComponentAddress, Volt);
}