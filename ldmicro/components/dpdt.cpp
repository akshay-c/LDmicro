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
static HWND Circuit1Radio;
static HWND Circuit2Radio;
static HWND NameTextbox;
HWND* DpdtDialog;


void DpdtStateChanged(DpdtStruct* DpdtData, void* ImageLocation);
void MakeDpdtControls()
{
    HWND ActionGrouper = CreateWindowEx(0, WC_BUTTON, ("Default Shorted Condition"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
        7, 3, 120, 65, *DpdtDialog, NULL, NULL, NULL);
    FontNice(ActionGrouper);

    Circuit1Radio = CreateWindowEx(0, WC_BUTTON, ("Circuit 1"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        16, 21, 100, 20, *DpdtDialog, NULL, NULL, NULL);
    FontNice(Circuit1Radio);

    Circuit2Radio = CreateWindowEx(0, WC_BUTTON, ("Circuit 2"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 41, 100, 20, *DpdtDialog, NULL, NULL, NULL);
    FontNice(Circuit2Radio);

    HWND textLabel = CreateWindowEx(0, WC_STATIC, ("Name:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        100, 80, 50, 21, *DpdtDialog, NULL, NULL, NULL);
    FontNice(textLabel);

    NameTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT,"",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 80, 155, 21, *DpdtDialog, NULL, NULL, NULL);
    FontFixed(NameTextbox);

    /*PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNameProc);*/
}

void LoadState(DpdtStruct* Data)
{
    if(Data->Circuit)
    {
        Button_SetCheck(Circuit2Radio, BST_CHECKED);
    }else
    {
        Button_SetCheck(Circuit1Radio, BST_CHECKED);
    }
}


BOOL SaveDpdtDialog(DpdtStruct* Data)
{
    char temp[15];
    BOOL Circuit;
    if(Button_GetState(Circuit1Radio) == BST_CHECKED)
    {
        Circuit = FALSE;
            
    }else if(Button_GetState(Circuit2Radio) == BST_CHECKED)
    {
        Circuit = TRUE;
    }else
    {
        MessageBox(*DpdtDialog,
            ("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
        return FALSE;
    }

    if(Edit_GetText(NameTextbox, (LPSTR)&temp, 15) < 1)
    {
        MessageBox(*DpdtDialog,
            ("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
        return FALSE;
    }else
    {
        Data-> Circuit = Circuit;
        strcpy(Data->Name, temp);
        if(Circuit)
        {
            Data->Image =DPDT_SWITCH_2;
        }
        else
        {
            Data->Image =DPDT_SWITCH_1;
        }
    }
    return TRUE;
}

void ShowDpdtDialog(void* ComponentAddress)
{
    DpdtStruct* Data = (DpdtStruct*) ComponentAddress;
    BOOL Canceled, Complete = TRUE;
    DpdtDialog = CreateDialogWindow("Dpdt Dialog", 100, 100, 263, 145, STYLE_VERTICAL);
    MakeDpdtControls();
    ShowDialogWindow();
    LoadState(Data);
    Canceled = ProcessDialogWindow();
    while(Canceled == FALSE)
    {
        Complete = SaveDpdtDialog(Data);
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
    DestroyWindow(*DpdtDialog);
}


int InitDpdt(void* ComponentAddress)
{
    DpdtStruct* temp = (DpdtStruct*)ComponentAddress;
    temp->Image = DPDT_SWITCH_1;
    temp->Circuit = FALSE;
    temp->Volt[0] = V_OPEN;
    temp->Volt[1] = V_OPEN;
    temp->Volt[2] = V_OPEN;
    temp->Volt[3] = V_OPEN;
    temp->Volt[4] = V_OPEN;
    temp->Volt[5] = V_OPEN;


    return DPDT_SWITCH_1;
}


double UpdateDpdtValues(void* ComponentAddress, int i=0)
{
    /*char Debug[256];
    sprintf_s(Debug, "Clicked: %p\n", ComponentAddress);
    OutputDebugString(Debug);*/
	DpdtStruct* Switch = (DpdtStruct*)ComponentAddress;

    if(Switch->Circuit)
        {
            Switch->Volt[2] =
                VoltChange(Switch->PinId[2], 2, ComponentAddress, V_OPEN);
            Switch->Volt[4] =
                VoltChange(Switch->PinId[4], 4, ComponentAddress, V_OPEN);

            double Voltage;
            Voltage = VoltRequest(Switch->PinId[0], ComponentAddress);
            Switch->Volt[0] = Voltage;
            Voltage = VoltRequest(Switch->PinId[1], ComponentAddress);
            Switch->Volt[1] = Voltage;

            Voltage = VoltRequest(Switch->PinId[3], ComponentAddress);
            Switch->Volt[3] = Voltage;
            Voltage = VoltRequest(Switch->PinId[5], ComponentAddress);
            Switch->Volt[5] = Voltage;


            if(Switch->Volt[0]==GND || Switch->Volt[3]==GND)
            {
            	Switch->Volt[3]=VoltChange(Switch->PinId[3], 3, ComponentAddress, GND);
            	Switch->Volt[0]=VoltChange(Switch->PinId[0],0,ComponentAddress,GND);
            }
            else
            {
            	double tempvolt=max(VoltRequest(Switch->PinId[0], ComponentAddress),VoltRequest(Switch->PinId[3], ComponentAddress));
            	Switch->Volt[3]=VoltChange(Switch->PinId[3],3,ComponentAddress,tempvolt);
            	Switch->Volt[0]=VoltChange(Switch->PinId[0],0,ComponentAddress,tempvolt);
            }
            if(Switch->Volt[1]==GND || Switch->Volt[5]==GND)
            {
                Switch->Volt[5]=VoltChange(Switch->PinId[5], 5, ComponentAddress, GND);
                Switch->Volt[1]=VoltChange(Switch->PinId[1],1,ComponentAddress,GND);
            }
            else
            {
                double tempvolt=max(VoltRequest(Switch->PinId[1], ComponentAddress),VoltRequest(Switch->PinId[5], ComponentAddress));
                Switch->Volt[5]=VoltChange(Switch->PinId[5],5,ComponentAddress,tempvolt);
                Switch->Volt[1]=VoltChange(Switch->PinId[1],1,ComponentAddress,tempvolt);
            }

        }


    else
        {
            Switch->Volt[3] =
                VoltChange(Switch->PinId[3], 3, ComponentAddress, V_OPEN);
            Switch->Volt[5] =
                VoltChange(Switch->PinId[5], 5, ComponentAddress, V_OPEN);

            double Voltage;
            Voltage = VoltRequest(Switch->PinId[0], ComponentAddress);
            Switch->Volt[0] = Voltage;
            Voltage = VoltRequest(Switch->PinId[1], ComponentAddress);
            Switch->Volt[1] = Voltage;

            Voltage = VoltRequest(Switch->PinId[2], ComponentAddress);
            Switch->Volt[2] = Voltage;
            Voltage = VoltRequest(Switch->PinId[4], ComponentAddress);
            Switch->Volt[4] = Voltage;


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
            if(Switch->Volt[1]==GND || Switch->Volt[4]==GND)
            {
                Switch->Volt[4]=VoltChange(Switch->PinId[4], 4, ComponentAddress, GND);
                Switch->Volt[1]=VoltChange(Switch->PinId[1],1,ComponentAddress,GND);
            }
            else
            {
                double tempvolt=max(VoltRequest(Switch->PinId[1], ComponentAddress),VoltRequest(Switch->PinId[4], ComponentAddress));
                Switch->Volt[4]=VoltChange(Switch->PinId[4],4,ComponentAddress,tempvolt);
                Switch->Volt[1]=VoltChange(Switch->PinId[1],1,ComponentAddress,tempvolt);
            }

        }
 return Switch->Volt[i];
}


void HandleDpdtEvent(void* ComponentAddress, int Event, BOOL SimulationStarted,
    void* ImageLocation, UINT ImageId, HWND* h)
{
    DpdtStruct* temp = (DpdtStruct*)ComponentAddress;

    if(SimulationStarted)
    {
        switch(Event)
        {
            case EVENT_MOUSE_CLICK:
            	DpdtStateChanged(temp,ImageLocation);
            	UpdateDpdtValues(ComponentAddress);
            	break;
         	default:
         		break;
        }
        
    }
    else
    {
        switch(Event){
            case EVENT_MOUSE_DBLCLICK:
            	ShowDpdtDialog(ComponentAddress);
                SetImage(temp->Image, ImageLocation);
                RefreshImages();
            	break;
            default:
            	break;
    }
    }
}

void DpdtStateChanged(DpdtStruct* DpdtData, void* ImageLocation)
{
    SetImage(DpdtData->Circuit ? DPDT_SWITCH_2 : DPDT_SWITCH_1,
            ImageLocation);
        RefreshImages();
}

double DpdtVoltChanged(void* DpdtData, BOOL SimulationStarted, int Index,
    double Volt, int Source, void* ImageLocation)
{

    // char Debug[256];
    DpdtStruct* temp = (DpdtStruct*)DpdtData;
    if(SimulationStarted)
    	return UpdateDpdtValues(DpdtData,Index);
    return Volt;
}


void SetDpdtIds(int* id,void* ComponentAddress)
{
    DpdtStruct* s = (DpdtStruct*)ComponentAddress;
    s->PinId[0] = *id++;
    s->PinId[1] = *id++;
    s->PinId[2] = *id++;
    s->PinId[3] = *id++;
    s->PinId[4] = *id++;
    s->PinId[5] = *id++;
    
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
