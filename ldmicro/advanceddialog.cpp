#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <wincodec.h>
#include <Windowsx.h>
#include <math.h>
// #include "stdafx.h"
// #include <gdiplus.h>

#include "ldmicro.h"
#include "componentlist.h"
#include "advanceddialog.h"
#include "components\components.h"
#include "naminglist.h"
#include "simulate.h"


static HWND     HorizScrollBar;
static HWND     VertScrollBar;
static HHOOK    MouseHookHandle;
static HMENU    AdvancedTopMenu;
static int      MouseX;
static int      MouseY;
static BOOL     Clicked = FALSE;
static BOOL     DblClicked = FALSE;
static BOOL     AdvDialogDone;
static BOOL     AdvDialogCancel;
static HMENU    AdvancedFileMenu;
static HMENU    AdvancedEditMenu;
static HMENU    AdvancedSimulateMenu;
static HMENU    AdvancedHelpMenu;
static int      CyclesPerTimerTick;


HWND        AdvancedDialog;
HWND        AdvancedWorkspace;
HWND        ComponentList;
HWND        NamingList;
HDC         hdc;
HDC         HdcMem;
HBITMAP     test = NULL;
HANDLE      ImageHeap;
BOOL        SimulationStarted = FALSE;
HFONT       AdvNiceFont;
HFONT       AdvFixedFont;
HINSTANCE   *ComponentInstance = &Instance;
char        MainProgramComponent;

int         AdvWrkspRight;
int         AdvWrkspBottom;
int         CompListWidth;
int         ResizeSwitch;
int         NamingListHeight  =  100;
UINT        TIMERID = 100;
UINT        DblClickTimer;
UINT        UniquePinId = 0;
UINT        MCUCount=0;
UINT        NameCount;
BOOL        AdvancedWindowOpen = FALSE;
// HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);

void RefreshDialogScrollbars(void);
void StartAdvSimulationTimer(void);
void CALLBACK CycleAdvTimer(HWND hwnd, UINT msg, UINT_PTR id, DWORD time);

// PinInfo PinData[MAX_PINS];
PinMcu          McuPin[MAX_MCU_PINS];
PinName         NameId[MAX_PINS];
PinComponent    ComponentPin;

//-----------------------------------------------------------------------------
// Hook that we install when the user starts dragging the `splitter,' in case
// they drag it out of the narrow area of the drawn splitter bar. Resize
// the listview in response to mouse move, and unhook ourselves when they
// release the mouse button.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK MouseHook(int code, WPARAM wParam, LPARAM lParam)
{
    switch(code) {
        case HC_ACTION: {
            MSLLHOOKSTRUCT *mhs = (MSLLHOOKSTRUCT *)lParam;

            switch(wParam) {
                case WM_MOUSEMOVE: {
              /*      char vx[3],vy[3];
            _itoa(10,vy,10);
            _itoa(ResizeSwitch,vx,10);
            MessageBox(AdvancedDialog,
               _(vx), _(vy), MB_OK | MB_ICONWARNING);*/
                    switch(ResizeSwitch)
                    {
                    case 1:
                    {
                        int dx = MouseX - mhs->pt.x;
                        CompListWidth += dx;
                        if(CompListWidth < 50) CompListWidth = 50;
                        MouseX = mhs->pt.x;
                        AdvancedDialogResized();
                    break;
                    }
                    case 2:
                    {
                        int dy = MouseY - mhs->pt.y;
                        NamingListHeight += dy;
                        if(NamingListHeight < 50) NamingListHeight = 50;
                        MouseY = mhs->pt.y;
                        AdvancedDialogResized();
                    }
                    break;
                    }
                    break;
                }

                case WM_LBUTTONUP:
                    ResizeSwitch = 0;
                    UnhookWindowsHookEx(MouseHookHandle);
                    break;
            }
            break;
        }
    }
    return CallNextHookEx(MouseHookHandle, code, wParam, lParam);
}

static void ProcessAdvancedMenu(int code)
{
    switch(code) {
        case MNU_ADV_SIMULATION_MODE:
            ToggleSimulationMode();
            ToggleAdvancedSimulationMode();
            break;

        case MNU_ADV_START_SIMULATION:
            StartSimulation();
            StartAdvSimulation();
            break;

        case MNU_ADV_STOP_SIMULATION:
            StopSimulation();
            StopAdvSimulation();
            break;

        case MNU_ADV_SINGLE_CYCLE:
            SimulateOneCycle(TRUE);
            SimulateOneAdvCycle(TRUE);
            break;
        default:
        return;
    }
}

void ToggleAdvancedSimulationMode()
{

    if(InSimulationMode) {
        SimulationStarted = TRUE;
        EnableMenuItem(AdvancedSimulateMenu, MNU_ADV_START_SIMULATION, MF_ENABLED);
        EnableMenuItem(AdvancedSimulateMenu, MNU_ADV_SINGLE_CYCLE, MF_ENABLED);

        EnableMenuItem(AdvancedFileMenu, MNU_ADV_OPEN, MF_GRAYED);
        EnableMenuItem(AdvancedFileMenu, MNU_ADV_SAVE, MF_GRAYED);
        EnableMenuItem(AdvancedFileMenu, MNU_ADV_SAVE_AS, MF_GRAYED);
        EnableMenuItem(AdvancedFileMenu, MNU_ADV_NEW, MF_GRAYED);

        EnableMenuItem(AdvancedTopMenu, 1, MF_GRAYED | MF_BYPOSITION);
        /*EnableMenuItem(AdvancedTopMenu, 2, MF_GRAYED | MF_BYPOSITION);
        EnableMenuItem(AdvancedTopMenu, 3, MF_GRAYED | MF_BYPOSITION);
        EnableMenuItem(AdvancedTopMenu, 5, MF_GRAYED | MF_BYPOSITION);*/
    
        CheckMenuItem(AdvancedSimulateMenu, MNU_ADV_SIMULATION_MODE, MF_CHECKED);

        // ClearSimulationData();
        // Recheck InSimulationMode, because there could have been a compile
        // error, which would have kicked us out of simulation mode.
        /*if(UartFunctionUsed() && InSimulationMode) {
            ShowUartSimulationWindow();
        }*/
    } else {
        // RealTimeSimulationRunning = FALSE;
        //KillTimer(MainWindow, TIMER_SIMULATE);

        EnableMenuItem(AdvancedSimulateMenu, MNU_ADV_START_SIMULATION, MF_GRAYED);
        EnableMenuItem(AdvancedSimulateMenu, MNU_ADV_STOP_SIMULATION, MF_GRAYED);
        EnableMenuItem(AdvancedSimulateMenu, MNU_ADV_SINGLE_CYCLE, MF_GRAYED);

        EnableMenuItem(AdvancedFileMenu, MNU_ADV_OPEN, MF_ENABLED);
        EnableMenuItem(AdvancedFileMenu, MNU_ADV_SAVE, MF_ENABLED);
        EnableMenuItem(AdvancedFileMenu, MNU_ADV_SAVE_AS, MF_ENABLED);
        EnableMenuItem(AdvancedFileMenu, MNU_ADV_NEW, MF_ENABLED);

        EnableMenuItem(AdvancedTopMenu, 1, MF_ENABLED | MF_BYPOSITION);
        /*EnableMenuItem(AdvancedTopMenu, 2, MF_ENABLED | MF_BYPOSITION);
        EnableMenuItem(AdvancedTopMenu, 3, MF_ENABLED | MF_BYPOSITION);
        EnableMenuItem(AdvancedTopMenu, 5, MF_ENABLED | MF_BYPOSITION);*/

        CheckMenuItem(AdvancedSimulateMenu, MNU_ADV_SIMULATION_MODE, MF_UNCHECKED);
        SimulationStarted = FALSE;

        /*if(UartFunctionUsed()) {
            DestroyUartSimulationWindow();
        }*/
    }

    // UpdateMainWindowTitleBar();

    DrawMenuBar(AdvancedDialog);
    InvalidateRect(AdvancedDialog, NULL, FALSE);
    // ListView_RedrawItems(, 0, Prog.io.count - 1);
}

static void CALLBACK DoubleClickProc(HWND hWnd, UINT msg, UINT Id, DWORD sysTime)
{
    KillTimer(AdvancedWorkspace, TIMERID);
    if(Clicked)
    {
        Clicked = FALSE;
    }
    // MessageBox(AdvancedDialog, "Double click duration", "Mouse Clicked", MB_OK | MB_ICONWARNING);
}

//-----------------------------------------------------------------------------
// Window procedure for Advanced Simulation Dialog Box
//-----------------------------------------------------------------------------
static LRESULT CALLBACK AdvancedDlgProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    switch (msg) {
        case WM_NOTIFY: {
            NMHDR *h = (NMHDR *)lParam;
            if(h->hwndFrom == ComponentList) {
                ComponentListProc(h);
            }
            if(h->hwndFrom == NamingList) {
                NamingListProc(h);
            }
            return 0;
            break;
        }

        case WM_COMMAND: {
            HWND h = (HWND)lParam;
            if(h == OkButton && wParam == BN_CLICKED) {
                AdvDialogDone = TRUE;
            } else if(h == CancelButton && wParam == BN_CLICKED) {
                AdvDialogDone = TRUE;
                AdvDialogCancel = TRUE;
            }
            ProcessAdvancedMenu(LOWORD(wParam));
            break;
        }
        case WM_PAINT: {
            RECT        r;
            PAINTSTRUCT ps;
            Hdc = BeginPaint(hwnd, &ps);
            // Draw the splitter thing to grab to resize componentList
            GetClientRect(AdvancedDialog, &r);
            r.left = AdvWrkspRight + 1;
            r.right = AdvWrkspRight + 2;
            FillRect(Hdc, &r, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
            r.left = AdvWrkspRight - 1;
            r.right = AdvWrkspRight;
            FillRect(Hdc, &r, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
            r.left = AdvWrkspRight;
            r.right = AdvWrkspRight + 1;
            FillRect(Hdc, &r, (HBRUSH)GetStockObject(WHITE_BRUSH));
            EndPaint(hwnd, &ps);

            // Draw the splitter thing to grab to resize componentList
            GetClientRect(AdvancedDialog, &r);
            r.top = AdvWrkspBottom + 1;
            r.bottom = AdvWrkspBottom + 2;
            FillRect(Hdc, &r, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
            r.top = AdvWrkspBottom - 1;
            r.bottom = AdvWrkspBottom;
            FillRect(Hdc, &r, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
            r.top = AdvWrkspBottom;
            r.bottom = AdvWrkspBottom + 1;
            FillRect(Hdc, &r, (HBRUSH)GetStockObject(WHITE_BRUSH));
            EndPaint(hwnd, &ps);
            return 1;
        }
        case WM_LBUTTONDOWN: {
            // MessageBox(AdvancedDialog,
            // _("Mouse Button Clicked"), _("Mouse click"), MB_OK | MB_ICONWARNING);
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            POINT pt;
            pt.x = x; pt.y = y;
            ClientToScreen(AdvancedWorkspace, &pt);
            MouseX = pt.x;
            MouseY = pt.y;
            if((x > (AdvWrkspRight - 9)) && (x < (AdvWrkspRight + 3))) {

            // char vx[3],vy[3];
            // _itoa(y,vy,10);
            // _itoa(x,vx,10);
            // MessageBox(AdvancedDialog,
            //    _(vx), _(vy), MB_OK | MB_ICONWARNING);
            ResizeSwitch =1;
            // InvalidateRect(AdvancedWorkspace, NULL, FALSE);
            }
            else if((y > (AdvWrkspBottom - 9)) && (y < (AdvWrkspBottom + 3)))
            {
                ResizeSwitch = 2;
            }
            if(ResizeSwitch > 0)
            {
                MouseHookHandle = SetWindowsHookEx(WH_MOUSE_LL,
                (HOOKPROC)MouseHook, Instance, 0);
                SetFocus(AdvancedWorkspace);
            }
            break;
        }
        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if((y >(AdvWrkspBottom - 9)) && (y < (AdvWrkspBottom + 3)) && (x < (AdvWrkspRight + 3)))
            {
                SetCursor(LoadCursor(NULL, IDC_SIZENS));
            }
            else if((x > (AdvWrkspRight - 9)) && (x < (AdvWrkspRight + 3))) 
            {
                SetCursor(LoadCursor(NULL, IDC_SIZEWE));
            }
            else
            {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
            }
            break;
        }
        case WM_SIZE:
            AdvancedDialogResized();
                break;
        case WM_CLOSE:
        case WM_DESTROY:
            AdvancedWindowClosing();
            AdvDialogDone = TRUE;
            AdvDialogCancel = TRUE;
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 1;
}

//-----------------------------------------------------------------------------
// Window procedure for Advanced Workspace 
// Black workspace area within the Advanced Dialog
//-----------------------------------------------------------------------------
static LRESULT CALLBACK AdvancedWrkspProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    switch (msg) {
        case WM_NOTIFY: {
            break;
        }

        case WM_COMMAND: {
            HWND h = (HWND)lParam;
            if(h == OkButton && wParam == BN_CLICKED) {
                AdvDialogDone = TRUE;
            } else if(h == CancelButton && wParam == BN_CLICKED) {
                AdvDialogDone = TRUE;
                AdvDialogCancel = TRUE;
            }
            break;
        }
        case WM_CREATE:{
            // test=LoadBitmap(GetModuleHandle(NULL),"relay_nc.bmp");
            /*test = (HBITMAP) LoadBitmap (Instance, MAKEINTRESOURCE(RELAY_NC));//,IMAGE_BITMAP,
                            // 0,0, LR_LOADTRANSPARENT);
            if(test ==NULL){
                MessageBox(hwnd, "Could not load Bitmap!", "Error", MB_OK | MB_ICONEXCLAMATION);
            }*/
            /*testmask = CreateBitmapMask(test, RGB(255, 255, 255));
            if(testmask ==NULL){
                MessageBox(hwnd, "Could not create mask!", "Error", MB_OK | MB_ICONEXCLAMATION);
            }*/
        }
        break;
        case WM_PAINT:{
            PAINTSTRUCT ps;
            int i;
            hdc = BeginPaint(hwnd, &ps);
            for(i=0; i < ImagesDrawn; i++)
            {
               ImageList_Draw(ImageStack[i].Image->Images, 0, hdc,
                    ImageStack[i].x, ImageStack[i].y, ILD_TRANSPARENT); 
            }
            // if(ComponentDiagrams == NULL){
            //     InitializeImageList();
            // }
            // InitializeComponentImage(0);
            // DeleteDC(HdcMem);
            EndPaint(hwnd, &ps);
        }
        break;
        case WM_LBUTTONDOWN:{
            // left button down
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if(!SimulationStarted)
            {
                // char vx[3],vy[3];
                // _itoa(y,vy,10);
                // _itoa(x,vx,10);
                // MessageBox(AdvancedDialog, _(vx), _(vy), MB_OK | MB_ICONWARNING);
                BeginComponentDrag(x, y);
            }
            else{
                // Send message to respective component

            }
            ProcessEvent(x, y, EVENT_MOUSE_DOWN);
        }
        break;
        case WM_LBUTTONUP:{
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if(!SimulationStarted){
                EndComponentDrag(x, y);
            }
            else{
                // Send message to respective component
                ProcessEvent(x, y, EVENT_MOUSE_UP);
            }
            // if((DragX < (x + 3)) && (DragX > (x-3)) && (DragY < (y + 3)) && (DragY > (y-3)))
            {
                /*MessageBox(NULL,
            ("Reached"), ("Info"), MB_OK | MB_ICONWARNING);*/
                ProcessEvent(x, y, EVENT_MOUSE_CLICK);
                // char vx[3],vy[3];
                // _itoa(y,vy,10);
                // _itoa(x,vx,10);
                // MessageBox(AdvancedDialog, _(vx), _(vy), MB_OK | MB_ICONWARNING);
                // MessageBox(AdvancedDialog, "Mouse Clicked", "Mouse Clicked", MB_OK | MB_ICONWARNING);
            }
        }
        break;
        // case WM_LBUTTONDBLCLK:
        // {
        //     int x = GET_X_LPARAM(lParam);
        //     int y = GET_Y_LPARAM(lParam);
        //     // MessageBox(AdvancedDialog, "Mouse DoubleClicked", "Mouse Double Clicked", MB_OK | MB_ICONWARNING);
        //     if(SimulationStarted){
        //         // Send message to respective component
        //     }
        //     else
        //     {
        //     }
        //     ProcessEvent(x, y, EVENT_MOUSE_DBLCLICK);
        // }
        // break;
        case WM_RBUTTONUP:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if(!SimulationStarted)
            {
                // Get Pin Parameters Here
            }
            ProcessEvent(x,y,EVENT_MOUSE_RCLICK);
        }
        break;
        case WM_MOUSEMOVE:{
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            ComponentDrag(x, y);
        }
        break;
        // case WM_SIZE:
        //     AdvancedDialogResized();
        //         break;
        case WM_CLOSE:
        case WM_DESTROY:{
            AdvDialogDone = TRUE;
            AdvDialogCancel = TRUE;
            // HeapDestroy(ImageHeap);
        }
        break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 1;
}
/*HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
    HDC HdcMem, HdcMem2;
    HBITMAP HbmMask;
    BITMAP bm;
    // Create monochrome (1 bit) mask bitmap.  
    GetObject(hbmColour, sizeof(BITMAP), &bm);
    HbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
    // Get some HDCs that are compatible with the display driver
    HdcMem = CreateCompatibleDC(0);
    HdcMem2 = CreateCompatibleDC(0);
    SelectObject(HdcMem, hbmColour);
    SelectObject(HdcMem2, HbmMask);
    // Set the background colour of the colour image to the colour
    // you want to be transparent.
    SetBkColor(HdcMem, crTransparent);

    BitBlt(HdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, HdcMem, 0, 0, SRCCOPY);
    BitBlt(HdcMem, 0, 0, bm.bmWidth, bm.bmHeight, HdcMem2, 0, 0, SRCINVERT);
    DeleteDC(HdcMem);
    DeleteDC(HdcMem2);
    return HbmMask;
}*/

//-----------------------------------------------------------------------------
//Create dialog box classes specifically for Advanced Simulation
//-----------------------------------------------------------------------------

void MakeAdvancedDialogClass(void)
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                          CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)AdvancedDlgProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName    = "AdvancedDialog";
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

void MakeAdvancedWorkspaceClass(void)
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNWINDOW | CS_BYTEALIGNWINDOW | CS_OWNDC;
    wc.lpfnWndProc      = (WNDPROC)AdvancedWrkspProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName    = "AdvancedWorkspace";
    wc.lpszMenuName     = NULL;
    RegisterClassEx(&wc);

}
void MakeComponentListClass(void)
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNWINDOW | CS_BYTEALIGNWINDOW | CS_OWNDC |
                          CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)AdvancedWrkspProc;
    wc.hInstance        = Instance;
    // wc.hbrBackground    = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName    = "ComponentList";
    // wc.lpszMenuName     = NULL;
    RegisterClassEx(&wc);
}

void MakeNamingListClass(void)
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNWINDOW | CS_BYTEALIGNWINDOW | CS_OWNDC |
                          CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)AdvancedDlgProc;
    wc.hInstance        = Instance;
    // wc.hbrBackground    = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName    = "NamingList";
    // wc.lpszMenuName     = NULL;
    RegisterClassEx(&wc);
}

void ShowAdvancedDialog()
{
    // int itmcount = Prog.io.count;
    //       char vx[3],vy[3];
    //         _itoa(10,vy,10);
    //         _itoa(itmcount,vx,10);
    //         MessageBox(MainWindow,
    //            _(vx), _(vy), MB_OK | MB_ICONWARNING);
    if(AdvancedWindowOpen)
    {
        return;
    }
    ComponentPin.Count = 0;
    ImageHeap = HeapCreate(0, 1024*64, 0);
    MakeAdvancedWindowMenus();
    AdvancedDialog = CreateWindowClient(WS_EX_CONTROLPARENT, "AdvancedDialog", 
        _("Advanced Simulation Mode"), WS_OVERLAPPED | WS_SYSMENU
        | WS_THICKFRAME | WS_CLIPCHILDREN | WS_SIZEBOX,
        10 , 10 , 800 , 600 , NULL , AdvancedTopMenu , Instance , NULL );
    InitComponents();
    RECT r;
    CompListWidth = 160;
    MakeAdvancedDialogControls();
    // AdvancedDialogResized();
    //RefreshDialogScrollbars();
    GetClientRect(AdvancedDialog , &r );

    // EnableWindow(MainWindow, FALSE);
    ShowWindow(AdvancedDialog, SW_SHOW);
    AdvancedWindowOpen = TRUE;
    ToggleAdvancedSimulationMode();

    MSG msg;
    DWORD ret;
    AdvDialogDone = FALSE;
    AdvDialogCancel = FALSE;
    while((ret = GetMessage(&msg, NULL, 0, 0)) && !AdvDialogDone) {
        // if(msg.message == WM_KEYDOWN) {
        //     if(msg.wParam == VK_RETURN) {
        //         AdvDialogDone = TRUE;
        //         break;
        //     } else if(msg.wParam == VK_ESCAPE) {
        //         AdvDialogDone = TRUE;
        //         AdvDialogCancel = TRUE;
        //         break;
        //     }
        // }

        if(IsDialogMessage(AdvancedDialog , &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }   

    // EnableWindow(MainWindow, TRUE);
    DestroyWindow(AdvancedDialog);
    AdvancedWindowOpen = FALSE;
    return;
}

void MakeAdvancedDialogControls(void)
{
    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
#define LV_ADD_COLUMN(hWnd, i, w, s) do { \
        lvc.iSubItem = i; \
        lvc.pszText = s; \
        lvc.iOrder = 0; \
        lvc.cx = w; \
        ListView_InsertColumn(hWnd, i, &lvc); \
    } while(0)
    
    AdvancedWorkspace = CreateWindowEx(WS_EX_CLIENTEDGE, "AdvancedWorkspace", "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP,
        0 , 0 , 640 , 600 , AdvancedDialog , NULL , Instance , NULL );
    
    ComponentList = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "ComponentList", WS_CHILD |
        LVS_REPORT | LVS_NOSORTHEADER | LVS_SORTASCENDING | LVS_SHOWSELALWAYS | WS_TABSTOP |
        LVS_SINGLESEL | WS_CLIPSIBLINGS, 
        12, 25, 300, 300, AdvancedDialog, NULL, Instance, NULL);
    ListView_SetExtendedListViewStyle(ComponentList, LVS_EX_FULLROWSELECT);
    LV_ADD_COLUMN(ComponentList, 0,  150,       _("Components"));
    ComponentListInitiate();
    
    NamingList = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "NamingList", WS_CHILD |
        LVS_REPORT | LVS_NOSORTHEADER | LVS_SORTASCENDING | LVS_SHOWSELALWAYS | WS_TABSTOP |
        LVS_SINGLESEL | WS_CLIPSIBLINGS, 312, 325, 300, 300, AdvancedDialog, NULL, Instance, NULL);
    LV_ADD_COLUMN(NamingList, 0,  250,       _("Name"));
    LV_ADD_COLUMN(NamingList, 1,  100,       _("State/Volt"));
    LV_ADD_COLUMN(NamingList, 2,  100,       _("Pin on Processor"));
    LV_ADD_COLUMN(NamingList, 3,  100,       _("MCU Port"));
    PopulateNamingList();
    RefreshNamingList();
    //     HorizScrollBar = CreateWindowEx(0, WC_SCROLLBAR, "", WS_CHILD |
    //     SBS_HORZ | SBS_BOTTOMALIGN | WS_VISIBLE | WS_CLIPSIBLINGS, 
    //     100, 100, 100, 100, AdvancedDialog, NULL, Instance, NULL);
    // VertScrollBar = CreateWindowEx(0, WC_SCROLLBAR, "", WS_CHILD |
    //     SBS_VERT | SBS_LEFTALIGN | WS_VISIBLE | WS_CLIPSIBLINGS, 
    //     200, 100, 100, 100, AdvancedDialog, NULL, Instance, NULL);
    // RECT scroll;
    // GetWindowRect(HorizScrollBar, &scroll);
    // ScrollHeight = scroll.bottom - scroll.top;
    // GetWindowRect(VertScrollBar, &scroll);
    // ScrollWidth = scroll.right - scroll.left;
    /*ComponentList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("listbox"), "Test", WS_CHILD
        | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | WS_CLIPSIBLINGS | WS_TABSTOP,
        240, 40, 150, 20, AdvancedDialog, NULL, Instance, NULL);
    SendMessage(ComponentList, LB_ADDSTRING, 0, (LPARAM)"name");
    SendMessage(ComponentList, LB_ADDSTRING, 0, (LPARAM)"extension");
    SendMessage(ComponentList, LB_ADDSTRING, 0, (LPARAM)"date");
    SendMessage(ComponentList, LB_ADDSTRING, 0, (LPARAM)"size");*/
    ShowWindow(AdvancedWorkspace, SW_SHOW);
    ShowWindow(ComponentList, SW_SHOW);
    ShowWindow(NamingList, SW_SHOW);
}

void AdvancedDialogResized(void)
{
    RECT AdvDial;
    GetClientRect(AdvancedDialog, &AdvDial);
    
    if(CompListWidth < 30) {
        CompListWidth = 30;
    }
    if(CompListWidth > AdvDial.right - 250){
        CompListWidth = AdvDial.right - 250;
    }
    if(NamingListHeight < 30) {
        NamingListHeight = 30;
    }
    if(NamingListHeight > AdvDial.bottom - 250){
        NamingListHeight = AdvDial.bottom - 250;
    }
    AdvWrkspRight = AdvDial.right - CompListWidth;
    AdvWrkspBottom = AdvDial.bottom - NamingListHeight;
    MoveWindow(AdvancedWorkspace, 0, 0,
        AdvWrkspRight - 1, AdvWrkspBottom - 1, TRUE);
    MoveWindow(ComponentList, AdvWrkspRight + 2, 0,
        CompListWidth, AdvDial.bottom, TRUE);
    MoveWindow(NamingList, 0, AdvWrkspBottom + 2,
        AdvWrkspRight - 1, AdvDial.bottom - AdvWrkspBottom - 2, TRUE);
    ListView_SetColumnWidth(ComponentList, 0, AdvDial.right - AdvWrkspRight - 5);
}

void MakeAdvancedWindowMenus(void)
{
    AdvancedFileMenu = CreatePopupMenu();
    AppendMenu(AdvancedFileMenu, MF_STRING, MNU_ADV_NEW, _("&New\tCtrl+N"));
    AppendMenu(AdvancedFileMenu, MF_STRING, MNU_ADV_OPEN, _("&Open...\tCtrl+O"));
    AppendMenu(AdvancedFileMenu, MF_STRING, MNU_ADV_SAVE, _("&Save\tCtrl+S"));
    AppendMenu(AdvancedFileMenu, MF_STRING, MNU_ADV_SAVE_AS, _("Save &As..."));
    AppendMenu(AdvancedFileMenu, MF_SEPARATOR, 0, "");
    AppendMenu(AdvancedFileMenu, MF_STRING,   MNU_EXIT,   _("E&xit"));

    AdvancedEditMenu = CreatePopupMenu();
    AppendMenu(AdvancedEditMenu, MF_STRING, MNU_ADV_UNDO, _("&Undo\tCtrl+Z"));
    AppendMenu(AdvancedEditMenu, MF_STRING, MNU_ADV_REDO, _("&Redo\tCtrl+Y"));
    AppendMenu(AdvancedEditMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(AdvancedEditMenu, MF_STRING, MNU_ADV_CUT, _("Cu&t\tCtrl+X"));
    AppendMenu(AdvancedEditMenu, MF_STRING, MNU_ADV_COPY, _("&Copy\tCtrl+C"));
    AppendMenu(AdvancedEditMenu, MF_STRING, MNU_ADV_PASTE, _("&Paste\tCtrl+V"));
    AppendMenu(AdvancedEditMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(AdvancedEditMenu, MF_STRING, MNU_ADV_DEL, _("De&lete\tCtrl+Y"));
    
    AdvancedSimulateMenu = CreatePopupMenu();
    AppendMenu(AdvancedSimulateMenu, MF_STRING, MNU_ADV_SIMULATION_MODE,
        _("Si&mulation Mode\tCtrl+M"));
    AppendMenu(AdvancedSimulateMenu, MF_STRING | MF_GRAYED,
        MNU_ADV_START_SIMULATION, _("Start &Real-Time Simulation\tCtrl+R"));
    AppendMenu(AdvancedSimulateMenu, MF_STRING | MF_GRAYED,
        MNU_ADV_STOP_SIMULATION, _("&Halt Simulation\tCtrl+H"));
    AppendMenu(AdvancedSimulateMenu, MF_STRING | MF_GRAYED,
        MNU_ADV_SINGLE_CYCLE, _("Single &Cycle\tSpace"));

    AdvancedHelpMenu = CreatePopupMenu();
    AppendMenu(AdvancedHelpMenu, MF_STRING, MNU_ADV_MANUAL, _("&Manual...\tF1"));
    AppendMenu(AdvancedHelpMenu, MF_STRING, MNU_ADV_ABOUT, _("&About..."));

    AdvancedTopMenu = CreateMenu();
    AppendMenu(AdvancedTopMenu, MF_STRING | MF_POPUP,
        (UINT_PTR)AdvancedFileMenu, _("&File"));
    AppendMenu(AdvancedTopMenu, MF_STRING | MF_POPUP,
        (UINT_PTR)AdvancedEditMenu, _("&Edit"));
    AppendMenu(AdvancedTopMenu, MF_STRING | MF_POPUP,
        (UINT_PTR)AdvancedSimulateMenu, _("Si&mulate"));
    AppendMenu(AdvancedTopMenu, MF_STRING | MF_POPUP,
        (UINT_PTR)AdvancedHelpMenu, _("&Help"));

}

void ProcessEvent(int x, int y, int Event)
{
    int i = 0;
    switch(Event)
    {
        case EVENT_MOUSE_CLICK:
        {
            if(Clicked)
            {
                KillTimer(AdvancedWorkspace, TIMERID);
                Event = EVENT_MOUSE_DBLCLICK;
                Clicked = FALSE;
            }else
            {
                Clicked = TRUE;
                DblClickTimer = SetTimer(AdvancedWorkspace, TIMERID,
                    GetDoubleClickTime(), DoubleClickProc);
            }
            break;
        }
    }

    for(i = 0; i < ImagesDrawn; i++)
    {
        if((((ImageStack[i].x + COMPONENT_WIDTH) > x) &&
            (ImageStack[i].x < x) &&
            (ImageStack[i].y < y) &&
            ((ImageStack[i].y + COMPONENT_HEIGHT) > y)))
        {
            /*char Debug[256];
            sprintf_s(Debug, "Editing Component Address: %p\n",ImageStack[i].Properties);
            OutputDebugString(Debug);*/
            NotifyComponent(ImageStack[i].Properties, ImageStack[i].PinName,
                ImageStack[i].Image->ComponentId, Event, SimulationStarted,
                &AdvancedDialog, ImageStack[i].Index, ImageStack[i].Id,
                &ImageStack[i].Image->Images);  //Replace Image in Imagelist here
        }
    }
}

void* AllocImageHeap(size_t n)
{
    void *p = HeapAlloc(ImageHeap, HEAP_ZERO_MEMORY, n);
    return p;
}

void* ReallocImageHeap(LPVOID lpMem, size_t n)
{
    // char Debug[256];
    void *p = HeapReAlloc(ImageHeap, HEAP_ZERO_MEMORY,
        lpMem, n);
    if(p == NULL)
    {
        /*sprintf_s(Debug, "Cannot Reallocate Memory\n");
        OutputDebugString(Debug);*/
        return lpMem;
    }
    return p;
}

void FreeImageHeap(void *p)
{
    HeapFree(ImageHeap, 0, p);
}

void SetMcu(int PinId, int Type)
{
    UINT i;
    for(i=0; i<MCUCount; i++)
    {
        if(McuPin[i].PinId == PinId)
        {
            McuPin[i].type = Type;
            McuPin[i].InternalPullup = TRUE;
            return;
        }
    }
    McuPin[MCUCount].type = Type;
    McuPin[MCUCount].InternalPullup = TRUE;
    McuPin[MCUCount++].PinId = PinId;
    /*char vx[3],vy[3];
            _itoa(MCUCount,vy,10);
            _itoa(McuPin[MCUCount].type,vx,10);
            MessageBox(NULL,
               _("SetMcu"), _(vx), MB_OK | MB_ICONWARNING);*/
}

void ToggleInternalPullup(int PinId)
{
    for(UINT i=0;i<MCUCount;i++)
    {
        if(McuPin[i].PinId == PinId)
        {
            McuPin[i].InternalPullup = !(McuPin[i].InternalPullup);
        }
    }
}

int RegisterPinName(LPCTSTR Name)
{
    UINT i;
    if(Name[0] == 0)
    {
        return 0;
    }
    for(i=0; i<NameCount; i++)
    {
        if((strcmp(NameId[i].Name, Name)) == 0)
        {
            return NameId[i].PinId;
        }
    }
    // NameId[NameCount].PinData->ImageId = (void**)AllocImageHeap(sizeof(void*));
    NameId[NameCount].PinData.ImageId = (void**)AllocImageHeap(sizeof(void*));
    NameId[NameCount].PinData.ImageType = (int*)AllocImageHeap(sizeof(int));
    NameId[NameCount].PinData.Index = (int*)AllocImageHeap(sizeof(int));
    // NameId[NameCount].PinData = &PinData[NameCount];    
    NameId[NameCount].PinData.LinkCount = 0;
    //SetMcu(NameCount, ImageId);
    NameId[NameCount].PinId = ++UniquePinId;
    
    if (sizeof(TCHAR) == sizeof(char))  // String is non-unicode
        strcpy(NameId[NameCount].Name, (char*)(Name));
    else                                // String is unicode
        wcstombs(NameId[NameCount].Name, (wchar_t*)Name, MAX_NAME_LENGTH);
    // MessageBox(AdvancedDialog, (LPCTSTR)NameId[NameCount].Name, "Registered", MB_OK | MB_ICONWARNING);
    
    /*char vx[3],vy[3];
    _itoa(10,vy,10);
    _itoa(NameId[NameCount].PinId,vx,10);
    MessageBox(AdvancedDialog,
        _(vx), Name, MB_OK | MB_ICONWARNING);*/
    NameCount++;
    RefreshNamingList();
    return NameId[i].PinId;
}
int DeletePinImage(LPCTSTR Name, void* ImageId, int Index)
{
    UINT i;
    int Count, j, index;
    int PinId = -1;
    void* Value;

    if(Name[0] == 0)
    {
        return PinId;
    }
    for(i=0; i<NameCount; i++)
    {
        if((strcmp(NameId[i].Name, Name)) == 0)
        {
            PinId = NameId[i].PinId;
            Count = NameId[i].PinData.LinkCount;
            if(Count == 0)
            {
                DeletePinName(i);
                return PinId;
            }
            /*char vx[3],vy[3];
                    // _itoa(Index,vy,10);
                    _itoa(Count,vx,10);
                    MessageBox(AdvancedDialog,
                        _(vx), "Deleting", MB_OK | MB_ICONWARNING);*/
            for(j =0; j < Count; j++)
            {
                Value =(void*) NameId[i].PinData.ImageId[j];
                index = NameId[i].PinData.Index[j];
                /*char vx[20],vy[3];
                    // _itoa(Index,vy,10);
                    _itoa((int)Value,vx,10);
                        MessageBox(AdvancedDialog,
                        _(vx), "Deleting", MB_OK | MB_ICONWARNING);*/
                if((ImageId == Value) /*&& (Index == index)*/)
                {
                    // MessageBox(NULL,"Value found","Info",MB_OK|MB_ICONWARNING);
                    if(index==Index)
                    {
                        NameId[i].PinData.ImageId[j] =
                            NameId[i].PinData.ImageId[--Count];
                        NameId[i].PinData.ImageType[j] =
                            NameId[i].PinData.ImageType[Count];
                        NameId[i].PinData.Index[j] =
                            NameId[i].PinData.Index[Count];
                        NameId[i].PinData.LinkCount = Count;
                        NameId[i].PinData.ImageId =
                            (void**)ReallocImageHeap(
                                (void**)NameId[i].PinData.ImageId,
                                (Count * sizeof(void*)));
                        NameId[i].PinData.ImageType =
                            (int*)ReallocImageHeap(NameId[i].PinData.ImageType,
                            (Count * sizeof(int)));
                        NameId[i].PinData.Index =
                            (int*)ReallocImageHeap(NameId[i].PinData.Index,
                            (Count * sizeof(int)));
                            /*char Debug[256];
                            sprintf_s(Debug, "Deleting Index[%d]", j);
                            OutputDebugString(Debug);*/
                        /*char vx[3],vy[3];
                        // _itoa(Index,vy,10);
                        _itoa(Count,vx,10);
                        MessageBox(AdvancedDialog,
                            _(vx), "Deleting", MB_OK | MB_ICONWARNING);*/
                        /*char vx[3],vy[3];
                        _itoa(Index,vy,10);
                        _itoa(Count,vx,10);
                        MessageBox(AdvancedDialog,
                            _(vx), _(vy), MB_OK | MB_ICONWARNING);*/
                        if(Count == 0)
                        {
                            DeletePinName(i);
                        }
                            return PinId;
                    }
                }
            }
        }
    }
    return -1;
}

int DeleteComponentPin(int PinId, void* ComponentAddress)
{
    PinComponent* temp = (PinComponent*)ComponentPin.Next;
    PinComponent* Prev = &ComponentPin;
    if(ComponentPin.PinId == NULL)
    {
        return -1;
    }
    else if(ComponentPin.PinId == PinId)
    {
            for(int i=0;i<ComponentPin.Count;i++)
            {
                if(ComponentPin.ComponentAddress[i] == ComponentAddress)
                {
                    ComponentPin.ComponentAddress[i] = 
                        ComponentPin.ComponentAddress[(--ComponentPin.Count)];
                    ReallocImageHeap(ComponentPin.ComponentAddress,
                        sizeof(void*)*ComponentPin.Count);
                    return PinId;
                }
            }
            if(ComponentPin.Count == 0)
            {
                if(ComponentPin.Next == NULL)
                {
                    ComponentPin.PinId = NULL;
                    return PinId;
                }
                else
                {
                    ComponentPin = *temp;
                    FreeImageHeap((void*)temp);
                }
                return PinId;
            }
        }
        else
        {            
            while(temp != NULL)
            {
                if(temp->PinId == PinId)
                {
                    for(int i=0; i<temp->Count; i++)
                    {
                        if(temp->ComponentAddress[i] == ComponentAddress)
                        {
                            if(temp->Count == 1)
                            {
                                Prev->Next = temp->Next;
                                FreeImageHeap((void*)temp);
                            }
                            else
                            {
                                temp->ComponentAddress[i] = 
                                    temp->ComponentAddress[
                                        (--temp->Count)];
                                ReallocImageHeap(temp->ComponentAddress,
                                    sizeof(void*) * (temp->Count));
                                return PinId;
                            }
                        }
                        Prev = temp;
                        temp = (PinComponent*)temp->Next;
                    }
                }
            }
        }
        return -1;
}
int AddComponentPin(int PinId, void* ComponentAddress)
{
    PinComponent* temp = (PinComponent*)ComponentPin.Next;
    if(ComponentPin.PinId == NULL)
    {
        ComponentPin.PinId = PinId;
        ComponentPin.Count = 1;
        ComponentPin.ComponentAddress = (void**)AllocImageHeap(sizeof(void*));
        ComponentPin.ComponentAddress[0] = ComponentAddress;
        return PinId;
    }
    else if(ComponentPin.PinId == PinId)
    {
        ComponentPin.ComponentAddress = (void**)ReallocImageHeap(
            (void*)ComponentPin.ComponentAddress,
            (sizeof(void*)) * (++ComponentPin.Count));
        ComponentPin.ComponentAddress[(ComponentPin.Count-1)] =ComponentAddress;
        return PinId;
    }
    else
    {
        PinComponent* Prev = &ComponentPin;
        while(temp != NULL)
        {
            if(temp->PinId == PinId)
            {
                temp->ComponentAddress = (void**)ReallocImageHeap(
                    (void*)ComponentPin.ComponentAddress,
                    (sizeof(void*) * (++ComponentPin.Count)));
                temp->ComponentAddress[
                    (ComponentPin.Count-1)] = ComponentAddress;
                return PinId;
            }
            Prev = temp;
            temp = (PinComponent*) temp->Next;
        }
        temp = (PinComponent*) AllocImageHeap(sizeof(PinComponent));
        temp->PinId =PinId;
        temp->ComponentAddress = (void**)AllocImageHeap(sizeof(void*));
        temp->Count = 1;
        temp->ComponentAddress[0] = ComponentAddress;
        Prev->Next = (void*)temp;
        return PinId;
    }
    return -1;
}

int SetPinImage(int PinId, void* ImageId, int ImageType, int Index)
{
    UINT i,j, Count;
    void* Value;
    int index;
    for(i=0; i<NameCount; i++)
    {
        if(NameId[i].PinId == PinId)
        {
            Count = NameId[i].PinData.LinkCount;
            for(j =0; j<Count; j++)
            {
                Value =(void*) NameId[i].PinData.ImageId[j];
                index = NameId[i].PinData.Index[j];
                if((ImageId == Value) && (index == Index))
                {
                    /*char Debug[256];
                    sprintf_s(Debug, "Already Exists\n");
                    OutputDebugString(Debug);*/
                    return NameId[i].PinId;
                }
            }
            NameId[i].PinData.LinkCount = ++Count;
            NameId[i].PinData.ImageId =
                (void**)ReallocImageHeap((void**)NameId[i].PinData.ImageId,
                (Count) * sizeof(void*));
            NameId[i].PinData.ImageType =
                (int*)ReallocImageHeap(NameId[i].PinData.ImageType,
                (Count) * sizeof(int));
            NameId[i].PinData.Index =
                (int*)ReallocImageHeap(NameId[i].PinData.Index,
                (Count) * sizeof(int));
            NameId[i].PinData.ImageId[Count-1] = ImageId;
            NameId[i].PinData.ImageType[Count-1] = ImageType;
            NameId[i].PinData.Index[Count-1] = Index;
            /*char Debug[256];
            sprintf_s(Debug, "Adding ImageId[%d]: %p \t Previous: %p\n",
                Count-1,NameId[i].PinData.ImageId[Count-1],NameId[i].PinData.ImageId[0]);
            OutputDebugString(Debug);*/
            /*for(j =0; j<Count; j++)
            {
                char vx[20],vy[3];
                    // _itoa(Index,vy,10);
                    _itoa((int)NameId[i].PinData.ImageId[j],vx,10);
                        MessageBox(AdvancedDialog,
                        _(vx), "Count", MB_OK | MB_ICONWARNING);
            }*/
            /*char vx[20],vy[3];
                    // _itoa(Index,vy,10);
                    _itoa((int)ImageId,vx,10);
                    MessageBox(AdvancedDialog,
                        _(vx), "Count", MB_OK | MB_ICONWARNING);*/
            return NameId[i].PinId;
        }
    }
    return -1;
}

int DeletePinName(UINT Index)
{
    UINT i;
    UINT PinId = NameId[Index].PinId;
    for(i=0;i<MCUCount;i++)
    {
        if(PinId == McuPin[i].PinId)
        {
            return 0;
        }
    }
    FreeImageHeap(NameId[Index].PinData.ImageId);
    FreeImageHeap(NameId[Index].PinData.ImageType);
    FreeImageHeap(NameId[Index].PinData.Index);
    NameId[Index] = NameId[--NameCount];
    RefreshNamingList();
    return 1;
}

void AdvancedWindowClosing()
{
    FlushPinNames();
}

int FlushPinNames()
{
    UINT i;
    int Count = 0;
    for(i=0; i<NameCount && NameCount>0; i++)
    {
        if(NameId[i].PinData.LinkCount<=0)
        {
            // MessageBox(NULL, (LPCTSTR)NameId[i].Name,"Deleted", MB_OK | MB_ICONWARNING);
            NameId[i--] = NameId[--NameCount];
            Count++;
        }
    }
    RefreshNamingList();
    /*char vx[3],vy[3];
    _itoa(10,vy,10);
    _itoa(NameCount,vx,10);
    MessageBox(NULL,
        _(vx), _(vy), MB_OK | MB_ICONWARNING);*/
    MCUCount =0;
    return Count;
}

void StartAdvSimulation()
{
    // RealTimeSimulationRunning = TRUE;
    EnableMenuItem(AdvancedSimulateMenu, MNU_ADV_START_SIMULATION, MF_GRAYED);
    EnableMenuItem(AdvancedSimulateMenu, MNU_ADV_STOP_SIMULATION, MF_ENABLED);
    StartAdvSimulationTimer();

    // UpdateMainWindowTitleBar();
}

//-----------------------------------------------------------------------------
// Stop real-time simulation. Have to update the controls grayed status
// to reflect this.
//-----------------------------------------------------------------------------
void StopAdvSimulation()
{
    // RealTimeSimulationRunning = FALSE;
    EnableMenuItem(AdvancedSimulateMenu, MNU_ADV_START_SIMULATION, MF_ENABLED);
    EnableMenuItem(AdvancedSimulateMenu, MNU_ADV_STOP_SIMULATION, MF_GRAYED);
    KillTimer(MainWindow, TIMER_SIMULATE);
    // UpdateMainWindowTitleBar();
}

void SimulateOneAdvCycle(BOOL ForceRefresh)
{
    static BOOL Simulating = FALSE;
    if(Simulating) return;
    Simulating = TRUE;
    /*Code here*/
    Simulating = FALSE;
}

void StartAdvSimulationTimer()
{
    int p = Prog.cycleTime/1000;
    if(p < 5)
    {
        SetTimer(AdvancedDialog, TIMER_ADV_SIMULATE, 10, CycleAdvTimer);
        CyclesPerTimerTick = 10000 / Prog.cycleTime;
    }else
    {
        SetTimer(AdvancedDialog, TIMER_ADV_SIMULATE, p, CycleAdvTimer);
        CyclesPerTimerTick = 1;
    }
}

void CALLBACK CycleAdvTimer(HWND hwnd, UINT msg, UINT_PTR id, DWORD time)
{
    int i;
    for(i = 0; i < CyclesPerTimerTick; i++)
    {
        SimulateOneAdvCycle(FALSE);
    }
}

// void CreateVoltRequest(int Index, double VoltReq)
// {
//     void* Value;
//     UINT Count,j;
//     Count = NameId[Index].PinData.LinkCount;
//     /*char vx[4],vy[4];
//     _itoa(NameId[i].PinData.Volt,vy,10);
//     _itoa(VoltReq,vx,10);
//     MessageBox(NULL,
//         (vy), (vx), MB_OK | MB_ICONWARNING);*/
//     if(Count>0)
//     {
//         // MessageBox(NULL,"reached", "info", MB_OK | MB_ICONEXCLAMATION);
//         for(j =0; j < Count; j++)
//         {
//             Value = (void*) NameId[Index].PinData.ImageId[j];
//             for(int k =0; k<ImagesDrawn; k++)
//             {
//                 if(ImageStack[k].Properties == Value)
//                 {
//                     NameId[Index].PinData.Volt = VoltSet(Value, SimulationStarted,
//                         NameId[Index].PinData.ImageType[j], NameId[Index].PinData.Index[j],
//                         VoltReq, SOURCE_PROGRAM_MAIN, &ImageStack[k].Image->Images);
//                     NameId[Index].PinData.ProgVolt = NameId[Index].PinData.Volt;

//                 }
//             }
//         }
//     }

// }

void TranslateState(char *name, BOOL state)
{
    double VoltReq;
    UINT i;
    VoltReq = (double) (state ? VOLT_5 : GND);
    for(i=0; i<NameCount; i++)
    {
        if((strcmp(NameId[i].Name, name)) == 0)
        {
            // CreateVoltRequest(i,VoltReq);
            /*NameId[i].PinData.ProgVolt = RefreshVolt(
                NameId[i].PinId, NULL, i, NULL, VoltReq);*/
            if((NameId[i].PinData.ProgVolt > VoltReq) ||
                (NameId[i].PinData.ProgComponent == &MainProgramComponent) ||
                (NameId[i].PinData.ProgComponent == NULL))
            {
                NameId[i].PinData.ProgComponent = &MainProgramComponent;
                if(VoltReq <= NameId[i].PinData.Volt)
                {
                    NameId[i].PinData.ProgVolt = VoltReq;
                    /*char vx[3],vy[10];
            _gcvt(NameId[i].PinData.ProgVolt,10,vy);
            _itoa(NameId[i].PinId,vx,10);
            MessageBox(AdvancedDialog,
               _(vy), _(NameId[i].Name), MB_OK | MB_ICONWARNING);*/
                    /*sprintf_s(Debug, "Executing1: \t ProgVoltage:%f\n",
                    NameId[Id].PinData.ProgVolt);
                    OutputDebugString(Debug);*/
                }
                else
                {
                    NameId[i].PinData.ProgVolt = NameId[i].PinData.Volt;
                    /*sprintf_s(Debug, "Executing2: \tVoltage:%f\n",
                        NameId[Id].PinData.Volt);
                    OutputDebugString(Debug);*/
                }
                RefreshVolt(NameId[i].PinId, NULL, i, NULL, VoltReq);
                /*sprintf_s(Debug, "Executing3: \t ProgVoltage:%f\n",
                    NameId[Id].PinData.ProgVolt);
                OutputDebugString(Debug);*/
            }
            /*char Debug[256];
    sprintf_s(Debug, "TranslateState: %s: %f \n", name, VoltReq);
    OutputDebugString(Debug);*/
        }
    }
}

void MCUPinState(char *name, BOOL state)
{
    double volt = (double) state?VOLT_5:GND;
    for(UINT i=0; i<NameCount; i++)
    {
        if((strcmp(NameId[i].Name, name)) == 0)
        {
            NameId[i].PinData.ProgVolt = volt;
        }
    }
}

int IsMCUPin(int PinId)
{
    UINT i;
    for(i=0; i<MCUCount; i++)
    {
        if(McuPin[i].PinId == PinId)
        {
            return i;
        }
    }
    return -1;
}

/*double RequestVoltChange(int PinId, void *ComponentAddress, double volt)
{
    // Compare NameId.PinData.Imageid with component address,
        and notify all other components of voltage change
    char* Name;
    int Pin;
    // MessageBox(AdvancedDialog, "Test", "Pullup", MB_OK | MB_ICONWARNING);
    for(unsigned int i =0;i<NameCount;i++)
    {
        if((NameId[i].PinId == PinId))
        {
          /*char vx[3],vy[10];
            _gcvt(NameId[i].PinData.Volt,10,vy);
            _itoa(PinId,vx,10);
            MessageBox(AdvancedDialog,
               _(vx), _(vy), MB_OK | MB_ICONWARNING);
            if(NameId[i].PinData.Volt <= volt)
            {
                // MessageBox(NULL, "Returning", "Mouse Clicked",
                    MB_OK | MB_ICONWARNING);
                return NameId[i].PinData.Volt;
            }
            Name = (char*)&NameId[i].Name;
            int num = NameId[i].PinData.LinkCount;
            Pin = i;
            for(int j=0;j<num;j++)
            {
                if(!(ComponentAddress == NameId[i].PinData.ImageId[j]))
                {
                    for(int k =0; k<ImagesDrawn; k++)
                    {
                        if(ImageStack[k].Properties == ComponentAddress)
                        {
                            if((NameId[i].PinData.ProgVolt > volt))
                            {
                                NameId[i].PinData.ProgVolt = VoltRequest(
                                    NameId[i].PinData.ImageId[j],
                                    SimulationStarted,
                                    NameId[i].PinData.ImageType[j],
                                    NameId[i].PinData.Index[j],
                                    volt, SOURCE_FORCE_MAIN,
                                    ImageStack[k].Image->Images);
                                NameId[i].PinData.ProgComponent =
                                ComponentAddress;
                                char Debug[256];
    sprintf_s(Debug, "1Name:%s \t ProgVolt:%f \t Volt:%f \t RVolt: %f\n", Name,
        NameId[i].PinData.ProgVolt,NameId[i].PinData.Volt,volt);
    OutputDebugString(Debug);
                            }
                            else if((NameId[i].PinData.ProgVolt < volt))
                            {
                                if(
                                    (NameId[i].PinData.ProgComponent ==
                                    ComponentAddress) ||
                                    NameId[i].PinData.ProgComponent == NULL)
                                {
                                    // MessageBox(AdvancedDialog, "Set",
                                        "Mouse Clicked",
                                        MB_OK | MB_ICONWARNING);
                                    NameId[i].PinData.ProgVolt = VoltRequest(
                                    NameId[i].PinData.ImageId[j],
                                    SimulationStarted,
                                    NameId[i].PinData.ImageType[j],
                                    NameId[i].PinData.Index[j], volt,
                                    SOURCE_FORCE_MAIN,
                                    ImageStack[k].Image->Images);
                                    char Debug[256];
    sprintf_s(Debug, "2Name:%s \t ProgVolt:%f \t Volt:%f \t RVolt: %f\n", Name,
        NameId[i].PinData.ProgVolt,NameId[i].PinData.Volt,volt);
    OutputDebugString(Debug);
                                }
                            }
                        }
                    }
                }
                else
                {
                    if((NameId[i].PinData.ProgComponent == ComponentAddress))
                    {
                        // MessageBox(AdvancedDialog, "Set", "Test",
                        MB_OK | MB_ICONWARNING);
                        for(int k =0; k<ImagesDrawn; k++)
                        {
                            if(ImageStack[k].Properties == ComponentAddress)
                            {
                                NameId[i].PinData.ProgVolt = VoltRequest(
                                    NameId[i].PinData.ImageId[j],
                                    SimulationStarted,
                                    NameId[i].PinData.ImageType[j],
                                    NameId[i].PinData.Index[j],
                                    volt, SOURCE_FORCE_MAIN,
                                    ImageStack[k].Image->Images);
                                NameId[i].PinData.ProgComponent =
                                ComponentAddress;
                            }
                        }
                        char Debug[256];
    sprintf_s(Debug, "3Name:%s \t ProgVolt:%f \t Volt:%f \t RVolt: %f\n", Name,
        NameId[i].PinData.ProgVolt,NameId[i].PinData.Volt,volt);
    OutputDebugString(Debug);
                    }
                    else if(NameId[i].PinData.ProgComponent == NULL)
                    {
                        NameId[i].PinData.ProgVolt = volt;
                        NameId[i].PinData.ProgComponent = ComponentAddress;
                    }
                }
            }
        }
    }
    int Index;
    Index = IsMCUPin(PinId);
    if(Index >=0)
    {
        if(McuPin[Index].type == IO_TYPE_DIG_INPUT)
        {
            if(NameId[Pin].PinData.ProgVolt == VOLT_5)
            {
                SimulationSetContact(Name);
                // MessageBox(AdvancedDialog, "Set", "Mouse Clicked",
                    MB_OK | MB_ICONWARNING);
            }
            else if(NameId[Pin].PinData.ProgVolt == GND)
            {
                SimulationResetContact(Name);
                // MessageBox(AdvancedDialog, "Reset", "Mouse Clicked",
                    MB_OK | MB_ICONWARNING);
            }
            else if(NameId[Pin].PinData.ProgVolt == V_OPEN)
            {
                if(McuPin[Index].InternalPullup)
                {
                    NameId[Pin].PinData.ProgVolt = VOLT_5;
                    SimulationSetContact(Name);
                    // MessageBox(AdvancedDialog, "Set", "Pullup",
                    MB_OK | MB_ICONWARNING);
                }
                else
                {
                    SimulationResetContact(Name);
                    // MessageBox(AdvancedDialog, "Set", "Pullup",
                        MB_OK | MB_ICONWARNING);
                }
            }
        }
        // char Debug[256];
    // sprintf_s(Debug, "Name:%s \t ProgVolt:%f \t Volt:%f \n", Name,
        NameId[Index].PinData.ProgVolt,NameId[Index].PinData.Volt);
    // OutputDebugString(Debug);
    }
    if(NameId[Pin].PinData.ProgVolt > NameId[Pin].PinData.Volt)
            {
                NameId[Pin].PinData.ProgVolt = NameId[Pin].PinData.Volt;
                char Debug[256];
    sprintf_s(Debug, "4Name:%s \t ProgVolt:%f \t Volt:%f \t RVolt: %f\n", Name,
        NameId[Pin].PinData.ProgVolt,NameId[Pin].PinData.Volt,volt);
    OutputDebugString(Debug);
            }
            else
            {
                char Debug[256];
    sprintf_s(Debug, "5Name:%s \t ProgVolt:%f \t Volt:%f \t RVolt: %f\n", Name,
        NameId[Pin].PinData.ProgVolt,NameId[Pin].PinData.Volt,volt);
    OutputDebugString(Debug);
            }
    char vx[10],vy[10];
            _gcvt(NameId[Pin].PinData.ProgVolt,8,vy);
            _itoa(Pin,vx,2);
            MessageBox(AdvancedDialog,
               Name, _(vy), MB_OK | MB_ICONWARNING);
    return NameId[Pin].PinData.ProgVolt;
}*/

double RequestVoltChange(int PinId, int Index,
    void *ComponentAddress, double volt)
{
    UINT Id;
    double temp;
    // char Debug[1024];
    for(UINT i=0; i<NameCount; i++)
    {
        if(NameId[i].PinId == PinId)
        {
            Id = i;
            break;
        }
    }
    // if(Id >= NameCount)
    // {
    //     sprintf_s(Debug, "PinId Not found %s: %d\n",
    //     NameId[Id].Name, PinId);
    //     OutputDebugString(Debug);
    // }
    temp = NameId[Id].PinData.ProgVolt;
    
    /*sprintf_s(Debug, "Changing Voltage %s: %f to %f \t ProgVolt:\
        %f \tChanger: %p\n",
        NameId[Id].Name, temp, volt, NameId[Id].PinData.ProgVolt,
        NameId[Id].PinData.ProgComponent);
        OutputDebugString(Debug);*/
    if((NameId[Id].PinData.ProgVolt > volt) ||
        (NameId[Id].PinData.ProgComponent == ComponentAddress) ||
        (NameId[Id].PinData.ProgComponent == NULL))
    {
        NameId[Id].PinData.ProgComponent = ComponentAddress;
        if(volt <= NameId[Id].PinData.Volt)
        {
            NameId[Id].PinData.ProgVolt = volt;
            /*sprintf_s(Debug, "Executing1: \t ProgVoltage:%f\n",
                NameId[Id].PinData.ProgVolt);
            OutputDebugString(Debug);*/
        }
        else
        {
            NameId[Id].PinData.ProgVolt = NameId[Id].PinData.Volt;
            /*sprintf_s(Debug, "Executing2: \tVoltage:%f\n",
            NameId[Id].PinData.Volt);
            OutputDebugString(Debug);*/
        }
        RefreshVolt(PinId, Index, Id, ComponentAddress,
            NameId[Id].PinData.ProgVolt);
        /*sprintf_s(Debug, "Executing3: \t ProgVoltage:%f\n",
            NameId[Id].PinData.ProgVolt);
        OutputDebugString(Debug);*/
    }
        RefreshProcessorStat(PinId, Id);
        /*sprintf_s(Debug, "Final Voltage %s: %f to %f \t ProgVolt:\
             %f \tChanger: %p\n",
            NameId[Id].Name, temp, volt, NameId[Id].PinData.ProgVolt,
            NameId[Id].PinData.ProgComponent);
        OutputDebugString(Debug);*/
    return NameId[Id].PinData.ProgVolt;
}

double RefreshVolt(int PinId, int Index, UINT Id,
    void* ComponentAddress, double volt)
{
    // char Debug[1024];
    double FinalVolt = volt;
    /*sprintf_s(Debug, "Executing4: \t ProgVoltage:%f\n",
        NameId[Id].PinData.ProgVolt);
    OutputDebugString(Debug);*/
    for(int i=0; i<NameId[Id].PinData.LinkCount;i++)
    {
        /*sprintf_s(Debug, "Currently processing: %s \tComponentAddress:%p\
             \tIndex:%d \tRequested by: %p,Index:%d\n",
            NameId[Id].Name, NameId[Id].PinData.ImageId[i],
            NameId[Id].PinData.Index[i],ComponentAddress, Index);
        OutputDebugString(Debug);*/
        if(!(NameId[Id].PinData.ImageId[i] == ComponentAddress))
        {
            /*sprintf_s(Debug, "Executing5: \t ProgVoltage:%f\n",
                NameId[Id].PinData.ProgVolt);
            OutputDebugString(Debug);*/
            for(int k =0; k<ImagesDrawn; k++)
            {
                if((ImageStack[k].Properties == NameId[Id].PinData.ImageId[i]))
                {
                    /*sprintf_s(Debug, "Requesting Voltage %s: %f to \
                        %f \tProgVolt: %f \tComponentAddress:%p, Index: %d\n",
                        NameId[Id].Name, volt, FinalVolt,
                        NameId[Id].PinData.ProgVolt,
                        NameId[Id].PinData.ImageId[i],
                        NameId[Id].PinData.Index[i]);
                    OutputDebugString(Debug);*/
                    /*if(ComponentAddress == NULL)
                    {
                                            char vx[sizeof(int)*8+1],vy[10];
                                    _gcvt(NameId[Id].PinData.ProgVolt,10,vy);
                                    // if(NameId[Id].PinData.Index)
                                        _itoa(ImageStack[i].Image->ComponentId,vx,10);
                                    MessageBox(AdvancedDialog,
                                       (LPSTR)(vx), (LPSTR)(NameId[Id].Name), MB_OK | MB_ICONWARNING);
                                }*/
                    
                    FinalVolt = VoltSet(NameId[Id].PinData.ImageId[i],
                        SimulationStarted, ImageStack[k].Image->ComponentId,
                        NameId[Id].PinData.Index[i],
                        NameId[Id].PinData.ProgVolt,
                        SOURCE_PROGRAM_MAIN, &ImageStack[k].Image->Images);
                    if(FinalVolt < NameId[Id].PinData.ProgVolt)
                    {
                        NameId[Id].PinData.ProgVolt = FinalVolt;
                        NameId[Id].PinData.ProgComponent =
                            ImageStack[k].Properties;
                        /*sprintf_s(Debug, "Executing7: \t \
                        ProgVoltage:%f\n",NameId[Id].PinData.ProgVolt);
                        OutputDebugString(Debug);*/
                    }
                    else
                    {
                        if(ImageStack[k].Properties ==
                            NameId[Id].PinData.ProgComponent)
                        {
                            NameId[Id].PinData.ProgVolt = FinalVolt;
                            NameId[Id].PinData.ProgComponent =
                                ImageStack[k].Properties;
                            /*sprintf_s(Debug, "Executing8: \t ProgVoltage:\
                            %f\n",NameId[Id].PinData.ProgVolt);
                            OutputDebugString(Debug);*/
                        }
                    }
                    if(NameId[Id].PinData.ProgVolt > NameId[Id].PinData.Volt)
                    {
                        NameId[Id].PinData.ProgVolt = NameId[Id].PinData.Volt;
                    }
                    /*sprintf_s(Debug, "Updating Voltage %s: %f to \
                        %f \tProgVolt: %f\n",NameId[Id].Name,
                        volt,FinalVolt,NameId[Id].PinData.ProgVolt);
                    OutputDebugString(Debug);*/
                }
            }
        }
    }
    /*sprintf_s(Debug, "Executing:10 \t ProgVoltage:\
    %f\n",NameId[Id].PinData.ProgVolt);
        OutputDebugString(Debug);*/
    RefreshProcessorStat(PinId, Id);
    /*if(ComponentAddress == NULL)
                    {
                                            char vx[10],vy[10];
                                    _gcvt(NameId[Id].PinData.ProgVolt,10,vy);
                                    _itoa(NameId[Id].PinId,vx,10);
                                    MessageBox(AdvancedDialog,
                                       (LPSTR)(vx), (LPSTR)(NameId[Id].Name), MB_OK | MB_ICONWARNING);
                                }*/
    /*sprintf_s(Debug, "Executing11: \t ProgVoltage:%f\n",
    NameId[Id].PinData.ProgVolt);
        OutputDebugString(Debug);*/
    return NameId[Id].PinData.ProgVolt;
}
double RefreshProcessorStat(int PinId, UINT Id)
{
    char* Name;
    // char Debug[256];
    int MCUPinId;
    MCUPinId = IsMCUPin(PinId);
    if(MCUPinId >=0)
    {
        Name = (char*)&NameId[Id].Name;
        if(McuPin[MCUPinId].type == IO_TYPE_DIG_INPUT)
        {
            if(NameId[Id].PinData.ProgVolt == VOLT_5)
            {
                SimulationSetContact(Name);
                // MessageBox(AdvancedDialog, "Set",
                // "Mouse Clicked", MB_OK | MB_ICONWARNING);
            }
            else if(NameId[Id].PinData.ProgVolt == GND)
            {
                SimulationResetContact(Name);
                // MessageBox(AdvancedDialog, "Reset",
                // "Mouse Clicked", MB_OK | MB_ICONWARNING);
            }
            else if(NameId[Id].PinData.ProgVolt == V_OPEN)
            {
                if(McuPin[MCUPinId].InternalPullup)
                {
                    /*sprintf_s(Debug, "Executing21: \t ProgVoltage:%f\n",
                        NameId[Id].PinData.ProgVolt);
                    OutputDebugString(Debug);*/
                    NameId[Id].PinData.ProgVolt = VOLT_5;
                    SimulationSetContact(Name);
                    // MessageBox(AdvancedDialog, "Set", "Pullup",\
                        // MB_OK | MB_ICONWARNING);
                }
                else
                {
                    SimulationResetContact(Name);
                    // MessageBox(AdvancedDialog, "Set", "Pullup",
                    // MB_OK | MB_ICONWARNING);
                }
            }
        }
    }
    else
    {
        if(NameId[Id].PinData.ProgVolt > NameId[Id].PinData.Volt)
        {
            /*sprintf_s(Debug, "Executing22: \t ProgVoltage:%f\n",
                NameId[Id].PinData.ProgVolt);
            OutputDebugString(Debug);*/
            NameId[Id].PinData.ProgVolt = NameId[Id].PinData.Volt;
        }
    }
    /*sprintf_s(Debug, "Executing23: \t ProgVoltage:%f\n",
        NameId[Id].PinData.ProgVolt);
    OutputDebugString(Debug);*/
    return NameId[Id].PinData.ProgVolt;
}

double GetGlobalVoltage(int PinId, void* ComponentAddress)
{
    int MCUPinId = IsMCUPin(PinId);
    for(UINT i=0; i<NameCount; i++)
    {
        if(NameId[i].PinId == PinId)
        {
        /*char Debug[256];
        sprintf_s(Debug, "GetGlobalVoltage: %s\n",
            NameId[i].Name);
        OutputDebugString(Debug);*/
        if(MCUPinId >=0)
        {
            if(McuPin[MCUPinId].type == IO_TYPE_DIG_INPUT)
            {
                if(ComponentAddress == NameId[i].PinData.ProgComponent)
                {
                    return VOLT_5;
                }
            }
        }
        return NameId[i].PinData.ProgVolt;
        }
    }
    return GND - 1;
}
// int RegisterPinState(int Index, double Volt)
// {
//     PinData[Index].McuPin = TRUE;
//     return 0;
// }

/*void RefreshDialogScrollbars(void)
{
    SCROLLINFO vert, horiz;
    SetUpScrollbars(&NeedHoriz, &horiz, &vert);
    SetScrollInfo(HorizScrollBar, SB_CTL, &horiz, TRUE);
    SetScrollInfo(VertScrollBar, SB_CTL, &vert, TRUE);

    RECT main;
    GetClientRect(AdvancedWorkspace, &main);

    if(NeedHoriz) {
        MoveWindow(HorizScrollBar, 0, IoListTop - ScrollHeight - 2,
            main.right - ScrollWidth - 2, ScrollHeight, TRUE);
        ShowWindow(HorizScrollBar, SW_SHOW);
        EnableWindow(HorizScrollBar, TRUE);
    } else {
        ShowWindow(HorizScrollBar, SW_HIDE);
    }
    MoveWindow(VertScrollBar, main.right - ScrollWidth - 2, 1, ScrollWidth,
        NeedHoriz ? (IoListTop - ScrollHeight - 4) : (IoListTop - 3), TRUE);

    MoveWindow(VertScrollBar, main.right - ScrollWidth - 2, 1, ScrollWidth,
        NeedHoriz ? (IoListTop - ScrollHeight - 4) : (IoListTop - 3), TRUE);

    InvalidateRect(AdvancedDialog, NULL, FALSE);
}*/