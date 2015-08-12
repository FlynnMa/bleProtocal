#ifndef protocalInternal_h
#define protocalInternal_h

#define PROTOCAL_VERSION_MAJOR     0

#define PROTOCAL_VERSION_SUB       1

#define PROTOCAL_VERSION_MODIFY    0

#define PROTOCAL_VERSION_NUM       0

/*
 * ========================================================
 * custom command ids supported by bluetooth module protocal
 * each command should follow its formate definition, and the data must
 * strictly limited in the length range
 * =======================================================
*/
/*! save or load record, each record with maximum 20 bytes
 *  query param : recordID 1byte + lengthToQuery1byte
 *  set   param : recordID 1byte + data nB(n<=16)
 */
#define BIC_CMD_ID_RECORD                   1

/*
 * ========================================================
 * custom command ids supported by onelution electronic vehicle protocal
 * each command should follow its formate definition, and the data must
 * strictly limited in the length range
 * =======================================================
*/
#define CMD_ID_DEVICE_ID                      0 /**< 4byte device ID
                                                    in binary formate */

#define CMD_ID_DEVICE_NAME                    1 /**< less than 24bytes
                                                device name in ASCII formate */

#define CMD_ID_FIRMWARE_VERSION               2 /**< 4byte firmware version
                                                in binary formate :
                                                first:  major
                                                second: min
                                                third:  modify
                                                fourth: reserved */

#define CMD_ID_MAINBOARD_TEMPERITURE          3 /**< 4byte temperiture float
                                                in binary formate */

#define CMD_ID_BATTERY_VOLTAGE                4 /**< 4bytes voltage float
                                                in binary formate */

#define CMD_ID_CHARGE_STATUS                  5 /**< 1byte charging status
                                                  0 : no charging
                                                  1 : charging */

#define CMD_ID_SPEED                          6 /**< 4bytes speed float in
                                                binary formate */

#define CMD_ID_MILE                           7 /**< 4bytes unsigned int */

#define CMD_ID_MAX_SPEED                      8 /**< 4bytes unsigned int */

#define CMD_ID_LOW_BATTERY                    9 /**< 4bytes float in binary */

#define CMD_ID_SHUTDOWN_BATTERY               10 /**< 4bytes float in binary */

#define CMD_ID_FULL_BATTERY                   11 /**< 4bytes float in binary */

#define CMD_ID_POWER_ONOFF                    12 /**< 1 byte power onoff
                                                        status */

#define CMD_ID_DRIVE_MODE                     13 /**< 1 byte drive mode */


#ifdef __cplusplus
extern "C" {
#endif

/*
 * initialize protocal stack
 *
 * @param none
 *
 * @return @ERROR_TYPE
 */
int bicProcessInit(void);

/*
 * @brief Do the protocal level command handler
 *
 * @param[i] pProtocal  instance of ProtocalStatusType
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
                            uint8_t cmd, uint8_t *pData, uint8_t len);

/*!
 * process response the acknowledge
 *
 * @param[i] cmd     command id
 * @param[i] result  error code
 *
 * @return none
 */
void bicProcessAck(uint8_t cmd, uint8_t result);

/*!
 * Dispatch events for high level processing
 *
 */
int32_t bicProcessDispatchEvents(void);

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
int32_t bicReceiveChar(uint8_t ch);


#ifdef __cplusplus
}
#endif


#endif
