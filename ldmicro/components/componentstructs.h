#ifndef _COMPONENT_STRUCTS
#define _COMPONENT_STRUCTS

typedef struct SwitchStructTag
{
    int     id;
    int     Image;
    BOOL    Latched;                   //Temporary/Latched Action
    BOOL    NOpen;                     //Initial Open/Closed position
    BOOL    Open;
    char    Name[15];
    double  Volt[2];
    int     PinId[2];
}SwitchStruct;

typedef struct RelayStructTag
{
    int     id;
    int     Image;
    BOOL    NC;                        //Whether relay is operated
    double  MinOperatingVolt;             //Operating voltage
    double  MaxOperatingVolt;
    double  CoilVolt1;                 //Voltage at input pin
    double  CoilVolt2;                 //Voltage at input pin
    double  COMVolt;                   //Voltage at COM pin
    double  NOVolt;                    //Voltage at NO pin
    double  NCVolt;					   //Voltage at NC pin
    int     PinId[5];                   

}RelayStruct;

typedef struct SpdtStructTag
{
	int id;
	int image;
	BOOL latched;      //Whether the swetch is in latch mode or not
	BOOL NO1;          //Whether Output 1 is connected
	double Volt[3];    //Voltage at Input, Output1, Output2 respectively
	int PinId[3];
}SpdtStruct;

typedef struct DpstStructTag
{
	int id;
	int image;
	BOOL latched;      //Whether the swetch is in latch mode or not
	BOOL NO;          //Whether the inputs and outputs are disconnected (Open)
	double Volt[4];    // Voltage at Input1, Input2, Output1, Output2 respectively
	int PinId[4];
}DpstStruct;

typedef struct DpdtStructTag
{
	int id;
	int image;
	BOOL latched;      //Whether the swetch is in latch mode or not
	BOOL NS1;          //Whether the inputs and outputs are connected in state 1
	double Volt[6];    // Voltage at Input1, Input2, Output11, Output12, Output21, Output22 respectively
	int PinId[6];
}DpdtStruct;

#endif
