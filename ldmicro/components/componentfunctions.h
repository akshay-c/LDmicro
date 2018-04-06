#ifndef _COMPONENT_FUNCTIONS
#define _COMPONENT_FUNCTIONS

//Few Prerequisites
extern HFONT      MyNiceFont;
extern HFONT      MyFixedFont;
extern HWND       OkButton;
extern HWND       CancelButton;
extern HINSTANCE* ComponentInstance;

extern BOOL DlgDone;
extern BOOL DlgCancel;
extern HWND ComponentDialog;

// Common Functions

void FontNice(HWND h);
void FontFixed(HWND h);
HWND* CreateDialogWindow(LPCTSTR title, int x, int y, int width, int height, int style);
void ShowDialogWindow(void);
BOOL ProcessDialogWindow(void);

/*Initialization Functions*/
int InitSwitch(void* ComponentAddress);
int InitRelay(void* ComponentAddress);
int Init_SPDT(void* ComponentAddress);
int Init_DPST(void* CompponentAddress);
//int Init_DPDT(void* CompponentAddress);

/*Event Handlers*/
void HandleSwitchEvent(void* ComponentAddress, int Event, BOOL SimulationStarted,
    void* ImageLocation, UINT ImageId, HWND* h);
void HandleRelayEvent(void* ComponentAddress, int Event, BOOL SimulationStarted,
    void* ImageLocation, UINT ImageId, HWND* h);
void HandleSPDTEvent(void* ComponentAddress, int Event, BOOL SimulationStarted,
	void* ImageLocation, UINT ImageId, HWND* h);
void HandleDPSTEvent(void* ComponentAddress, int Event, BOOL SimulationStarted,
	void* ImageLocation, UINT ImageId, HWND* h);
/*void HandleDPDTEvent(void* ComponentAddress, int Event, BOOL SimulationStarted,
	void* ImageLocation, UINT ImageId, HWND* h);*/

/*Request Handlers*/
double SwitchVoltChanged(void* ComponentAddress, BOOL SimulationStarted, int index,
    double Volt, int Source, void* ImageLocation);
double RelayVoltChanged(void* ComponentAddress, BOOL SimulationStarted, int index,
    double Volt, int Source, void* ImageLocation);
double SPDTVoltChanged(void* ComponentAddress, BOOL SimulationStarted, int index,
	double Volt, int Source, void* ImageLocation);
double DPSTVoltChanged(void* ComponentAddress, BOOL SimulationStarted, int index,
	double Volt, int Source, void* ImageLocation);
/*double DPDTVoltChanged(void* ComponentAddress, BOOL SimulationStarted, int index,
	double Volt, int Source, void* ImageLocation);*/

/*Program Reference Functions*/
void SetSwitchIds(int*, void*);
void SetRelayIds(int*, void*);
void SetSPDTIds(int*, void*);
void SetDPSTIds(int*, void*);
//void SetDPDTIds(int*, void*);

// Relay Functions

#endif
