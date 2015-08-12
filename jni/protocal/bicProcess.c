#include "protocal.h"
#include "protocalApi.h"
#include "bicProcess.h"
#include "list.h"
#include "bicJni.h"
#include "trace.h"

static const uint8_t version[4] = {PROTOCAL_VERSION_MAJOR,
        PROTOCAL_VERSION_SUB, PROTOCAL_VERSION_MODIFY, PROTOCAL_VERSION_NUM};

static const char copyright[] = "Ma Yunfei @ 2015";

static const char date[] = __DATE__;

extern DeviceInfoType  devInfo;


void bleSChedule(void)
{
}
extern int32_t jniSendData(void *pData, uint32_t len);

static void waitEmpty(void)
{}

extern void jni_send_event(int evtID, int evtType);



/*
 * initialize protocal stack
 *
 * @param none
 *
 * @return @ERROR_TYPE
 */
int bicProcessInit(void)
{
/* to be implemented */
    return protocalApiInit(jniSendData, bicProcessCmd,
        bicProcessAck, waitEmpty, bleSChedule);
}

static int32_t onSet(uint8_t cmd, uint8_t *pData,
    uint8_t len)
{
    switch(cmd)
    {
        case CMD_ID_DEVICE_ID:
            devInfo.deviceID = *(uint32_t*)pData;
            break;

        case CMD_ID_DEVICE_NAME:
            memset(devInfo.name, 0, sizeof(devInfo.name));
            memcpy(devInfo.name, pData, len);
            break;

        case CMD_ID_FIRMWARE_VERSION:
             if (len == 4)
                 memcpy(devInfo.version, pData, len);
              break;

        case CMD_ID_MAINBOARD_TEMPERITURE:
             memcpy((void*)&devInfo.mainboardTemperiture, pData, 4);
             break;

        case CMD_ID_SPEED:
             memcpy((void*)&devInfo.speed, pData, 4);
             break;

        case CMD_ID_MAX_SPEED:
             memcpy((void*)&devInfo.maxSpeed, pData, 4);
             break;

        case CMD_ID_BATTERY_VOLTAGE:
             memcpy((void*)&devInfo.voltage, pData, 4);
             break;

        case CMD_ID_LOW_BATTERY:
             memcpy((void*)&devInfo.minVoltage, pData, 4);
             break;

        case CMD_ID_FULL_BATTERY:
             memcpy((void*)&devInfo.fullVoltage, pData, 4);
             break;

        case CMD_ID_SHUTDOWN_BATTERY:
             memcpy((void*)&devInfo.shutdownVoltage, pData, 4);
             break;

        case CMD_ID_CHARGE_STATUS:
             devInfo.chargerIn = (int)(*pData);
             break;

        case CMD_ID_MILE:
             memcpy((void*)&devInfo.mile, pData, 4);
             break;

        case CMD_ID_PROTOCAL_COPYRIGHT:
             memset(devInfo.copyRight, 0, len);
             memcpy((void*)devInfo.copyRight, pData, len);
             break;

        case CMD_ID_POWER_ONOFF:
             devInfo.powerOnOff = (int)(*pData);
             break;

        case CMD_ID_DRIVE_MODE:
             devInfo.driveMode = (int)(*pData);
             break;

        case CMD_ID_DEVICE_CONNECTION:
            devInfo.connection = (int)(*pData);
            break;

        default:
             break;
        }

    return 0;
}


static int32_t onQuery(uint8_t cmd,
                    uint8_t *pData, uint8_t len)
{
    int32_t ret = 0;
    uint8_t size, idx;
    uint8_t buff[24];

    switch(cmd)
    {
        case CMD_ID_PROTOCAL_VERSION:
            protocalApiSetU32(cmd, *(uint32_t*)version);
            ret = 1;
            break;

        case CMD_ID_PROTOCAL_COPYRIGHT:
            protocalApiSetStr(cmd, copyright);
            ret = 1;
            break;

        case CMD_ID_PROTOCAL_DATETIME:
            protocalApiSetStr(cmd, date);
            ret = 1;
            break;

        default:
            break;
    }
    return ret;
}

/*
 * @brief Do the protocal level command handler
 * Note: this function does hiden process in embeded target, mobile phone can
 * get those information via commands to identify it.
 *
 * @param[i] cmdType    command type
 * @param[i] cmd        command id
 * @param[i] pData      data to be handled
 * @param[i] len        length of pData context
 *
 * @return
 *  0 : continue process
 *  1 : this is an internal command, no needer further process
 */
int32_t bicProcessCmd(uint8_t cmdType,
            uint8_t cmd, uint8_t *pData, uint8_t len)
{
    uint32_t ret = 0;

    PERR("type:%d, cmd:%d, len:%d", cmdType, cmd, len);
    switch(cmdType)
    {
        case CMD_TYPE_ACK:
            break;

        case CMD_TYPE_QUERY:
            ret = onQuery(cmd, pData, len);
            break;

        case CMD_TYPE_SET:
            ret = onSet(cmd, pData, len);
            break;

        default:
            break;
    }

    jni_send_event((int)cmd, (int)cmdType);
    return ret;
}

/*!
 * process response the acknowledge
 *
 * @param[i] cmd     command id
 * @param[i] result  error code
 *
 * @return none
 */
void bicProcessAck(uint8_t cmd, uint8_t result)
{
    if (result == ERROR_CHECKSUM)
    {
        protocalApiResendLastPackage();
    }
}

/*!
 * Dispatch events for high level processing
 *
 * @param none
 *
 * @return ERROR_TYPE
 */
int32_t bicProcessDispatchEvents(void)
{
    return protocalApiDispatchEvents();
}

/*!
 * This function response characters when data is received, data will be
 * transferred into low level protocal for further analysis.
 * Notice: this function must be invoked in an individual thread.
 * Make sure it will not be blocked by other tasks
 *
 * @param[in] ch
 *
 * @return none
 */
int32_t bicReceiveChar(uint8_t ch)
{
    return protocalApiReceiveChar(ch);
}


