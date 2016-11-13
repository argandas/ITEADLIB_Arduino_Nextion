/**
 * @file NexHardware.h
 *
 * The definition of base API for using Nextion. 
 *
 * @author  Wu Pengfei (email:<pengfei.wu@itead.cc>)
 * @date    2015/8/11
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */
#ifndef __NEXHARDWARE_H__
#define __NEXHARDWARE_H__
#include <Arduino.h>
#include "NexConfig.h"
#include "NexTouch.h"

#define NEX_RET_INVALID_CMD                 (0x00)
#define NEX_RET_CMD_FINISHED                (0x01)
#define NEX_RET_INVALID_COMPONENT_ID        (0x02)
#define NEX_RET_INVALID_PAGE_ID             (0x03)
#define NEX_RET_INVALID_PICTURE_ID          (0x04)
#define NEX_RET_INVALID_FONT_ID             (0x05)
#define NEX_RET_INVALID_BAUD                (0x11)
#define NEX_RET_INVALID_VARIABLE            (0x1A)
#define NEX_RET_INVALID_OPERATION           (0x1B)
#define NEX_RET_EVENT_TOUCH_HEAD            (0x65)
#define NEX_RET_EVENT_POSITION_HEAD         (0x67)
#define NEX_RET_CURRENT_PAGE_ID_HEAD        (0x66)
#define NEX_RET_EVENT_SLEEP_POSITION_HEAD   (0x68)
#define NEX_RET_STRING_HEAD                 (0x70)
#define NEX_RET_NUMBER_HEAD                 (0x71)
#define NEX_RET_EVENT_AUTO_WAKE_UP          (0x87)
#define NEX_RET_EVENT_LAUNCHED              (0x88)
#define NEX_RET_EVENT_UPGRADED              (0x89)

/**
 * @addtogroup CoreAPI 
 * @{ 
 */

/**
 * Init Nextion.  
 * 
 * @return true if success, false for failure. 
 */
bool nexInit(void);

/**
 * Listen touch event and calling callbacks attached before.
 * 
 * Supports push and pop at present. 
 *
 * @param nex_listen_list - index to Nextion Components list. 
 * @return none. 
 *
 * @warning This function must be called repeatedly to response touch events
 *  from Nextion touch panel. Actually, you should place it in your loop function. 
 */
void nexLoop(NexTouch *nex_listen_list[]);

/**
 * @}
 */

bool recvRetNumber(uint32_t *number, uint32_t timeout = 100);
uint16_t recvRetString(char *buffer, uint16_t len, uint32_t timeout = 100);
void sendCommand(const char* cmd);
uint8_t recvCommand(uint8_t* buff, int len, unsigned long timeout = 100);
bool recvRetCommandFinished(uint32_t timeout = 100);

#endif /* #ifndef __NEXHARDWARE_H__ */
