#include <wincodec.h>
#include <stdio.h>
//#include <string.h>
#include <commctrl.h>
#include <Windowsx.h>

#include "componentstructs.h"
#include "componentfunctions.h"
#include "componentimages.h"
#include "components.h"

//static HWND TemporaryRadio_spdt;
static HWND LatchedRadio_spdt;
static HWND ClosedRadio_1_spdt;
static HWND ClosedRadio_2_spdt;
//static HWND OpenRadio_spdt;
static HWND NameTextbox_spdt;
static HWND InNameTextbox_spdt;
static HWND OutNameTextbox_spdt;
HWND* SPDTDialog;

void SPDTStateChanged(SPDTStruct* SPDTData, void* ImageLocation);

// Making switch controls
void MakeSPDTControls()
{
	

	HWND ActionGrouper = CreateWindowEx(0, WC_BUTTON, ("Action"),
		WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
		7, 3, 120, 65, *SPDTDialog, NULL, NULL, NULL);
	FontNice(ActionGrouper);

	LatchedRadio_spdt = CreateWindowEx(0, WC_BUTTON, ("Latched"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
		16, 21, 100, 20, *SPDTDialog, NULL, NULL, NULL);
	FontNice(LatchedRadio_spdt);

	/*TemporaryRadio_spdt = CreateWindowEx(0, WC_BUTTON, ("Temporary"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
		16, 41, 100, 20, *SPDTDialog, NULL, NULL, NULL);
	FontNice(TemporaryRadio_spdt);*/
	
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
		140, 3, 120, 65, *SPDTDialog, NULL, NULL, NULL);
	FontNice(PositionGrouper);

	/*OpenRadio_spdt = CreateWindowEx(0, WC_BUTTON, ("Open"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
		149, 21, 100, 20, *SPDTDialog, NULL, NULL, NULL);
	FontNice(OpenRadio_spdt);*/

	
	ClosedRadio_1_spdt = CreateWindowEx(0, WC_BUTTON, ("Open"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
		149, 21, 100, 20, *SPDTDialog, NULL, NULL, NULL);
	FontNice(ClosedRadio_1_spdt);

	ClosedRadio_2_spdt = CreateWindowEx(0, WC_BUTTON, ("Closed"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
		149, 41, 100, 20, *SPDTDialog, NULL, NULL, NULL);
	FontNice(ClosedRadio_2_spdt);
	HWND textLabel = CreateWindowEx(0, WC_STATIC, ("Name:"),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
		100, 80, 50, 21, *SPDTDialog, NULL, NULL, NULL);
	FontNice(textLabel);

	NameTextbox_spdt = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
		WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
		155, 80, 155, 21, *SPDTDialog, NULL, NULL, NULL);
	FontFixed(NameTextbox_spdt);

	/*PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC,
	(LONG_PTR)MyNameProc);*/
}

void SPDTLoadState(SPDTStruct* Data)
{

		Button_SetCheck(LatchedRadio_spdt, BST_CHECKED);

	if (Data->init_pos)
	{
		Button_SetCheck(ClosedRadio_1_spdt, BST_CHECKED);
	}
	else
	{
		Button_SetCheck(ClosedRadio_2_spdt, BST_CHECKED);
	}
	//Edit_SetText(NameTextbox_spdt, Data->Name);
}

BOOL SaveSPDTDialog(SPDTStruct* Data)
{
	char temp[15];
	BOOL latch_1, latch_2, init_pos;

	// For pin 1
	if (Button_GetState(LatchedRadio_spdt) == BST_CHECKED)
	{
		if (Button_GetState(ClosedRadio_1_spdt) == BST_CHECKED)
		{
			init_pos = TRUE;
		}

		else if (Button_GetState(ClosedRadio_2_spdt) == BST_CHECKED)
		{
			init_pos = FALSE;
		}

		else
		{
			MessageBox(*SPDTDialog,
				("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
			return FALSE;
		}
	}
	
	/*else if (Button_GetState(TemporaryRadio_spdt) == BST_CHECKED)
	{
		latch_1 = FALSE;
	}
	else
	{
		MessageBox(*SPDTDialog,
			("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	
	// For pin 2
	if (Button_GetState(LatchedRadio_spdt) == BST_CHECKED)
	{
		latch_2 = TRUE;
	}
	/*else if (Button_GetState(TemporaryRadio_spdt) == BST_CHECKED)
	{
		latch_2 = FALSE;
	}
	else
	{
		MessageBox(*SPDTDialog,
			("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	*/
	
	if (Edit_GetText(NameTextbox_spdt, (LPSTR)&temp, 15) < 1)
	{
		MessageBox(*SPDTDialog,
			("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	else
	{
		Data->latch_1 = latch_1;
		Data->latch_2 = latch_2;
		Data->init_pos = init_pos;
		//Data->open = init_pos;
		strcpy(Data->name, temp);
		if (init_pos)
		{
			Data->image = SPDT_switch_connected_1;
		}
		else
		{
			Data->image = SPDT_switch_connected_2;
		}
	}
	return TRUE;
}

void ShowSPDTDialog(void* ComponentAddress)
{
	SPDTStruct* Data = (SPDTStruct*)ComponentAddress;
	BOOL Canceled, Complete = TRUE;
	SPDTDialog = CreateDialogWindow("Switch Dialog", 100, 100, 263, 145, STYLE_VERTICAL);
	MakeSPDTControls();
	ShowDialogWindow();
	SPDTLoadState(Data);
	Canceled = ProcessDialogWindow();
	while (Canceled == FALSE)
	{
		Complete = SaveSPDTDialog(Data);
		if (Complete == TRUE)
		{
			// MessageBox(*SwitchDialog,
			//     ("Saved"), ("Mouse click"), MB_OK | MB_ICONWARNING);
			break;
		}
		else
		{
			Complete = TRUE;
			Canceled = ProcessDialogWindow();
		}
	}
	DestroyWindow(*SPDTDialog);
}

int Init_SPDT(void* ComponentAddress)
{
	SPDTStruct* temp = (SPDTStruct*)ComponentAddress;
	temp->image = SPDT_switch_connected_1;
	temp->latch_1 = TRUE;
	temp->latch_2 = FALSE;
	//temp->Open = TRUE;
	temp->init_pos = TRUE;
	temp->volt[0] = V_OPEN;
	temp->volt[1] = V_OPEN;
	temp->volt[2] = V_OPEN;
	return SPDT_switch_connected_1;
}

void SPDTUpdateValues(SPDTStruct* spdt, void* ComponentAddress)
{
	/*char Debug[256];
	sprintf_s(Debug, "Clicked: %p\n", ComponentAddress);
	OutputDebugString(Debug);*/
	if (spdt->latch_1)
	/*{
		// Changing the initial state
		spdt->volt[0] =
			VoltRequest(spdt->pinId[0], ComponentAddress);
		spdt->volt[1] =
			VoltRequest(spdt->pinId[1], ComponentAddress);

		//spdt->volt[2] =
			//VoltChange(spdt->pinId[2], 2, ComponentAddress, VOLT_5);
		// char vx[3],vy[10];
		// _gcvt(Switch->Volt[0],8,vy);
		// _itoa(Switch->PinId[0],vx,10);
		// MessageBox(NULL,
		//    (vx),(vy), MB_OK | MB_ICONWARNING);
		// Switch->Volt[1] = GlobalVoltChange(Switch->PinId[1], ComponentAddress, V_OPEN);
		// MessageBox(NULL,
		//         "Open", "Test", MB_OK | MB_ICONWARNING);
	}
	else*/
	{
		// Storing real time values in each pin
		double Voltage;
		Voltage = VoltRequest(spdt->pinId[0], ComponentAddress);
		spdt->volt[0] = Voltage;
		Voltage = VoltRequest(spdt->pinId[1], ComponentAddress);
		spdt->volt[1] = Voltage;
		Voltage = VoltRequest(spdt->pinId[2], ComponentAddress);
		spdt->volt[2] = Voltage;
		if (spdt->volt[0] > spdt->volt[1])
		{
			/*sprintf_s(Debug, "Values2: Volt1:%f \t Volt2:%f \tAddr: %p \tPin0:%d \t Pin1:%d\n",
			Switch->Volt[0], Switch->Volt[1], ComponentAddress, Switch->PinId[0], Switch->PinId[1]);
			OutputDebugString(Debug);*/
			spdt->volt[0] = VoltChange(spdt->pinId[0], 0,
				ComponentAddress, spdt->volt[1]);
		}
		else if (spdt->volt[1] > spdt->volt[0])
		{
			/*sprintf_s(Debug, "Values3: Volt1:%f \t Volt2:%f \tAddr: %p \tPin0:%d \t Pin1:%d\n",
			Switch->Volt[0], Switch->Volt[1], ComponentAddress, Switch->PinId[0], Switch->PinId[1]);
			OutputDebugString(Debug);*/
			spdt->volt[1] = VoltChange(spdt->pinId[1], 1,
				ComponentAddress, spdt->volt[0]);
		}
	}
	/*sprintf_s(Debug, "Values: Volt1:%f \t Volt2:%f \tAddr: %p \tPin0:%d \t Pin1:%d\n",
	Switch->Volt[0], Switch->Volt[1], ComponentAddress, Switch->PinId[0], Switch->PinId[1]);
	OutputDebugString(Debug);*/
	else if (spdt->latch_2) {
		double Voltage;
		Voltage = VoltRequest(spdt->pinId[0], ComponentAddress);
		spdt->volt[0] = Voltage;
		Voltage = VoltRequest(spdt->pinId[1], ComponentAddress);
		spdt->volt[1] = Voltage;
		Voltage = VoltRequest(spdt->pinId[2], ComponentAddress);
		spdt->volt[2] = Voltage;
		if (spdt->volt[0] > spdt->volt[2])
		{
			/*sprintf_s(Debug, "Values2: Volt1:%f \t Volt2:%f \tAddr: %p \tPin0:%d \t Pin1:%d\n",
			Switch->Volt[0], Switch->Volt[1], ComponentAddress, Switch->PinId[0], Switch->PinId[1]);
			OutputDebugString(Debug);*/
			spdt->volt[0] = VoltChange(spdt->pinId[0], 0,
				ComponentAddress, spdt->volt[2]);
		}
		else if (spdt->volt[2] > spdt->volt[0])
		{
			/*sprintf_s(Debug, "Values3: Volt1:%f \t Volt2:%f \tAddr: %p \tPin0:%d \t Pin1:%d\n",
			Switch->Volt[0], Switch->Volt[1], ComponentAddress, Switch->PinId[0], Switch->PinId[1]);
			OutputDebugString(Debug);*/
			spdt->volt[2] = VoltChange(spdt->pinId[2], 2,
				ComponentAddress, spdt->volt[0]);
		}
	}
}

void HandleSPDTEvent(void* ComponentAddress, int Event, BOOL SimulationStarted,
	void* ImageLocation, UINT ImageId, HWND* h)
{
	SPDTStruct *temp = (SPDTStruct*)ComponentAddress;

	if (SimulationStarted)
	{
		switch (Event) {
		case EVENT_MOUSE_UP:
			//if (temp->latch_1)
			//{
				temp->latch_1 = !temp->latch_1;
				temp->latch_2 = !temp->latch_2;
			/*	/*MessageBox(NULL,
				"Latched", "Test", MB_OK | MB_ICONWARNING);
			}
			else
			{
				temp->latch_1 = !temp->latch_1;
				temp->latch_2 = !temp->latch_2;
				// MessageBox(NULL,
				// "Latched", "Test", MB_OK | MB_ICONWARNING);
			}*/
			SPDTStateChanged(temp, ImageLocation);
			SPDTUpdateValues(temp, ComponentAddress);
			break;
		case EVENT_MOUSE_DOWN:
			/*if (!temp->latch_2)
			{
				temp->latch_2 = !temp->init_pos;
				SPDTStateChanged(temp, ImageLocation);
				SPDTUpdateValues(temp, ComponentAddress);
			}*/
			break;
		}

	}
	else
	{
		switch (Event) {
		case EVENT_MOUSE_DBLCLICK:
			ShowSPDTDialog(ComponentAddress);
			SetImage(temp->image, ImageLocation);
			RefreshImages();
			break;
		}
	}
}

void SPDTStateChanged(SPDTStruct* spdtData, void* ImageLocation)
{
	if (spdtData->latch_1 == TRUE) {
		SetImage(spdtData->latch_1 ? SPDT_switch_connected_2 : SPDT_switch_connected_1,
			ImageLocation);
		RefreshImages();
	}
	else if (spdtData->latch_2 == TRUE) {
		SetImage(spdtData->latch_2 ? SPDT_switch_connected_1 : SPDT_switch_connected_2,
			ImageLocation);
		RefreshImages();
	}

}

/*double SPDTVoltChanged(void* spdtData, BOOL SimulationStarted, int Index,
	double Volt, int Source, void* ImageLocation)
{
	// char Debug[256];
	SPDTStruct* temp = (SPDTStruct*)spdtData;
	if (SimulationStarted)
	{
		double Voltage;
		Voltage = VoltRequest(temp->pinId[!Index], spdtData);
		if (temp->latch_1) {
			if (Index == 0)
			{
				temp->volt[Index] = VoltChange(temp->pinId[Index], Index, spdtData, Volt);
				temp->volt[Index + 1] = VoltChange(temp->pinId[Index - 1], Index - 1, spdtData, Volt);
				temp->volt[Index + 2] = V_OPEN;
				/*sprintf_s(Debug, "SwitchVoltChanged: \tAddress: %p \tVolt0:%f \t Volt1:%f\n",
				SwitchData, Voltage, Volt);
				OutputDebugString(Debug);

				if (Voltage > Volt)
				{
					temp->volt[Index] = VoltChange(temp->pinId[Index], Index, spdtData, Volt);
					temp->volt[Index + 1] = VoltChange(temp->pinId[Index + 1], Index + 1, spdtData, Volt);
					temp->volt[Index + 2] = VoltChange(temp->pinId[Index + 1], Index + 1, spdtData, V_OPEN);
					/*sprintf_s(Debug, "SwitchVoltChanged2: \tAddress: %p \tVolt0:%f \t Volt1:%f\n",
					SwitchData, Voltage, Volt);
					OutputDebugString(Debug);
					return temp->volt[Index];
				}
				return temp->volt[Index];
			}
			else if (Index == 1)
			{
				temp->volt[Index] = VoltChange(temp->pinId[Index - 1], Index - 1, spdtData, Volt);
				temp->volt[Index + 1] = VoltChange(temp->pinId[Index], Index, spdtData, Volt);
				temp->volt[Index + 2] = V_OPEN;
				/*sprintf_s(Debug, "SwitchVoltChanged: \tAddress: %p \tVolt0:%f \t Volt1:%f\n",
				SwitchData, Voltage, Volt);
				OutputDebugString(Debug);

				if (Voltage > Volt)
				{
					temp->volt[Index] = VoltChange(temp->pinId[Index], Index, spdtData, Volt);
					temp->volt[Index + 1] = VoltChange(temp->pinId[Index + 1], Index + 1, spdtData, Volt);
					temp->volt[Index + 2] = VoltChange(temp->pinId[Index + 1], Index + 1, spdtData, V_OPEN);
					/*sprintf_s(Debug, "SwitchVoltChanged2: \tAddress: %p \tVolt0:%f \t Volt1:%f\n",
					SwitchData, Voltage, Volt);
					OutputDebugString(Debug);
					return temp->volt[Index];
				}
				return temp->volt[Index];
			}
			else if (Index == 2)
			{
				temp->volt[Index] = VoltChange(temp->pinId[Index - 2], Index - 2, spdtData, Volt);
				temp->volt[Index + 1] = VoltChange(temp->pinId[Index - 1], Index - 1, spdtData, Volt);
				temp->volt[Index + 2] = V_OPEN;
				/*sprintf_s(Debug, "SwitchVoltChanged: \tAddress: %p \tVolt0:%f \t Volt1:%f\n",
				SwitchData, Voltage, Volt);
				OutputDebugString(Debug);

				if (Voltage > Volt)
				{
					temp->volt[Index] = VoltChange(temp->pinId[Index], Index, spdtData, Volt);
					temp->volt[Index + 1] = VoltChange(temp->pinId[Index + 1], Index + 1, spdtData, Volt);
					temp->volt[Index + 2] = VoltChange(temp->pinId[Index + 1], Index + 1, spdtData, V_OPEN);
					/*sprintf_s(Debug, "SwitchVoltChanged2: \tAddress: %p \tVolt0:%f \t Volt1:%f\n",
					SwitchData, Voltage, Volt);
					OutputDebugString(Debug);
					return temp->volt[Index];
				}
				return temp->volt[Index];
			}
		}
		else if (temp->latch_2) {
			if (Index == 0) {
				// temp->Volt[!Index] = VoltChange(temp->PinId[!Index], !Index, SwitchData, Volt);

				//temp->volt[0] = VoltChange(temp->pinId[!Index-1], Index-1, spdtData, V_OPEN);
				Voltage = VoltChange(temp->pinId[Index], Index, spdtData, Volt);
				temp->volt[Index] = Voltage;

				// VoltChange(temp->PinId[!Index], !Index, SwitchData, Volt);
				return Voltage;
			}
		}
		else
		{
			temp->volt[Index] = Volt;
			return Volt;
		}
	}
	return 0;
}*/
	
double SwitchVoltChanged(void* spdtData, BOOL SimulationStarted, int Index,
	double Volt, int Source, void* ImageLocation)
{
	SPDTStruct* temp = (SPDTStruct*)spdtData;
if (SimulationStarted)
{
	double Voltage;
	Voltage = VoltRequest(temp->pinId[!Index], spdtData);
	if (temp->latch_1)
	{
		temp->volt[!Index] = VoltChange(temp->pinId[!Index], !Index, spdtData, V_OPEN);
		temp->volt[Index] = V_OPEN;
		/*sprintf_s(Debug, "SwitchVoltChanged: \tAddress: %p \tVolt0:%f \t Volt1:%f\n",
		SwitchData, Voltage, Volt);
		OutputDebugString(Debug);*/
		return temp->volt[Index];
	}
	if (Voltage > Volt)
	{
		temp->volt[Index] = Volt;
		temp->volt[!Index] = VoltChange(temp->pinId[!Index], !Index, spdtData, Volt);
		/*sprintf_s(Debug, "SwitchVoltChanged2: \tAddress: %p \tVolt0:%f \t Volt1:%f\n",
		SwitchData, Voltage, Volt);
		OutputDebugString(Debug);*/
		return temp->volt[Index];
	}
	else
	{
		// temp->Volt[!Index] = VoltChange(temp->PinId[!Index], !Index, SwitchData, Volt);
		Voltage = VoltChange(temp->pinId[!Index], !Index, spdtData, Volt);
		temp->volt[Index] = Voltage;
		// VoltChange(temp->PinId[!Index], !Index, SwitchData, Volt);
		return Voltage;
	}
}
else
{
	temp->volt[Index] = Volt;
	return Volt;
}
return 0;
}

// Setting pin Ids for each pin
void SetSPDTIds(int* id, void* ComponentAddress)
{
	SPDTStruct *s = (SPDTStruct*)ComponentAddress;
	s->pinId[0] = *id++;
	s->pinId[1] = *id++;
	s->pinId[2] = *id;
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