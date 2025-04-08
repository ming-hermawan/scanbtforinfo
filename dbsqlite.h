/*
 * Scan Bluetooth's for Info
 * Copyright (C) 2025 Hermawan <minghermawan@yahoo.com>
 * https://www.linkedin.com/in/hermawan-ho-a3801194/
 * GNU General Public License (GPL) v3.0
 */
#include <sqlite3.h>

struct BTStruct {
    char addr[19];
    char name[249];
    char coName[255];
    char type[50];
    uint8_t lmpVer;
    uint16_t lmpSubVer;
    char manufactureName[49];
};
void CreateTblBT();
void InstBT(struct BTStruct bt);
void UpdBT(char addr[19],
           char name[249],
           char coName[255],
           char type[50],
           uint8_t lmpVer,
           uint16_t lmpSubVer,
           char manufactureName[49]);
int GetBTsCnt();
void GetBTs(struct BTStruct *result);
