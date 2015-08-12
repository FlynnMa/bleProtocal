#ifndef bicJni_h
#define bicJni_h

typedef struct sendDataTypes {
    uint8_t sendBytes[128];
    uint32_t  len;
} sendDataTypes;

typedef struct DeviceInfoType {
    char        name[32];
    char        version[4];
    char        copyRight[32];
    int         deviceID;
    int         mile;
    int         powerOnOff;
    int         chargerIn;
    int         driveMode;
    int         connection;
    float       speed;
    float       maxSpeed;
    float       voltage;
    float       maxVoltage;
    float       minVoltage;
    float       shutdownVoltage;
    float       fullVoltage;
    float       mainboardTemperiture;

   /* write section, data will write to device */
    char       ApkVersion[4];
} DeviceInfoType;

typedef struct EventType {
    int   event;
    int   evtType;  /* 1: query, 2: set, 3: ack*/

    union{
        int    iValue;
        float  fValue;
        char   sValue[128];
    };
} EventType;

typedef struct JniCtlType {
  int readPip;
  int writePip;
  volatile int isRunning;
} JniCtlType;




#endif
