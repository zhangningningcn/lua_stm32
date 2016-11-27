//#include <stdint.h>
#include <stm32f4xx.h>
#include <string.h>
#include "xmodem.h"
#include "app_usart.h"
#include "io.h"
#define DataChackTypeSUM  0
#define DataChackTypeCRC  1

enum XMODE_STATE {
    XMODE_STATE_STOP,
    XMODE_STATE_CONN,
    XMODE_STATE_READ,
    XMODE_STATE_WAITSEND,
    XMODE_STATE_SENDING

};

static uint8_t PacketNum;
static int State = XMODE_STATE_STOP;
static FILE *RWFile;
static uint8_t *DataBuffer;
static int ErrorCounter;
static int DataChackType;

static int DataAddCheck(uint8_t *data,int len);
static uint16_t calcrc(uint8_t *ptr, int count);
static int CRC_Check(uint8_t *data,int len);




int ent_recfile(const char *fn) {
    PacketNum = 1;
    RWFile = mio_fopen(fn,"w+");
    if(RWFile == NULL) {
        return -1;
    }
    DataBuffer = (uint8_t *)malloc(135);
    State = XMODE_STATE_CONN;
    USART_Lua_SetState(USART_Lua_STATE_TRANSFILE);
    return 0;
}
int ent_senfile(const char *fn) {
    PacketNum = 1;
    RWFile = mio_fopen(fn,"r");
    if(RWFile == NULL) {
        return -1;
    }
    DataBuffer = (uint8_t *)malloc(135);
    State = XMODE_STATE_WAITSEND;
    USART_Lua_SetState(USART_Lua_STATE_TRANSFILE);
    return 0;
}
void XmodemRecData(char *data,int len,int errno) {
    int freadlen = 0;
    if(errno == 0) {
        switch(State) {
            case XMODE_STATE_CONN:
                State = XMODE_STATE_READ;
                //no break;
            case XMODE_STATE_READ:
                switch(data[0]) {
                    case XMODE_SOH:
                        if((PacketNum == data[1]) && (PacketNum == (uint8_t)~data[2]) && (len == 133)) {
                            data += 3;
                            if(CRC_Check((uint8_t *)data,128)) {
                                char *f_eof;
                                data[128] = '\0';
                                len = 128;
                                if((f_eof = strchr(data,XMODE_EOF)) != NULL) {
                                    len = f_eof - data;
                                }
                                mio_fwrite(data,1,len,RWFile);
                                DataBuffer[0] = XMODE_ACK;
                                USART_Lua_Send((char *)DataBuffer,1);
                                PacketNum++;
                            }
                            else {
                                DataBuffer[0] = XMODE_NAK;
                                USART_Lua_Send((char *)DataBuffer,1);
                            }
                        }
                        break;
                    case XMODE_EOT:
                        State = XMODE_STATE_STOP;
                        mio_fclose(RWFile);
                        USART_Lua_SetState(USART_Lua_STATE_NORMAL);
                        free(DataBuffer);
                        Lua_InPutOutPutEnable();
                        break;
                    case XMODE_CAN:
                        State = XMODE_STATE_STOP;
                        mio_fclose(RWFile);
                        mio_fremove(RWFile);
                        USART_Lua_SetState(USART_Lua_STATE_NORMAL);
                        free(DataBuffer);
                        Lua_InPutOutPutEnable();
                        break;
                    
                }
                break;
            case XMODE_STATE_SENDING:
                switch(data[0]) {
                    case XMODE_ACK:
                        PacketNum++;
                        L_SEND_DATA:
                        ErrorCounter = 0;
                        freadlen = mio_fread(DataBuffer+3,1,128,RWFile);
                        if(freadlen == 0) {
                            DataBuffer[0] = XMODE_EOF;
                            USART_Lua_Send((char *)DataBuffer,1);
                            State = XMODE_STATE_STOP;
                            mio_fclose(RWFile);
                        }
                        else{
                            DataBuffer[0] = XMODE_SOH;
                            DataBuffer[1] = PacketNum;
                            DataBuffer[2] = ~PacketNum;
                            if(freadlen < 128) {
                                memset(DataBuffer,XMODE_EOF,128-freadlen);
                            }
                            len = DataAddCheck(DataBuffer+3,128);
                            USART_Lua_Send((char *)DataBuffer,len+3);
                        }
                        break;
                    case XMODE_NAK:
                        if(ErrorCounter < XMODE_MAX_ERROR) {
                            USART_Lua_Send((char *)DataBuffer,133);
                            ErrorCounter++;
                        }
                        else {
                            DataBuffer[0] = XMODE_CAN;
                            USART_Lua_Send((char *)DataBuffer,1);
                            USART_Lua_SetState(USART_Lua_STATE_NORMAL);
                        }
                        break;
                    case XMODE_CAN:
                        mio_fclose(RWFile);
                        break;
                    
                }
                break;
            case XMODE_STATE_WAITSEND:
                if(data[0] == XMODE_NAK) {
                    DataChackType = DataChackTypeSUM;
                }
                else if(data[0] == 'C') {
                    DataChackType = DataChackTypeCRC;
                }
                PacketNum = 1;
                goto L_SEND_DATA;
                //break;
            default:
                DataBuffer[0] = XMODE_EOT;
                USART_Lua_Send((char *)DataBuffer,1);
                if(data[0] == XMODE_ACK){
                    USART_Lua_SetState(USART_Lua_STATE_NORMAL);
                    free(DataBuffer);
                    Lua_InPutOutPutEnable();
                }
                break;
                        
        }
        
    }
    else {
        if(State == XMODE_STATE_CONN) {
            DataBuffer[0] = 'C';
            USART_Lua_Send((char *)DataBuffer,1);
        }
        else {
            DataBuffer[0] = XMODE_NAK;
            USART_Lua_Send((char *)DataBuffer,1);
        }
    }
}

static int DataAddCheck(uint8_t *data,int len) {
    if(DataChackType == DataChackTypeSUM) {
        uint8_t sum = 0;
        for(int i=0;i<len;i++) {
            sum += *data;
            data++;
        }
        *data = sum;
        return len+1;
    }
    else if(DataChackType == DataChackTypeCRC) {
        uint16_t crc = calcrc(data,len);
        data[len] = (char)(crc>>8 & 0xFF);
        data[len+1] = (char)(crc & 0xFF);
        return len+2;
        
    }
    return 0;
}


static int CRC_Check(uint8_t *data,int len) {
    uint16_t crc = data[len];
    crc <<= 8;
    crc += data[len+1];
    if(crc == calcrc(data, len)) {
        return 1;
    }
    return 0;
}


static uint16_t calcrc(uint8_t *ptr, int count)
{
    uint16_t crc = 0, i;
    while(--count >= 0) {
        crc = crc ^ (uint16_t)*ptr++ << 8;
        for(i = 0; i < 8; ++i) {
            if(crc & 0x8000) {
                crc = crc << 1 ^ 0x1021;
            }
            else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

