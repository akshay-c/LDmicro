#include <wincodec.h>
#include <stdio.h>
#include <math.h>
//#include <string.h>
#include <commctrl.h>
#include <Windowsx.h>

#include "components.h"
#include "componentstructs.h"
#include "componentfunctions.h"
#include "componentimages.h"

HWND* RelayDialog;
HWND MinOperatingVoltTextbox;
HWND MaxOperatingVoltTextbox;

enum RelayPins{
    Coil1 = 0,
    Coil2 = 1,
    COM = 2,
    NO = 3,
    NC = 4,
};

int InitRelay(void* ComponentAddress)
{
    RelayStruct* temp = (RelayStruct*)ComponentAddress;
    temp->Image = RELAY_NC;
    temp->MinOperatingVolt = 5;
    temp->MaxOperatingVolt = 12;
    temp->NC = TRUE;

    return RELAY_NC;
}

void MakeRelayControls(void)
{
    HWND MinOperatingVoltLabel = CreateWindowEx(0, WC_STATIC, ("Minimum Operating Volt:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 21, 100, 21, *RelayDialog, NULL, NULL, NULL);
    FontNice(MinOperatingVoltLabel);

    HWND MaxOperatingVoltLabel = CreateWindowEx(0, WC_STATIC, ("Maximum Operating Volt:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 41, 100, 21, *RelayDialog, NULL, NULL, NULL);
    FontNice(MaxOperatingVoltLabel);

    MinOperatingVoltTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        110, 21, 155, 21, *RelayDialog, NULL, NULL, NULL);
    FontFixed(MinOperatingVoltTextbox);

    MaxOperatingVoltTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        110, 41, 155, 21, *RelayDialog, NULL, NULL, NULL);
    FontFixed(MaxOperatingVoltTextbox);

    /*PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNameProc);*/
}

void LoadState(RelayStruct* Data)
{
    char Voltage[15];
    sprintf((char*)Voltage,"%f",Data->MinOperatingVolt);
    Edit_SetText(MinOperatingVoltTextbox, Voltage);
    sprintf((char*)Voltage,"%f",Data->MaxOperatingVolt);
    Edit_SetText(MaxOperatingVoltTextbox, Voltage);
}

BOOL SaveRelayDialog(RelayStruct* Data)
{
    double volt, volt2;
    char Voltage[15];
    Edit_GetText(MinOperatingVoltTextbox, (LPSTR)&Voltage, 15);
    volt = atof(Voltage);
    Edit_GetText(MaxOperatingVoltTextbox, (LPSTR)&Voltage, 15);
    volt2 = atof(Voltage);
    if(volt == 0.0f)
    {
        MessageBox(*RelayDialog,
            ("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
        return FALSE;
    }
    else if((volt2 == 0.0f) || (volt2<volt))
    {
        MessageBox(*RelayDialog,
            ("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
        return FALSE;
    }else
    {
        Data->MinOperatingVolt = volt;
        Data->MaxOperatingVolt = volt2;
    }
    return TRUE;
}

void ShowRelayDialog(void* ComponentAddress)
{
    RelayStruct* Data = (RelayStruct*) ComponentAddress;
    BOOL Canceled, Complete = TRUE;
    RelayDialog = CreateDialogWindow("Relay Dialog", 100, 100, 290, 50, STYLE_HORIZONTAL);
    MakeRelayControls();
    ShowDialogWindow();
    LoadState(Data);
    Canceled = ProcessDialogWindow();
    while(Canceled == FALSE)
    {
        Complete = SaveRelayDialog(Data);
        if(Complete == TRUE)
        {
            // MessageBox(*RelayDialog,
            //     ("Saved"), ("Mouse click"), MB_OK | MB_ICONWARNING);
            break;
        }else
        {
            Complete =TRUE;
            Canceled = ProcessDialogWindow();
        }
    }
    DestroyWindow(*RelayDialog);
}

void HandleRelayEvent(void* ComponentAddress, int Event, BOOL SimulationStarted,
    void* ImageLocation, UINT ImageId, HWND* h)
{
    RelayStruct *temp = (RelayStruct*)ComponentAddress;
    if(SimulationStarted)
    {
        /*Relay is a static component. It's value does not change with
         direct user interaction hence this part is blank for relays*/
    }
    else
    {
        switch(Event){
            case EVENT_MOUSE_DBLCLICK:
                ShowRelayDialog(ComponentAddress);
                // SetImage(temp->Image, ImageLocation);
                // RefreshImages();
            break;
    }
    }
}

double RelayVoltChanged(void* ComponentAddress, BOOL SimulationStarted, int index,
    double Volt, int Source, void* ImageLocation)
{
    if(SimulationStarted)
    {
        RelayStruct *Relay = (RelayStruct*)ComponentAddress;
        /*if(index == Coil2)
        {    
                char vx[10],vy[10];
                _gcvt(Volt, 10, vy);
                _itoa(index, vx, 10);
                MessageBox(NULL,
                    (LPSTR)(vx), (LPSTR)(vx), MB_OK | MB_ICONWARNING);
            }*/
        double Volt2,diff;
        switch(index)
        {
            case Coil1:
                Volt2 = VoltRequest(Relay->PinId[Coil2], ComponentAddress);
            break;
            case Coil2:
                Volt2 = VoltRequest(Relay->PinId[Coil1], ComponentAddress);
            break;
            default:
                return Volt;
        }
        diff = fabs(Volt2-Volt);
        if((diff >= Relay->MinOperatingVolt) && (diff <= Relay->MaxOperatingVolt))
        {
            Relay->Image = RELAY_NO;
        }
        else
        {
            Relay->Image = RELAY_NC;
        }
        SetImage(Relay->Image, ImageLocation);
        RefreshImages();
    }
    /*char Debug[256];
    sprintf_s(Debug, "RelayVoltChanged: \tAddr1: %p \t Volt1: %f \t Volt2: %f\n",
        ComponentAddress, Volt, Volt2);
    OutputDebugString(Debug);*/
    return Volt;
}

void SetRelayIds(int* id,void* ComponentAddress)
{
    RelayStruct *s = (RelayStruct*)ComponentAddress;
    s->PinId[Coil1] = *id++;
    s->PinId[Coil2] = *id++;
    s->PinId[COM] = *id++;
    s->PinId[NO] = *id++;
    s->PinId[NC] = *id++;

    /*char Debug[256];
    sprintf_s(Debug, "SetRelayIds: \tAddr1: %p \tAddr2: %p \tPin0:%d \t Pin1:%d \tPin2: %d Pin3: %d\n",
        ComponentAddress, s, s->PinId[0], s->PinId[1], s->PinId[2], s->PinId[3]);
    OutputDebugString(Debug);
    char vx[4],vy[4];
    _itoa(s->PinId[0],vy,10);
            _itoa(s->PinId[1],vx,10);
            MessageBox(NULL,
               (vy), (vx), MB_OK | MB_ICONWARNING);*/
}