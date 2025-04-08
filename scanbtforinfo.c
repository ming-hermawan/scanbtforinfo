/*
 * Scan Bluetooth's for Info
 * Copyright (C) 2025 Hermawan <minghermawan@yahoo.com>
 * https://www.linkedin.com/in/hermawan-ho-a3801194/
 * GNU General Public License (GPL) v3.0
 */
#include <stdlib.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include "btinfo.h"
#include "dbsqlite.h"

const int MAX_BT_NUM = 255;
const int LEN = 8;

int getBTIdx(char addr[19], struct BTStruct *btArray, int count) {
    for (int n2 = 0; n2 < count; n2++) {
        if (strcmp(btArray[n2].addr, addr) == 0) {
            return n2;
        }
    }
    return -1;
}

int main(int argc, char *argv[]) {
    CreateTblBT();
    int count = GetBTsCnt();
    struct BTStruct *btArray = malloc(count * sizeof(struct BTStruct));
    GetBTs(btArray);
    while(1) {
        printf("START SCANNING\n");
        int devId = hci_get_route(NULL);
        int socket = hci_open_dev( devId );
        if ( (devId < 0) || (socket < 0) ) {
            perror("opening socket");
            exit(1);
        }
        inquiry_info *inquiryInfo = (inquiry_info*)malloc(
          MAX_BT_NUM * sizeof(inquiry_info)
        );
        int btNum = hci_inquiry(
          devId,
          LEN, MAX_BT_NUM,
          NULL,
          &inquiryInfo,
          IREQ_CACHE_FLUSH);
        if( btNum < 0 )
            perror("hci_inquiry");
        printf("Found %d Bluetooth devices.\n", btNum);
        char addr[19] = {0};
        for (int n1 = 0; n1 < btNum; n1++) {
            struct BTStruct bt;
            inquiry_info *currentInquiryInfo = inquiryInfo + n1;
            ba2str(&(currentInquiryInfo)->bdaddr, addr);
            printf("%d). %s\n", (n1 + 1), addr);

            // Get Device Type
            char type[50];
            getType(currentInquiryInfo, type);

            // Get Device Info
            struct InfoStruct info =  getHCIInfo(devId, addr);

            // Save Info
            int btIdx = getBTIdx(addr, btArray, count);

            if(btIdx > -1) {
                if (!info.isSuccess) {
                    continue;
                }
                char updName[249] = "";
                if(
                  (info.name)
                  && (strcmp(updName, btArray[btIdx].name) != 0)
                ) {
                    strcpy(updName, info.name);
                    strcpy(btArray[btIdx].name, info.name);
                }
                char updCoName[255] = "";
                if(
                  (info.coName)
                  && (strcmp(info.coName, btArray[btIdx].coName) != 0)
                ) {
                    strcpy(updCoName, info.coName);
                    strcpy(btArray[btIdx].coName, info.coName);
                }
                char updType[50] = "";
                if(
                  (type)
                  && (strcmp(type, btArray[btIdx].type) != 0)
                ) {
                    strcpy(updType, type);
                    strcpy(btArray[btIdx].type, type);
                }
                uint8_t updLmpVer;
                if(
                  (info.lmpVer > 0)
                  && (info.lmpVer != btArray[btIdx].lmpVer) 
                ) {
                    updLmpVer = info.lmpVer;
                    btArray[btIdx].lmpVer = info.lmpVer;
                } else {
                    updLmpVer = 0;
                }
                uint16_t updLmpSubVer;
                if(
                  (info.lmpSubVer > 0)
                  && (info.lmpSubVer != btArray[btIdx].lmpSubVer) 
                ) {
                    updLmpSubVer = info.lmpSubVer;
                    btArray[btIdx].lmpSubVer = info.lmpSubVer;
                } else {
                    updLmpSubVer = 0;
                }
                char updManufactureName[49] = "";
                if(
                  (info.manufactureName)
                  && (strcmp(
                        info.manufactureName,
                        btArray[btIdx].manufactureName) != 0)
                ) {
                    strcpy(updManufactureName, info.manufactureName);
                    strcpy(
                      btArray[btIdx].manufactureName,
                      info.manufactureName);
                }
                UpdBT(
                  addr,
                  updName,
                  updCoName,
                  updType,
                  updLmpVer,
                  updLmpSubVer,
                  updManufactureName);
                bt = btArray[btIdx];
            } else {
                strcpy(bt.addr, info.addr);
                strcpy(bt.type, type);
                if(info.isSuccess) {
                    if (info.name)
                        strcpy(bt.name, info.name);
                    if (info.coName)
                        strcpy(bt.coName, info.coName);
                    bt.lmpVer = info.lmpVer;
                    bt.lmpSubVer = info.lmpSubVer;
                    if (info.manufactureName)
                        strcpy(bt.manufactureName, info.manufactureName);
                } else {
                    strcpy(bt.name, "");
                    strcpy(bt.coName, "");
                    bt.lmpVer = 0;
                    bt.lmpSubVer = 0;
                    strcpy(bt.manufactureName, "");
                }
                InstBT(bt);

                count += 1;
                struct BTStruct *newBTArray = realloc(
                  btArray,
                  count * sizeof(struct BTStruct)
                );
                if (newBTArray == NULL) {
                    printf("Memory reallocation failed.\n");
                    free(btArray);
                    exit(1);
                }
                btArray = newBTArray;
                newBTArray = NULL;
                btArray[count - 1] = bt;
            }
            printf("NAME             = %s\n", bt.name);
            printf("COMPANY          = %s\n", bt.coName);
            printf("TYPE             = %s\n", bt.type);
            printf("LMP-VER          = %d\n", bt.lmpVer);
            printf("LMP-SUB-VER      = %d\n", bt.lmpSubVer);
            printf("MANUFACTURE NAME = %s\n", bt.manufactureName);
        }
        free(inquiryInfo);
        close(socket);
    }
}
