#ifndef _ADVANCED_DIALOG_H
#define _ADVANCED_DIALOG_H

#define MAX_PIN_NAME                128

/*Advanced Dialog Menus*/
#define MNU_ADV_NEW                 0x01
#define MNU_ADV_OPEN                0x02
#define MNU_ADV_SAVE                0x03
#define MNU_ADV_SAVE_AS             0x04
#define MNU_ADV_EXIT                0x05

#define MNU_ADV_UNDO                0x10
#define MNU_ADV_REDO                0x11
#define MNU_ADV_CUT                 0x12
#define MNU_ADV_COPY                0x13
#define MNU_ADV_PASTE               0x14
#define MNU_ADV_DEL                 0x15

#define MNU_ADV_SIMULATION_MODE     0x20
#define MNU_ADV_START_SIMULATION    0x21
#define MNU_ADV_STOP_SIMULATION     0x22
#define MNU_ADV_SINGLE_CYCLE        0x23

#define MNU_ADV_MANUAL              0x30
#define MNU_ADV_ABOUT               0x31

#define MAX_NAME_LENGTH           128
#define MAX_SCREEN_ITEMS          512
#define MAX_PINS                  4000
#define MCU_PIN_FLAG              4000
#define MAX_MCU_PINS              128
#define TIMER_ADV_SIMULATE        101


typedef struct ImageStructTag {
    int         selectedState;
    HIMAGELIST  Images;
    int         ComponentId;
} ImageStruct;

typedef struct ImageLocationTag{
    int             Id;
    ImageStruct*    Image;
    int             Index;
    int             x;
    int             y;
    void*           Properties;
    void*           PinId;
    void*           PinName;
}ImageLocation;

typedef struct PinInfoTag{
    double          Volt;
    double          OperatingVolt;
    void**          ImageId;
    int*            Index;
    int*            ImageType;                         //To compare with imagelocation array
    int             LinkCount;                       //No of valid entries in array
    double          ProgVolt;
    void*           ProgComponent;
}PinInfo;

typedef struct PinMcuTag{
    UINT            PinId;
    UINT            state;
    int             type;
    BOOL            InternalPullup;
}PinMcu;

typedef struct PinNameTag{
    UINT            PinId;                           //Need to generate unique pinid every time user saves a name
    TCHAR           Name[MAX_NAME_LENGTH];
    PinInfo         PinData;
}PinName;

typedef struct PinComponentTag{
    UINT            PinId;
    void**          ComponentAddress;
    int             Count;
    void*           Next;
}PinComponent;

extern HANDLE           ImageHeap;
extern HFONT            AdvNiceFont;
extern HFONT            AdvFixedFont;
extern UINT             NameCount;
extern ImageLocation    ImageStack[MAX_SCREEN_ITEMS];
// extern PinInfo          PinData[MAX_PINS];
extern PinMcu           McuPin[MAX_MCU_PINS];
extern PinName          NameId[MAX_PINS];
extern HWND             AdvancedDialog;
extern PinComponent     ComponentPin;

/*Advanced Dialog Functions*/
void MakeAdvancedDialogControls(void);
void AdvancedDialogResized(void);
void MakeAdvancedWindowMenus(void);
void ProcessEvent(int x, int y, int Event);
void AdvancedWindowClosing(void);
void ToggleAdvancedSimulationMode(void);
void SimulateOneAdvCycle(BOOL ForceRefresh);
int IsMCUPin(int PinId);
// void CreateVoltRequest(int PinId, int Index, double VoltReq);

// Heap Functions
void* AllocImageHeap(size_t n);
void* ReallocImageHeap(LPVOID lpMem, size_t n);
void  FreeImageHeap(void *p);

// Component Functions

void InitComponents(void);

double GetGlobalVoltage(int PinId, void* ComponentAddress);
double RefreshVolt(int PinId, int Index, UINT Id, void* ComponentAddress, double volt);
double RefreshProcessorStat(int PinId, UINT Id);


int RegisterPinName(LPCTSTR Name);
int SetPinImage(int PinId,void* ImageId,int ImageType, int Index);
int FlushPinNames(void);                            //Clear Pins which are deleted from MainWindow
int DeRegisterPinName(LPCTSTR Name, void* ImageId);
int DeletePinImage(LPCTSTR Name, void* ImageId, int Index);
int DeletePinName(UINT Index);
void SetMcu(int PinId, int Type);
void RefreshNamingList(void);
void PopulateNamingList(void);

double RequestVoltChange(int PinId, int Index, void *ComponentAddress, double volt);

int DeleteComponentPin(int PinId, void* ComponentAddress);
int AddComponentPin(int PinId, void* ComponentAddress);
// int RegisterPinState(int Index, double Volt);

//NamingList functions
void ToggleInternalPullup(int PinId);


extern BOOL SimulationStarted;

#endif
