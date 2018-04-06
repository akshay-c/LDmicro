#include <wincodec.h>
#include <stdio.h>
//#include <string.h>
#include <commctrl.h>
#include <Windowsx.h>

#include "componentstructs.h"
#include "componentfunctions.h"
#include "componentimages.h"
#include "components.h"

static HWND TemporaryRadio;
static HWND OpenRadio;
static HWND ClosedRadio;
static HWND NameTextbox;
HWND* DpstDialog;


void DpstStateChanged(DpstStruct* DpstData, void* ImageLocation);
void MakeDpstControls()
{
    HWND ActionGrouper = CreateWindowEx(0, WC_BUTTON, ("Default State"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
        7, 3, 120, 65, *DpstDialog, NULL, NULL, NULL);
    FontNice(ActionGrouper);

    OpenRadio = CreateWindowEx(0, WC_BUTTON, ("Open"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        16, 21, 100, 20, *DpstDialog, NULL, NULL, NULL);
    FontNice(OpenRadio);

    ClosedRadio = CreateWindowEx(0, WC_BUTTON, ("Closed"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 41, 100, 20, *DpstDialog, NULL, NULL, NULL);
    FontNice(ClosedRadio);

    HWND textLabel = CreateWindowEx(0, WC_STATIC, ("Name:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        100, 80, 50, 21, *DpstDialog, NULL, NULL, NULL);
    FontNice(textLabel);

    NameTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT,"",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 80, 155, 21, *DpstDialog, NULL, NULL, NULL);
    FontFixed(NameTextbox);

    /*PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNameProc);*/
}

void LoadState(DpstStruct* Data)
{
    if(Data->Open)
    {
        Button_SetCheck(OpenRadio, BST_CHECKED);
    }else
    {
        Button_SetCheck(ClosedRadio, BST_CHECKED);
    }
}


BOOL SaveDpstDialog(DpstStruct* Data)
{
    char temp[15]; 
    BOOL Open;
    if(Button_GetState(OpenRadio) == BST_CHECKED)
    {
        Open = TRUE;
            
    }else if(Button_GetState(ClosedRadio) == BST_CHECKED)
    {
        Open = FALSE;
    }else
    {
        MessageBox(*DpstDialog,
            ("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
        return FALSE;
    }

    if(Edit_GetText(NameTextbox, (LPSTR)&temp, 15) < 1)
    {
        MessageBox(*DpstDialog,
            ("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
        return FALSE;
    }else
    {
        Data-> Open = Open;
        strcpy(Data->Name, temp);
        if(Open)
        {
            Data->Image =DPST_SWITCH_2;
        }
        else
        {
            Data->Image =DPST_SWITCH_1;
        }
    }
    return TRUE;
}

void ShowDpstDialog(void* ComponentAddress)
{
    DpstStruct* Data = (DpstStruct*) ComponentAddress;
    BOOL Canceled, Complete = TRUE;
    DpstDialog = CreateDialogWindow("Dpst Dialog", 100, 100, 263, 145, STYLE_VERTICAL);
    MakeDpstControls();
    ShowDialogWindow();
    LoadState(Data);
    Canceled = ProcessDialogWindow();
    while(Canceled == FALSE)
    {
        Complete = SaveDpstDialog(Data);
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
    DestroyWindow(*DpstDialog);
}


int InitDpst(void* ComponentAddress)
{
    DpstStruct* temp = (DpstStruct*)ComponentAddress;
    temp->Image = DPST_SWITCH_2;
    temp->Open = TRUE;
    temp->Volt[0] = V_OPEN;
    temp->Volt[1] = V_OPEN;
    temp->Volt[2] = V_OPEN;
    temp->Volt[3] = V_OPEN;
    return DPST_SWITCH_2;
}


double UpdateValuesDpst(void* ComponentAddress, int i=0)
{
    /*char Debug[256];
    sprintf_s(Debug, "Clicked: %p\n", ComponentAddress);
    OutputDebugString(Debug);*/
	DpstStruct* Switch = (DpstStruct*)ComponentAddress;

    if(Switch->Open)
        {
            Switch->Volt[0] =
                VoltChange(Switch->PinId[0], 0, ComponentAddress, V_OPEN); 
            Switch->Volt[1] =
                VoltChange(Switch->PinId[1], 1, ComponentAddress, V_OPEN);
            Switch->Volt[2] =
                VoltChange(Switch->PinId[2], 2, ComponentAddress, V_OPEN); 
            Switch->Volt[3] =
                VoltChange(Switch->PinId[3], 3, ComponentAddress, V_OPEN);
        }

    else
    	{

            double Voltage;
            Voltage = VoltRequest(Switch->PinId[0], Switch);
            Switch->Volt[0] = Voltage;
            Voltage = VoltRequest(Switch->PinId[1], Switch);
            Switch->Volt[1] = Voltage;
            Voltage = VoltRequest(Switch->PinId[2], Switch);
            Switch->Volt[2] = Voltage;
            Voltage = VoltRequest(Switch->PinId[3], Switch);
            Switch->Volt[3] = Voltage;


            if(Switch->Volt[0]==GND || Switch->Volt[2]==GND)
            {
            	Switch->Volt[2]=VoltChange(Switch->PinId[2], 2, ComponentAddress, GND);
            	Switch->Volt[0]=VoltChange(Switch->PinId[0],0,ComponentAddress,GND);
            }
            else
            {
            double tempvolt=max(VoltRequest(Switch->PinId[0], ComponentAddress),VoltRequest(Switch->PinId[2], ComponentAddress));
                Switch->Volt[2]=VoltChange(Switch->PinId[2],2,ComponentAddress,tempvolt);
                Switch->Volt[0]=VoltChange(Switch->PinId[0],0,ComponentAddress,tempvolt);
            }

            if(Switch->Volt[1]==GND || Switch->Volt[3]==GND)
            {
                Switch->Volt[1]=VoltChange(Switch->PinId[1], 1, ComponentAddress, GND);
                Switch->Volt[3]=VoltChange(Switch->PinId[3],3,ComponentAddress,GND);
            }
         
            else
            {
            double tempvolt=max(VoltRequest(Switch->PinId[3], ComponentAddress),VoltRequest(Switch->PinId[1], ComponentAddress));
            	Switch->Volt[1]=VoltChange(Switch->PinId[1],1,ComponentAddress,tempvolt);
            	Switch->Volt[3]=VoltChange(Switch->PinId[3],3,ComponentAddress,tempvolt);
            }
	    }    	
 return Switch->Volt[i];
}


void HandleDpstEvent(void* ComponentAddress, int Event, BOOL SimulationStarted,
    void* ImageLocation, UINT ImageId, HWND* h)
{
    DpstStruct* temp = (DpstStruct*)ComponentAddress;

    if(SimulationStarted)
    {
        switch(Event)
        {
            case EVENT_MOUSE_CLICK:
            	DpstStateChanged(temp,ImageLocation);
            	UpdateValuesDpst(ComponentAddress);
            	break;
         	default:
         		break;
        }
        
    }
    else
    {
        switch(Event){
            case EVENT_MOUSE_DBLCLICK:
            	ShowDpstDialog(ComponentAddress);
                SetImage(temp->Image, ImageLocation);
                RefreshImages();
            	break;
            default:
            	break;
    }
    }
}

void DpstStateChanged(DpstStruct* DpstData, void* ImageLocation)
{
    SetImage(DpstData->Open ? DPST_SWITCH_2 : DPST_SWITCH_1,
            ImageLocation);
        RefreshImages();
}

double DpstVoltChanged(void* DpstData, BOOL SimulationStarted, int Index,
    double Volt, int Source, void* ImageLocation)
{

    // char Debug[256];
    DpstStruct* temp = (DpstStruct*)DpstData;
    if(SimulationStarted)
    	return UpdateValuesDpst(DpstData,Index);
    return Volt;
}


void SetDpstIds(int* id,void* ComponentAddress)
{
    DpstStruct* s = (DpstStruct*)ComponentAddress;
    s->PinId[0] = *id++;
    s->PinId[1] = *id++;
    s->PinId[2] = *id++;
    s->PinId[3] = *id++;
    /*char Debug[256];
    sprintf_s(Debug, "SetSwitchIds: \tAddr1: %p \tAddr2: %p \tPin0:%d \t Pin1:%d \tVolt0: %f Volt1: %f\n",
        ComponentAddress, s, s->PinId[0], s->PinId[1],VoltRequest(s->PinId[0], ComponentAddress),
        VoltRequest(s->PinId[0], ComponentAddress));
    OutputDebugString(Debug);
    char vx[4],vy[4];
    _itoa(s->PinId[0],vy,10);
            _itoa(s->PinId[1],vx,10);
            MessageBox(NULL,
               (vy), (vx), MB_OK | MB_ICONWARNING);*/
}
