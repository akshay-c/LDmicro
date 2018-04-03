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
static HWND LatchedRadio;
static HWND Circuit1Radio;
static HWND Circuit2Radio;
static HWND NameTextbox;
static HWND InNameTextbox;
static HWND OutNameTextbox;
HWND* SpdtDialog;

void SpdtStateChanged(SpdtStruct* SpdtData, void* ImageLocation);
void MakeSpdtControls()
{
    HWND ActionGrouper = CreateWindowEx(0, WC_BUTTON, ("Action"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
        7, 3, 120, 65, *SpdtDialog, NULL, NULL, NULL);
    FontNice(ActionGrouper);

    LatchedRadio = CreateWindowEx(0, WC_BUTTON, ("Latched"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        16, 21, 100, 20, *SpdtDialog, NULL, NULL, NULL);
    FontNice(LatchedRadio);

    TemporaryRadio = CreateWindowEx(0, WC_BUTTON, ("Temporary"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 41, 100, 20, *SpdtDialog, NULL, NULL, NULL);
    FontNice(TemporaryRadio);

    // SetOnlyRadio = CreateWindowEx(0, WC_BUTTON, _("(S) Set-Only"),
    //     WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
    //     16, 61, 100, 20, CoilDialog, NULL, Instance, NULL);
    // NiceFont(SetOnlyRadio);

    // ResetOnlyRadio = CreateWindowEx(0, WC_BUTTON, _("(R) Reset-Only"),
    //     WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
    //     16, 81, 105, 20, CoilDialog, NULL, Instance, NULL);
    // NiceFont(ResetOnlyRadio);

    HWND PositionGrouper = CreateWindowEx(0, WC_BUTTON, ("Position"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        140, 3, 120, 65, *SpdtDialog, NULL, NULL, NULL);
    FontNice(PositionGrouper);

    Circuit1Radio = CreateWindowEx(0, WC_BUTTON, ("Circuit 1"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
        149, 21, 100, 20, *SpdtDialog, NULL, NULL, NULL);
    FontNice(Circuit1Radio);

    Circuit2Radio = CreateWindowEx(0, WC_BUTTON, ("Circuit 2"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
        149, 41, 100, 20, *SpdtDialog, NULL, NULL, NULL);
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
    if(Data->Latched)
    {
        Button_SetCheck(LatchedRadio, BST_CHECKED);
    }else
    {
        Button_SetCheck(TemporaryRadio, BST_CHECKED);
    }
    if(Data->NCircuit)
    {
        Button_SetCheck(Circuit1Radio, BST_CHECKED);
    }
    else
    {
        Button_SetCheck(Circuit2Radio, BST_CHECKED);
    }
    Edit_SetText(NameTextbox, Data->Name);
}


BOOL SaveSpdtDialog(SpdtStruct* Data)
{
    char temp[15];
    BOOL Latched, NCircuit;
    if(Button_GetState(LatchedRadio) == BST_CHECKED)
    {
        Latched = TRUE;
            
    }else if(Button_GetState(TemporaryRadio) == BST_CHECKED)
    {
        Latched = FALSE;
    }else
    {
        MessageBox(*SpdtDialog,
            ("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
        return FALSE;
    }

    if(Button_GetState(Circuit1Radio) == BST_CHECKED)
    {
        NCircuit = TRUE;
            
    }else if(Button_GetState(Circuit2Radio) == BST_CHECKED)
    {
        NCircuit = FALSE;
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
        Data->Latched = Latched;
        Data-> NCircuit = NCircuit;
        Data-> Circuit = NCircuit;
        strcpy(Data->Name, temp);
        if(NCircuit)
        {
            Data->Image =SPDT_SWITCH_1;
        }
        else
        {
            Data->Image =SPDT_SWITCH_2;
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
    temp->Latched = TRUE;
    temp->NCircuit = FALSE;
    temp->Circuit = FALSE;
    temp->Volt[0] = V_OPEN;
    temp->Volt[1] = V_OPEN;
    temp->Volt[2] = V_OPEN;

    return SPDT_SWITCH_1;
}


void UpdateValues(SpdtStruct* Switch, void* ComponentAddress)
{
    /*char Debug[256];
    sprintf_s(Debug, "Clicked: %p\n", ComponentAddress);
    OutputDebugString(Debug);*/
    if(Switch->Circuit)
        {
            Switch->Volt[0] =
                VoltChange(Switch->PinId[0], 0, ComponentAddress, V_OPEN);
            Switch->Volt[1] =
                VoltChange(Switch->PinId[1], 1, ComponentAddress, V_OPEN);
            Switch->Volt[2] =
                VoltChange(Switch->PinId[2], 2, ComponentAddress, V_OPEN);
            // char vx[3],vy[10];
            // _gcvt(Switch->Volt[0],8,vy);
            // _itoa(Switch->PinId[0],vx,10);
            // MessageBox(NULL,
            //    (vx),(vy), MB_OK | MB_ICONWARNING);
            // Switch->Volt[1] = GlobalVoltChange(Switch->PinId[1], ComponentAddress, V_OPEN);
            // MessageBox(NULL,
            //         "Open", "Test", MB_OK | MB_ICONWARNING);
        }
        else
        {
            double Voltage;
            Voltage = VoltRequest(Switch->PinId[0], ComponentAddress);
            Switch->Volt[0] = Voltage;
            Voltage = VoltRequest(Switch->PinId[1], ComponentAddress);
            Switch->Volt[1] = Voltage;
            Voltage = VoltRequest(Switch->PinId[2], ComponentAddress);
            Switch->Volt[2] = Voltage;

            if (Switch->Circuit==FALSE)
            {
         	   if(Switch->Volt[0] > Switch->Volt[1])
           		{
        	        /*sprintf_s(Debug, "Values2: Volt1:%f \t Volt2:%f \tAddr: %p \tPin0:%d \t Pin1:%d\n",
        			Switch->Volt[0], Switch->Volt[1], ComponentAddress, Switch->PinId[0], Switch->PinId[1]);
           	    	OutputDebugString(Debug);*/
               		Switch->Volt[0] = VoltChange(Switch->PinId[0], 0,
                    ComponentAddress, Switch->Volt[1]);
            	}
            	else if(Switch->Volt[1] > Switch->Volt[0])
            	{
              		/*sprintf_s(Debug, "Values3: Volt1:%f \t Volt2:%f \tAddr: %p \tPin0:%d \t Pin1:%d\n",
        			Switch->Volt[0], Switch->Volt[1], ComponentAddress, Switch->PinId[0], Switch->PinId[1]);
             		OutputDebugString(Debug);*/
                	Switch->Volt[1] = VoltChange(Switch->PinId[1], 1,
                    ComponentAddress, Switch->Volt[0]);
                
            	}
        	}
        	else
            {
         	   if(Switch->Volt[0] > Switch->Volt[2])
           		{
        	        /*sprintf_s(Debug, "Values2: Volt1:%f \t Volt2:%f \tAddr: %p \tPin0:%d \t Pin1:%d\n",
        			Switch->Volt[0], Switch->Volt[1], ComponentAddress, Switch->PinId[0], Switch->PinId[1]);
           	    	OutputDebugString(Debug);*/
               		Switch->Volt[0] = VoltChange(Switch->PinId[0], 0,
                    ComponentAddress, Switch->Volt[2]);
            	}
            	else if(Switch->Volt[2] > Switch->Volt[0])
            	{
              		/*sprintf_s(Debug, "Values3: Volt1:%f \t Volt2:%f \tAddr: %p \tPin0:%d \t Pin1:%d\n",
        			Switch->Volt[0], Switch->Volt[1], ComponentAddress, Switch->PinId[0], Switch->PinId[1]);
             		OutputDebugString(Debug);*/
                	Switch->Volt[2] = VoltChange(Switch->PinId[2], 1,
                    ComponentAddress, Switch->Volt[0]);
                
            	}
        	}
        }
        /*sprintf_s(Debug, "Values: Volt1:%f \t Volt2:%f \tAddr: %p \tPin0:%d \t Pin1:%d\n",
        Switch->Volt[0], Switch->Volt[1], ComponentAddress, Switch->PinId[0], Switch->PinId[1]);
    OutputDebugString(Debug);*/
}


void HandleSpdtEvent(void* ComponentAddress, int Event, BOOL SimulationStarted,
    void* ImageLocation, UINT ImageId, HWND* h)
{
    SpdtStruct *temp = (SpdtStruct*)ComponentAddress;

    if(SimulationStarted)
    {
        switch(Event){
            case EVENT_MOUSE_UP:
                if(temp->Latched)
                {
                    temp->Circuit = !temp->Circuit;
                    /*MessageBox(NULL,
                    "Latched", "Test", MB_OK | MB_ICONWARNING);*/
                }
                else
                {
                    temp->Circuit = temp->NCircuit;
                    // MessageBox(NULL,
                    // "Latched", "Test", MB_OK | MB_ICONWARNING);
                }
                SpdtStateChanged(temp, ImageLocation);
                UpdateValues(temp, ComponentAddress);
            break;
            case EVENT_MOUSE_DOWN:
                if(!temp->Latched)
                {
                    temp->Circuit = !temp->NCircuit;
                    SpdtStateChanged(temp, ImageLocation);
                    UpdateValues(temp, ComponentAddress);
                }
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
    }
    }
}

void SpdtStateChanged(SpdtStruct* SpdtData, void* ImageLocation)
{
    SetImage(SpdtData->Circuit ? SPDT_SWITCH_1 : SPDT_SWITCH_2,
            ImageLocation);
        RefreshImages();
}

double SpdtVoltChanged(void* SpdtData, BOOL SimulationStarted, int Index,
    double Volt, int Source, void* ImageLocation)
{
    // char Debug[256];
    SpdtStruct* temp = (SpdtStruct*)SpdtData;
    if(SimulationStarted)
    {
        double Voltage;
        Voltage = VoltRequest(temp->PinId[!Index], SpdtData);
        if(temp->Circuit)
        {
            temp->Volt[!Index] = VoltChange(temp->PinId[!Index], !Index, SpdtData, V_OPEN);
            temp->Volt[Index] = V_OPEN;
            /*sprintf_s(Debug, "SwitchVoltChanged: \tAddress: %p \tVolt0:%f \t Volt1:%f\n",
        SwitchData, Voltage, Volt);
    OutputDebugString(Debug);*/
            return temp->Volt[Index];
        }
        if(Voltage > Volt)
        {
            temp->Volt[Index] = Volt;
            temp->Volt[!Index] = VoltChange(temp->PinId[!Index], !Index, SpdtData, Volt);
            /*sprintf_s(Debug, "SwitchVoltChanged2: \tAddress: %p \tVolt0:%f \t Volt1:%f\n",
        SwitchData, Voltage, Volt);
    OutputDebugString(Debug);*/
            return temp->Volt[Index];
        }
        else
        {
            // temp->Volt[!Index] = VoltChange(temp->PinId[!Index], !Index, SwitchData, Volt);
            Voltage = VoltChange(temp->PinId[!Index], !Index, SpdtData, Volt);
            temp->Volt[Index] = Voltage;
            // VoltChange(temp->PinId[!Index], !Index, SwitchData, Volt);
            return Voltage;
        }
    }
    else
    {
        temp->Volt[Index] = Volt;
        return Volt;
    }
    return 0;
}


void SetSpdtIds(int* id,void* ComponentAddress)
{
    SpdtStruct *s = (SpdtStruct*)ComponentAddress;
    s->PinId[0] = *id++;
    s->PinId[1] = *id++;
    s->PinId[2] = *id;
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
