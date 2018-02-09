#ifndef _NAMING_LIST
#define _NAMING_LIST

extern HWND NamingList;

// void NamingListProc(NMHDR *h);

int NameList_AddName(LPCTSTR Name, int Index);
double NameList_RegisterVolt(LPCTSTR Name, double volt);       //Forcefully set voltage will not be accessible outside the cpp file
double NameList_SetVolt(int Index, double volt);            //Set voltage if current voltage>specified voltage
double NameList_DeRegisterVolt(LPCTSTR Name);                //Send a message to rest of the components to set new voltage
void   NamingListProc(NMHDR *h);


// void ConfigureNamingList(int type,int PinOnProcessor, MCUPort);
#endif
