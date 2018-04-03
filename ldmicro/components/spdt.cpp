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
HWND* SpdtDialog;


void SpdtStateChanged(SpdtStruct* SpdtData, void* ImageLocation);
void MakeSpdtControls()
{
    HWND ActionGrouper = CreateWindowEx(0, WC_BUTTON, ("Default Shorted Circuit"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
        7, 3, 120, 65, *SpdtDialog, NULL, NULL, NULL);
    FontNice(ActionGrouper);

    Circuit1Radio = CreateWindowEx(0, WC_BUTTON, ("Circuit 1"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        16, 21, 100, 20, *SpdtDialog, NULL, NULL, NULL);
    FontNice(Circuit1Radio);

    Circuit2Radio = CreateWindowEx(0, WC_BUTTON, ("Circuit 2"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 41, 100, 20, *SpdtDialog, NULL, NULL, NULL);
    FontNice(Circuit2Radio);

    HWND textLabel = CreateWindowEx(0, WC_STATIC, ("Name:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        100, 80, 50, 21, *SpdtDialog, NULL, NULL, NULL);
    FontNice(textLabel);

    NameTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 80, 155, 21, *SpdtDialog, NULL, NULL, NULL);
    FontFixed(NameTextbox);

    /*PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNameProc);*/
}

void LoadState(SpdtStruct* Data)
{
    if(Data->Circuit)
    {
        Button_SetCheck(Circuit2Radio, BST_CHECKED);
    }else
    {
        Button_SetCheck(Circuit1Radio, BST_CHECKED);
    }
}


BOOL SaveSpdtDialog(SpdtStruct* Data)
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
        MessageBox(*SpdtDialog,
            ("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
        return FALSE;
    }

    if(Edit_GetText(NameTextbox, (LPSTR)&temp, 15) < 1)
    {
        MessageBox(*SpdtDialog,
            ("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
        return FALSE;
    }else
    {
        Data-> Circuit = Circuit;
        strcpy(Data->Name, temp);
        if(Circuit)
        {
            Data->Image =SPDT_SWITCH_2;
        }
        else
        {
            Data->Image =SPDT_SWITCH_1;
        }
    }
    return TRUE;
}

void ShowSpdtDialog(void* ComponentAddress)
{
    SpdtStruct* Data = (SpdtStruct*) ComponentAddress;
    BOOL Canceled, Complete = TRUE;
    SpdtDialog = CreateDialogWindow("Spdt Dialog", 100, 100, 263, 145, STYLE_VERTICAL);
    MakeSpdtControls();
    ShowDialogWindow();
    LoadState(Data);
    Canceled = ProcessDialogWindow();
    while(Canceled == FALSE)
    {
        Complete = SaveSpdtDialog(Data);
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
    DestroyWindow(*SpdtDialog);
}


int InitSpdt(void* ComponentAddress)
{
    SpdtStruct* temp = (SpdtStruct*)ComponentAddress;
    temp->Image = SPDT_SWITCH_1;
    temp->Circuit = FALSE;
    temp->Volt[0] = V_OPEN;
    temp->Volt[1] = V_OPEN;
    temp->Volt[2] = V_OPEN;

    return SPDT_SWITCH_1;
}


double UpdateValues(void* ComponentAddress, int i=0)
{
    /*char Debug[256];
    sprintf_s(Debug, "Clicked: %p\n", ComponentAddress);
    OutputDebugString(Debug);*/
	SpdtStruct* Switch = (SpdtStruct*)ComponentAddress;

    if(Switch->Circuit)
        {
            Switch->Volt[1] =
                VoltChange(Switch->PinId[1], 1, ComponentAddress, V_OPEN);

            double Voltage;
            Voltage = VoltRequest(Switch->PinId[0], ComponentAddress);
            Switch->Volt[0] = Voltage;
            Voltage = VoltRequest(Switch->PinId[2], ComponentAddress);
            Switch->Volt[2] = Voltage;


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
        }

    else
    	{
    		Switch->Volt[2] =
                VoltChange(Switch->PinId[2], 2, Switch, V_OPEN);

            double Voltage;
            Voltage = VoltRequest(Switch->PinId[0], Switch);
            Switch->Volt[0] = Voltage;
            Voltage = VoltRequest(Switch->PinId[1], Switch);
            Switch->Volt[1] = Voltage;


            if(Switch->Volt[0]==GND || Switch->Volt[1]==GND)
            {
            	Switch->Volt[1]=VoltChange(Switch->PinId[1], 1, ComponentAddress, GND);
            	Switch->Volt[0]=VoltChange(Switch->PinId[0],0,ComponentAddress,GND);
            }
            else
            {
            double tempvolt=max(VoltRequest(Switch->PinId[0], ComponentAddress),VoltRequest(Switch->PinId[1], ComponentAddress));
            	Switch->Volt[1]=VoltChange(Switch->PinId[1],1,ComponentAddress,tempvolt);
            	Switch->Volt[0]=VoltChange(Switch->PinId[0],0,ComponentAddress,tempvolt);
            }
	    }    	
 return Switch->Volt[i];
}


void HandleSpdtEvent(void* ComponentAddress, int Event, BOOL SimulationStarted,
    void* ImageLocation, UINT ImageId, HWND* h)
{
    SpdtStruct* temp = (SpdtStruct*)ComponentAddress;

    if(SimulationStarted)
    {
        switch(Event)
        {
            case EVENT_MOUSE_CLICK:
            	SpdtStateChanged(temp,ImageLocation);
            	UpdateValues(ComponentAddress);
            	break;
         	default:
         		break;
        }
        
    }
    else
    {
        switch(Event){
            case EVENT_MOUSE_DBLCLICK:
            	ShowSpdtDialog(ComponentAddress);
                SetImage(temp->Image, ImageLocation);
                RefreshImages();
            	break;
            default:
            	break;
    }
    }
}

void SpdtStateChanged(SpdtStruct* SpdtData, void* ImageLocation)
{
    SetImage(SpdtData->Circuit ? SPDT_SWITCH_2 : SPDT_SWITCH_1,
            ImageLocation);
        RefreshImages();
}

double SpdtVoltChanged(void* SpdtData, BOOL SimulationStarted, int Index,
    double Volt, int Source, void* ImageLocation)
{

    // char Debug[256];
    SpdtStruct* temp = (SpdtStruct*)SpdtData;
    if(SimulationStarted)
    	return UpdateValues(SpdtData,Index);
    return Volt;
}


void SetSpdtIds(int* id,void* ComponentAddress)
{
    SpdtStruct* s = (SpdtStruct*)ComponentAddress;
    s->PinId[0] = *id++;
    s->PinId[1] = *id++;
    s->PinId[2] = *id++;
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
