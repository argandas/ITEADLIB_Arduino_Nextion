/**
 * @file NexHardware.cpp
 *
 * The implementation of base API for using Nextion. 
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
#include "NexHardware.h"

/*
 * Receive uint32_t data. 
 * 
 * @param number - save uint32_t data. 
 * @param timeout - set timeout time. 
 *
 * @retval true - success. 
 * @retval false - failed.
 *
 */
bool recvRetNumber(uint32_t *number, uint32_t timeout)
{
    bool ret = false;
    uint8_t temp[8];

    if (NULL != number)
    {
        while (recvCommand(temp, sizeof(temp), timeout))
        {
            if ((temp[0] == NEX_RET_NUMBER_HEAD) && (temp[5] == 0xFF) && (temp[6] == 0xFF) && (temp[7] == 0xFF))
            {
                *number = (temp[4] << 24) | (temp[3] << 16) | (temp[2] << 8) | (temp[1]);
                ret = true;
                break;
            }
        }
    }
    return ret;
}

/*
 * Receive string data. 
 * 
 * @param buffer - save string data. 
 * @param len - string buffer length. 
 * @param timeout - set timeout time. 
 *
 * @return the length of string buffer.
 *
 */
uint16_t recvRetString(char *buffer, uint16_t len, uint32_t timeout)
{
#if 0
    /* Allocate enough memory for the requested string */
    char* __temp = (char*) malloc(len + 4);

    /* Check allocated memory */
    if((NULL != __temp) && (NULL != buffer))
    {
        while(recvCommand(__temp, len + 4, timeout))
        {
            if (__temp[0] == NEX_RET_STRING_HEAD)
            {
                /* Received string */
                *number = (temp[4] << 24) | (temp[3] << 16) | (temp[2] << 8) | (temp[1]);
                ret = true;
                break;
            }
        }

    }

    /* Free memory */
    free (__temp);
#endif

    uint16_t ret = 0;
    bool str_start_flag = false;
    uint8_t cnt_0xff = 0;
    String temp = String("");
    uint8_t c = 0;
    long start;

    if (!buffer || len == 0)
    {
        goto __return;
    }

    start = millis();
    while (millis() - start <= timeout)
    {
        while (nexSerial.available())
        {
            c = nexSerial.read();
            if (str_start_flag)
            {
                if (0xFF == c)
                {
                    cnt_0xff++;
                    if (cnt_0xff >= 3)
                    {
                        break;
                    }
                }
                else
                {
                    temp += (char) c;
                }
            }
            else if (NEX_RET_STRING_HEAD == c)
            {
                str_start_flag = true;
            }
        }

        if (cnt_0xff >= 3)
        {
            break;
        }
    }

    ret = temp.length();
    ret = ret > len ? len : ret;
    strncpy(buffer, temp.c_str(), ret);

    __return:

    dbSerialPrint("recvRetString[");
    dbSerialPrint(temp.length());
    dbSerialPrint(",");
    dbSerialPrint(temp);
    dbSerialPrintln("]");

    return ret;
}

/*
 * Send command to Nextion.
 *
 * @param cmd - the string of command.
 */
void sendCommand(const char* cmd)
{
    char* temp = (char*) cmd;
    while (nexSerial.available())
    {
        nexSerial.read();
    }

    dbSerialPrint(F("[NEX] Tx: "));

    /* Print byte */
    for (; *temp != NULL; temp++)
    {
        if (0x20 <= *temp || 0x7F > *temp)
        {
            /* Printable Chars */dbSerialPrint(*temp);
        }
        else
        {
            dbSerialPrint(" ");
            /* Non-printable Chars */
            if (0x10 > *temp)
            {
                dbSerialPrint("0");
            }
            dbSerialPrint(String((uint8_t)*temp, HEX));
        }
    }
    dbSerialPrintln();

    nexSerial.print(cmd);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
}

uint8_t recvCommand(uint8_t* buff, int len, unsigned long timeout)
{
    /* EOL Counter:
     * The EOL counter is static because this variable will count how many EOL characters
     * have been received even if the message was interrupted due to buffer limitations,
     * this will prevent later messages lost. This counter is cleared only when 3 continuous
     * EOL characters are received.
      */
    static uint8_t cnt = 0;

    uint8_t ret = 0;
    uint8_t idx = 0;
    const uint8_t _end = 0xFF; //end of file x3
    unsigned long tStart = millis();

    while ((nexSerial.available() == 0) && ((millis() - tStart) < timeout))
    {
        /* Do nothing, just wait for data */
        delay(10);
    }

    if (nexSerial.available() > 0)
    {
        /* Clear buffer */
        memset(buff, 0, len);

        dbSerialPrint(F("[NEX] Rx:"));

        /* Fill buffer */
        for (idx = 0; (nexSerial.available() > 0) && (len > idx) && (0 == ret); idx++, buff++)
        {
            /* Save received char */
            *buff = (uint8_t) nexSerial.read();

            /* Print byte */
            {
                dbSerialPrint(" ");
                if (0x10 > *buff)
                {
                    dbSerialPrint("0");
                }
                dbSerialPrint(String(*buff, HEX));
            }

            if (_end == *buff)
            {
                cnt++;
                if (3 <= cnt)
                {
                    /* Reset EOL counter */
                    cnt = 0;
                    /* Return how many bytes were received */
                    ret = idx + 1;
                    continue;
                }
            }
            else
            {
                /* Reset EOL count */
                cnt = 0;
            }
            delay(10);
        }

        /* Print used/available buffer space */
#if 0
        dbSerialPrintln(" (" + String(idx) + "/" + String(len) + ")\r\n");
#else
        dbSerialPrintln();
#endif
        dbSerialPrintln();
    }
    else
    {
        /* Timer has expired */
    }

    return ret;
}

/*
 * Command is executed successfully. 
 *
 * @param timeout - set timeout time.
 *
 * @retval true - success.
 * @retval false - failed. 
 *
 */
bool recvRetCommandFinished(uint32_t timeout)
{
    bool ret = false;
    uint8_t temp[4];

    while (recvCommand(temp, sizeof(temp), timeout))
    {
        if ((temp[0] == NEX_RET_CMD_FINISHED) && (temp[1] == 0xFF) && (temp[2] == 0xFF) && (temp[3] == 0xFF))
        {
            ret = true;
            break;
        }
    }

    return ret;
}

bool nexInit(void)
{
    bool ret1 = false;
    bool ret2 = false;

    dbSerialBegin(9600);
    nexSerial.begin(9600);
    sendCommand("");
    sendCommand("bkcmd=1");
    ret1 = recvRetCommandFinished();
    sendCommand("page 0");
    ret2 = recvRetCommandFinished();
    return ret1 && ret2;
}

void nexLoop(NexTouch *nex_listen_list[])
{
    static uint8_t __buffer[7];

    while (recvCommand(__buffer, sizeof(__buffer)))
    {
        if ((__buffer[0] == NEX_RET_EVENT_TOUCH_HEAD) && (__buffer[4] == 0xFF) && (__buffer[5] == 0xFF) && (__buffer[6] == 0xFF))
        {
            NexTouch::iterate(nex_listen_list, __buffer[1], __buffer[2], (int32_t) __buffer[3]);
        }
    }
}

