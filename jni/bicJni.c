/*
 * Copyright (C) 2015 bic Ltd. All rights reserved
 *
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <jni.h>

#include "trace.h"
#include "bicProcess.h"
#include "list.h"
#include "protocalApi.h"
#include "bicJni.h"
#include "protocal.h"

list_t  sendBuffer;
DeviceInfoType  devInfo;

JniCtlType jniCtl;


int32_t jniSendData(void *pData, uint32_t len)
{
    sendDataTypes *pBuffer = (sendDataTypes*)malloc(sizeof(sendDataTypes));

    memcpy(pBuffer->sendBytes, pData, len);
    pBuffer->len = len;
    listAdd(&sendBuffer, (uint32_t)pBuffer);
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_onDataRecv(
        JNIEnv* env, jobject thiz, jbyteArray jData)
{

    int i;
    jbyte *m_temp=(*env)->GetByteArrayElements(env, jData, 0);
    char *ch=(char*)m_temp;

    int len = (*env)->GetArrayLength(env, jData);

//    LOGD("Receive %d :", len);
    for (i = 0; i < len; i++)
    {
//        LOGD("recv char:%d", ch[i]);
        bicReceiveChar(ch[i]);
    }

    (*env)->ReleaseByteArrayElements(env, jData, m_temp, JNI_FALSE);
}

JNIEXPORT jbyteArray JNICALL
Java_com_vehicle_uart_DevMaster_getPackage(
        JNIEnv* env, jobject obj)
{
    // Array to fill with data
    jbyteArray *package;
    sendDataTypes *pData;

    if(0 != listGetAt(&sendBuffer, 0, (uint32_t*)&pData))
    {
        return NULL;
    }

    // Init  java byte array
    package = (*env)->NewByteArray(env, pData->len);

    (*env)->SetByteArrayRegion(env, package, 0, pData->len,
        (jbyte*)pData->sendBytes);

    listRemoveData(&sendBuffer, (uint32_t)pData);
    free(pData);

    return package;
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_init(
        JNIEnv *env, jobject obj)
{
    memset(&devInfo, 0, sizeof(devInfo));
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_update(
        JNIEnv *env, jobject obj)
{
    /* Create a corresponding file descriptor */
    jclass cls = (*env)->GetObjectClass(env, obj);

    /* dispatch stacked events */
    while(0 == bicProcessDispatchEvents());

    jfieldID nameID = (*env)->GetFieldID(env, cls,"name", "Ljava/lang/String;");
    jstring nameObj = (*env)->NewStringUTF(env, devInfo.name);
   (*env)->SetObjectField(env, obj, nameID, nameObj);
   (*env)->DeleteLocalRef(env, nameObj);

    jfieldID versionID = (*env)->GetFieldID(env, cls, "version", "[B");
    jbyteArray versionObj = (*env)->GetObjectField(env, obj, versionID);
    (*env)->SetByteArrayRegion(env, versionObj, 0, 4, (jbyte*)devInfo.version);
    (*env)->DeleteLocalRef(env, versionObj);

    jfieldID copyRightField = (*env)->GetFieldID(env, cls,
                                    "copyRight", "Ljava/lang/String;");
    if (copyRightField)
    {
        jstring  crObj = (*env)->NewStringUTF(env, devInfo.copyRight);
        (*env)->SetObjectField(env, obj, copyRightField, crObj);
        (*env)->DeleteLocalRef(env, crObj);
    }

    jfieldID deviceID = (*env)->GetFieldID(env, cls, "deviceID", "I");
    (*env)->SetIntField(env, obj, deviceID, devInfo.deviceID);

    jfieldID mileID = (*env)->GetFieldID(env, cls, "mile", "I");
    (*env)->SetIntField(env, obj, mileID, devInfo.mile);

    jfieldID powerOnOffID = (*env)->GetFieldID(env, cls, "powerOnOff", "I");
    (*env)->SetIntField(env, obj, powerOnOffID, devInfo.powerOnOff);

    jfieldID chargerInID = (*env)->GetFieldID(env, cls, "chargerIn", "I");
    (*env)->SetIntField(env, obj, chargerInID, devInfo.chargerIn);

    jfieldID driveModeID = (*env)->GetFieldID(env, cls, "driveMode", "I");
    (*env)->SetIntField(env, obj, driveModeID, devInfo.driveMode);

    jfieldID connectionID = (*env)->GetFieldID(env, cls, "connection", "I");
    (*env)->SetIntField(env, obj, connectionID, devInfo.connection);

    jfieldID speedID = (*env)->GetFieldID(env, cls,"speed","F");
    (*env)->SetFloatField(env, obj, speedID, devInfo.speed);

    jfieldID maxSpeedID = (*env)->GetFieldID(env, cls,"maxSpeed","F");
    (*env)->SetFloatField(env, obj, maxSpeedID, devInfo.maxSpeed);

    jfieldID voltageID = (*env)->GetFieldID(env, cls,"voltage","F");
    (*env)->SetFloatField(env, obj,voltageID,devInfo.voltage);

    jfieldID maxVoltageID = (*env)->GetFieldID(env, cls,"maxVoltage","F");
    (*env)->SetFloatField(env, obj, maxVoltageID,devInfo.maxVoltage);

    jfieldID minVoltageID = (*env)->GetFieldID(env, cls,"minVoltage","F");
    (*env)->SetFloatField(env, obj, minVoltageID,devInfo.minVoltage);

    jfieldID shutdownVoltageID = (*env)->GetFieldID(env, cls,"shutdownVoltage","F");
    (*env)->SetFloatField(env, obj, shutdownVoltageID,devInfo.shutdownVoltage);

    jfieldID fullVoltageID = (*env)->GetFieldID(env, cls,"fullVoltage","F");
    (*env)->SetFloatField(env, obj, fullVoltageID,devInfo.fullVoltage);

    jfieldID currentID = (*env)->GetFieldID(env, cls,"current","F");
    (*env)->SetFloatField(env, obj, currentID, devInfo.current);

    jfieldID mainboardTemperitureID = (*env)->GetFieldID(env,
                    cls,"mainboardTemperiture","F");
    (*env)->SetFloatField(env, obj, mainboardTemperitureID,
                    devInfo.mainboardTemperiture);

}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_readEvent(JNIEnv *env,
    jobject obj)
{
    int ret;
    EventType evt;

    ret = read(jniCtl.readPip, &evt, sizeof(EventType));
//    PWARN("jni read event:%d - %d", evt.event, evt.evtType);

     /* Create a corresponding file descriptor */
     jclass cls = (*env)->GetObjectClass(env, obj);

     jfieldID evtID = (*env)->GetFieldID(env, cls, "jniEvent", "I");
     (*env)->SetIntField(env, obj, evtID, evt.event);

    jfieldID evtTypeID = (*env)->GetFieldID(env, cls, "jniEventType", "I");
    (*env)->SetIntField(env, obj, evtTypeID, evt.evtType);

}

JNIEXPORT void JNICALL
    Java_com_vehicle_uart_DevMaster_query(
            JNIEnv *env, jobject obj, jint cmdID, jint dev)
{
//        PWARN("query cmd:%d from device:%d", cmdID, dev);
        protocalApiSetDevice(dev);
        protocalApiQuery(cmdID, NULL, 0);
}

JNIEXPORT void JNICALL
    Java_com_vehicle_uart_DevMaster_setInt(
            JNIEnv *env, jobject obj, jint cmdID, jint dev, jint data)
{
//    LOGW("set byte :%d", (uint8_t)data);
    protocalApiSetDevice(dev);
    protocalApiSetU8(cmdID, (uint8_t)data);
}


JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_querySpeed(
        JNIEnv *env, jobject obj)
{
    LOGD("query speed");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiQuery(CMD_ID_SPEED, NULL, 0);
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_queryMaxSpeed(
        JNIEnv *env, jobject obj)
{
    LOGD("query max speed");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiQuery(CMD_ID_MAX_SPEED, NULL, 0);
}


JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_queryVoltage(
        JNIEnv *env, jobject obj)
{
    LOGD("query voltage");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiQuery(CMD_ID_BATTERY_VOLTAGE, NULL, 0);
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_queryMaxVoltage(
        JNIEnv *env, jobject obj)
{
    LOGD("query maxVoltage");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiQuery(CMD_ID_FULL_BATTERY, NULL, 0);
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_queryLowVoltage(
        JNIEnv *env, jobject obj)
{
    LOGD("query low Voltage");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiQuery(CMD_ID_LOW_BATTERY, NULL, 0);
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_queryShutDownVoltage(
        JNIEnv *env, jobject obj)
{
    LOGD("query low Voltage");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiQuery(CMD_ID_SHUTDOWN_BATTERY, NULL, 0);
}


JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_queryVersion(
        JNIEnv *env, jobject obj)
{
    LOGD("query version");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiQuery(CMD_ID_FIRMWARE_VERSION, NULL, 0);
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_queryDeviceName(
        JNIEnv *env, jobject obj)
{
    LOGD("query device name");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiQuery(CMD_ID_DEVICE_NAME, NULL, 0);
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_queryDeviceID(
        JNIEnv *env, jobject obj)
{
    LOGD("query device id");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiQuery(CMD_ID_DEVICE_ID, NULL, 0);
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_queryBoardTemperature(
        JNIEnv *env, jobject obj)
{
    LOGD("query board temperature");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiQuery(CMD_ID_MAINBOARD_TEMPERITURE, NULL, 0);
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_queryMile(
        JNIEnv *env, jobject obj)
{
    LOGD("query speed");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiQuery(CMD_ID_MILE, NULL, 0);
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_queryPowerOnOff(
        JNIEnv *env, jobject obj)
{
    LOGD("query power on or off");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiQuery(CMD_ID_POWER_ONOFF, NULL, 0);
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_setPowerOnOff(
        JNIEnv *env, jobject obj, int onOff)
{
    LOGD("query power on or off");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiSetU8(CMD_ID_POWER_ONOFF, (uint8_t)onOff);
}


JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_queryDeviceMode(
        JNIEnv *env, jobject obj)
{
    LOGD("query power on or off");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiQuery(CMD_ID_DRIVE_MODE, NULL, 0);
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_setDeviceMode(
        JNIEnv *env, jobject obj, int devMode)
{
    LOGD("query power on or off");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiSetU8(CMD_ID_DRIVE_MODE, (uint8_t)devMode);
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_queryChargeStatus(
        JNIEnv *env, jobject obj)
{
    LOGD("query power on or off");
    protocalApiSetDevice(DEVICE_TYPE_EV);
    protocalApiQuery(CMD_ID_CHARGE_STATUS, NULL, 0);
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_setConnection(
        JNIEnv *env, jobject obj, int connect)
{
    LOGD("set connection status:%d", connect);
    protocalApiSetDevice(DEVICE_TYPE_BT);
    protocalApiSetU8(CMD_ID_DEVICE_CONNECTION, (uint8_t)connect);
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_getConnection(
        JNIEnv *env, jobject obj)
{
    LOGD("get connection status");
    protocalApiSetDevice(DEVICE_TYPE_BT);
    protocalApiQuery(CMD_ID_DEVICE_CONNECTION, NULL, 0);
}

void jni_send_event(int evtID, int evtType)
{
    EventType evt;

//    PERR("jni send event:%d - %d", evtID, evtType);
    evt.event = evtID;
    evt.evtType = evtType;
    write(jniCtl.writePip, &evt, sizeof(EventType));
}

JNIEXPORT void JNICALL
Java_com_vehicle_uart_DevMaster_sleepMs(
        JNIEnv *env, jobject obj)
{
    sleep(1000);
}



JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;
    int ret;
    int pip[2];

    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        LOGD("v1.4 not supported");
        return -1;
    }

    bicProcessInit();

    strcpy(devInfo.name, "shGeek technology");
    strcpy(devInfo.copyRight, "copy right");
    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

    memset(pip, 0, sizeof(pip));
    ret = pipe(pip);
    if (ret < 0)
    {
        devInfo.powerOnOff = 0;
        PERR("failed on initialize pip: %s\n", strerror(errno));
        return ret;
    }
    jniCtl.readPip = pip[0];
    fcntl(jniCtl.readPip, F_SETFL, 0);
    jniCtl.writePip = pip[1];

    return result;
}

