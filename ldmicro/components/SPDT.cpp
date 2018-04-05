#include <wincodec.h>
#include <stdio.h>
//#include <string.h>
#include <commctrl.h>
#include <Windowsx.h>

#include "componentstructs.h"
#include "componentfunctions.h"
#include "componentimages.h"
#include "components.h"

//static HWND TemporaryRadio;
static HWND LatchedRadio_spdt;
static HWND ClosedRadio_1_spdt;
static HWND ClosedRadio_2_spdt;
//static HWND OpenRadio;
static HWND NameTextbox_spdt;
static HWND InNameTextbox_spdt;
static HWND OutNameTextbox_spdt;
HWND* SPDTDialog;

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

	/*TemporaryRadio = CreateWindowEx(0, WC_BUTTON, ("Temporary"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
		16, 41, 100, 20, *SwitchDialog, NULL, NULL, NULL);
	FontNice(TemporaryRadio);*/

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

	ClosedRadio_1_spdt = CreateWindowEx(0, WC_BUTTON, ("Pin 1"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
		149, 21, 100, 20, *SPDTDialog, NULL, NULL, NULL);
	FontNice(ClosedRadio_1_spdt);

	ClosedRadio_2_spdt = CreateWindowEx(0, WC_BUTTON, ("Pin 2"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
		149, 41, 100, 20, *SPDTDialog, NULL, NULL, NULL);
	FontNice(ClosedRadio_2_spdt);

	/*NameTextbox_spdt = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
		WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
		155, 80, 155, 21, *SPDTDialog, NULL, NULL, NULL);
	FontFixed(NameTextbox_spdt);

	/*PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC,
	(LONG_PTR)MyNameProc);*/
}

void SPDTLoadState(SPDTStruct* Data)
{
	if (Data->latch_1)
	{
		Button_SetCheck(LatchedRadio_spdt, BST_CHECKED);
	}
	if (!Data->init_pos)
	{
		Button_SetCheck(ClosedRadio_2_spdt, BST_CHECKED);
	}
	else
	{
		Button_SetCheck(ClosedRadio_1_spdt, BST_CHECKED);
	}
	//Edit_SetText(NameTextbox_spdt, Data->Name);
}

BOOL SaveSPDTDialog(SPDTStruct* Data)
{
	BOOL latch_1, init_pos;
	if (Button_GetState(LatchedRadio_spdt) == BST_CHECKED)
	{
		latch_1 = TRUE;

	}
	else
	{
		MessageBox(*SPDTDialog,
			("Incomplete"), ("Warning"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}

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
	Data->latch_1 = latch_1;
	Data->init_pos = init_pos;
	//Data->Open = NOpen;
	if (init_pos)
	{
		Data->image = SPDT_switch_connected_1;
	}
	else
	{
		Data->image = SPDT_switch_connected_2;
	}
	return TRUE;
}

void ShowSPDTDialog(void* ComponentAddress)
{
	SPDTStruct* Data = (SPDTStruct*)ComponentAddress;
	BOOL Canceled, Complete = TRUE;
	SPDTDialog = CreateDialogWindow("SPDT Dialog", 100, 100, 263, 145, STYLE_VERTICAL);
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

void SPDTUpdateValues(SPDTStruct* spdt, void* ComponentAddress)
{
	SPDTStruct* temp = (SPDTStruct*)spdt;
	double v0, v1, v2;
	/*if (temp->latch_1 == TRUE)
	{
		temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, GND);
		temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, V_OPEN);
	}
	else
	{
		temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, V_OPEN);
		temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, GND);
	}*/
	if (temp->latch_1 == TRUE) 
	{
		v0 = VoltRequest(temp->pinId[0], ComponentAddress);
		v1 = VoltRequest(temp->pinId[1], ComponentAddress);
		if (v0 != GND && v1 != GND) {
			if (v0 != V_OPEN && v1 != V_OPEN) {
				if (v0 > v1) {
					//temp->volt[Index] = Volt;
					temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, v0);
					temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, V_OPEN);
					//temp->volt[Index + 2] = VoltChange(temp->pinId[Index + 2], Index + 2, spdtData, Volt);
					//return temp->volt[Index];
				}
				else {
					//Voltage = VoltChange(temp->pinId[Index + 1], Index + 1, spdtData, Volt);
					temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, v1);
					temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, V_OPEN);
					//return (temp->volt[Index]);
				}
			}
			else if (v1 == V_OPEN) {
				temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, v0);
				temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, V_OPEN);
				//return (temp->volt[Index]);
			}
			else if (v0 == V_OPEN) {
				temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, v1);
				temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, V_OPEN);
				//return (temp->volt[Index]);
			}
		}
		else {
			temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, GND);
			temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, GND);
			temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, V_OPEN);
			//return (temp->volt[Index]);
		}
	}
	else if (temp->latch_1 == FALSE) {
		v0 = VoltRequest(temp->pinId[0], ComponentAddress);
		v2 = VoltRequest(temp->pinId[2], ComponentAddress);
		if (v0 != GND && v2 != GND) {
			if (v0 != V_OPEN && v2 != V_OPEN) {
				if (v0 > v2) {
					//temp->volt[Index] = Volt;
					temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, v0);
					temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, V_OPEN);
					//temp->volt[Index + 2] = VoltChange(temp->pinId[Index + 2], Index + 2, spdtData, Volt);
					//return temp->volt[2];
				}
				else {
					//Voltage = VoltChange(temp->pinId[Index + 1], Index + 1, spdtData, Volt);
					temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, v2);
					temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, V_OPEN);
					//return (temp->volt[Index]);
				}
			}
			else if (v2 == V_OPEN) {
				temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, v0);
				temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, V_OPEN);
				//return (temp->volt[Index]);
			}
			else if (v0 == V_OPEN) {
				temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, v2);
				temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, V_OPEN);
				//return (temp->volt[Index]);
			}
		}
		else {
			temp->volt[0] = VoltChange(temp->pinId[0], 0, ComponentAddress, GND);
			temp->volt[2] = VoltChange(temp->pinId[2], 2, ComponentAddress, GND);
			temp->volt[1] = VoltChange(temp->pinId[1], 1, ComponentAddress, V_OPEN);
			//return (temp->volt[Index]);
		}
	}

	// Storing real time values in each pin
	/*double v0,v1,v2;
	v0 = VoltRequest(spdt->pinId[0], ComponentAddress);
	spdt->volt[0] = v0;
	v1 = VoltRequest(spdt->pinId[1], ComponentAddress);
	spdt->volt[1] = v1;
	v2 = VoltRequest(spdt->pinId[2], ComponentAddress);
	spdt->volt[2] = v2;
	if (spdt->latch_1 == TRUE) {

		if (spdt->volt[0] > spdt->volt[1])
		{
			spdt->volt[1] = VoltChange(spdt->pinId[0], 0,
				ComponentAddress, spdt->volt[0]);
		}
		else if (spdt->volt[1] > spdt->volt[0])
		{
			spdt->volt[0] = VoltChange(spdt->pinId[1], 1,
				ComponentAddress, spdt->volt[1]);
		}
	else if (spdt->latch_1 == FALSE) {
			if (spdt->volt[0] > spdt->volt[2])
			{
				spdt->volt[2] = VoltChange(spdt->pinId[0], 0,
				ComponentAddress, spdt->volt[0]);
			}
			else if (spdt->volt[2] > spdt->volt[0]){
				spdt->volt[0] = VoltChange(spdt->pinId[2], 1,
				ComponentAddress, spdt->volt[2]);
			}
		}
	}*/
}

void HandleSPDTEvent(void * ComponentAddress, int Event, BOOL SimulationStarted, void * ImageLocation, UINT ImageId, HWND * h)
{
	SPDTStruct *temp = (SPDTStruct*)ComponentAddress;

	if (SimulationStarted)
	{
		switch (Event) {
		case EVENT_MOUSE_CLICK:
			temp->latch_1 = !temp->latch_1;
			SetImage(temp->latch_1 ? SPDT_switch_connected_2 : SPDT_switch_connected_1,
				ImageLocation);
			RefreshImages();
			SPDTUpdateValues(temp, ComponentAddress);
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

double SPDTVoltChanged(void* spdtData, BOOL SimulationStarted, int Index,
	double Volt, int Source, void* ImageLocation) {
	// char Debug[256];
	SPDTStruct* temp = (SPDTStruct*)spdtData;
	if (SimulationStarted)
	{
		double v0, v1, v2;
		if (temp->latch_1 == TRUE) {
			v0 = VoltRequest(temp->pinId[0], spdtData);
			v1 = VoltRequest(temp->pinId[1], spdtData);
			if (v0 != GND && v1 != GND) {
				if (v0 != V_OPEN && v1 != V_OPEN) {
					if (v0 > v1) {
						//temp->volt[Index] = Volt;
						temp->volt[1] = VoltChange(temp->pinId[1], 1, spdtData, v0);
						//temp->volt[Index + 2] = VoltChange(temp->pinId[Index + 2], Index + 2, spdtData, Volt);
						return temp->volt[Index];
					}
					else {
						//Voltage = VoltChange(temp->pinId[Index + 1], Index + 1, spdtData, Volt);
						temp->volt[1] = VoltChange(temp->pinId[1], 1, spdtData, v0);
						return (temp->volt[Index]);
					}
				}
				else if (v1 == V_OPEN) {
					temp->volt[1] = VoltChange(temp->pinId[1], 1, spdtData, v0);
					return (temp->volt[Index]);
				}
				else if (v0 == V_OPEN) {
					temp->volt[0] = VoltChange(temp->pinId[0], 0, spdtData, v1);
					return (temp->volt[Index]);
				}
			}
			else {
				temp->volt[0] = VoltChange(temp->pinId[0], 0, spdtData, GND);
				temp->volt[1] = VoltChange(temp->pinId[1], 1, spdtData, GND);
				return (temp->volt[Index]);
			}
		}
		else if (temp->latch_1 == FALSE) {
			v0 = VoltRequest(temp->pinId[0], spdtData);
			v2 = VoltRequest(temp->pinId[2], spdtData);
			if (v0 != GND && v2 != GND) {
				if (v0 != V_OPEN && v2 != V_OPEN) {
					if (v0 > v2) {
						//temp->volt[Index] = Volt;
						temp->volt[2] = VoltChange(temp->pinId[2], 2, spdtData, v0);
						//temp->volt[Index + 2] = VoltChange(temp->pinId[Index + 2], Index + 2, spdtData, Volt);
						return temp->volt[2];
					}
					else {
						//Voltage = VoltChange(temp->pinId[Index + 1], Index + 1, spdtData, Volt);
						temp->volt[0] = VoltChange(temp->pinId[0], 0, spdtData, v2);
						return (temp->volt[Index]);
					}
				}
				else if (v2 == V_OPEN) {
					temp->volt[2] = VoltChange(temp->pinId[2], 2, spdtData, v0);
					return (temp->volt[Index]);
				}
				else if (v0 == V_OPEN) {
					temp->volt[0] = VoltChange(temp->pinId[0], 0, spdtData, v2);
					return (temp->volt[Index]);
				}
			}
			else {
				temp->volt[0] = VoltChange(temp->pinId[0], 0, spdtData, GND);
				temp->volt[2] = VoltChange(temp->pinId[2], 2, spdtData, GND);
				return (temp->volt[Index]);
			}
			/*temp->volt[0] = VoltChange(temp->pinId[0], 0, spdtData, GND);
					temp->volt[1] = VoltChange(temp->pinId[1], 1, spdtData, GND);
					return (temp->volt[0]);*/
					/*if (Index == 1) {
						Voltage = VoltRequest(temp->pinId[Index-1], spdtData);
						// Equalizing the voltage at pins
						if (Voltage > Volt) {
							//temp->volt[Index] = Volt;
							temp->volt[Index] = VoltChange(temp->pinId[Index-1], Index-1, spdtData, Voltage);
							//temp->volt[Index + 2] = VoltChange(temp->pinId[Index + 2], Index + 2, spdtData, Volt);
							return temp->volt[Index-1];
						}
						else if (Voltage < Volt) {
							//Voltage = VoltChange(temp->pinId[Index + 1], Index + 1, spdtData, Volt);
							temp->volt[Index-1] = Volt;
							return (temp->volt[Index]);
						}
						else if (Voltage == GND || Volt == GND) {
							temp->volt[Index] = VoltChange(temp->pinId[Index-1], Index-1, spdtData, GND);
							temp->volt[Index-1] = VoltChange(temp->pinId[Index-1], Index-1, spdtData, GND);
						}
						else if (Voltage )
					}
					else if (Index == 2) {
						Voltage = VoltRequest(temp->pinId[Index-2], spdtData);
						// Equalizing the voltage at pins
						if (Voltage > Volt) {
							//temp->volt[Index] = Volt;
							temp->volt[Index] = VoltChange(temp->pinId[Index-2], Index-2, spdtData, Voltage);
							//temp->volt[Index + 2] = VoltChange(temp->pinId[Index + 2], Index + 2, spdtData, Volt);
							return temp->volt[Index];
						}
						else {
							//Voltage = VoltChange(temp->pinId[Index + 1], Index + 1, spdtData, Volt);
							temp->volt[Index-2] = Volt;
							return (temp->volt[Index]);
						}
					}
					else {
						if (temp->latch_1 == TRUE) {
							Voltage = VoltRequest(temp->pinId[Index+1], spdtData);
							if (Voltage > Volt) {
								//temp->volt[Index] = Volt;
								temp->volt[Index] = VoltChange(temp->pinId[Index+1], Index+1, spdtData, Voltage);
								//temp->volt[Index + 2] = VoltChange(temp->pinId[Index + 2], Index + 2, spdtData, Volt);
								return temp->volt[Index];
							}
							else {
								//Voltage = VoltChange(temp->pinId[Index + 1], Index + 1, spdtData, Volt);
								temp->volt[Index+1] = Volt;
								return (temp->volt[Index]);
							}
						}
					}*/
		}
		else
		{
			temp->volt[Index] = Volt;
			return Volt;
		}
		return 0;
	}
}

int Init_SPDT(void * ComponentAddress)
{
	SPDTStruct* temp = (SPDTStruct*)ComponentAddress;
	temp->image = SPDT_switch_connected_1;
	temp->latch_1 = TRUE;
	//temp->latch_2 = FALSE;
	//temp->open = TRUE;
	temp->init_pos = TRUE;
	temp->volt[0] = V_OPEN;
	temp->volt[1] = V_OPEN;
	temp->volt[2] = V_OPEN;
	return SPDT_switch_connected_1;
}

void SetSPDTIds(int *id, void * ComponentAddress)
{
	SPDTStruct *s = (SPDTStruct*)ComponentAddress;
	s->pinId[0] = *id++;
	s->pinId[1] = *id++;
	s->pinId[2] = *id;
}