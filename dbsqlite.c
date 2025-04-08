/*
 * Scan Bluetooth's for Info
 * Copyright (C) 2025 Hermawan <minghermawan@yahoo.com>
 * https://www.linkedin.com/in/hermawan-ho-a3801194/
 * GNU General Public License (GPL) v3.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "dbsqlite.h"

const char *FILENAME = "bt.db";

const char* SQL_CREATE_TBL =
  "CREATE TABLE IF NOT EXISTS bt (" 
  "address TEXT PRIMARY KEY NOT NULL,"
  "name TEXT,"
  "company_name TEXT,"
  "type TEXT,"
  "lmp_version INT,"
  "lmp_sub_version INT,"
  "manufacture_name TEXT,"
  "created_at TEXT NOT NULL DEFAULT current_timestamp,"
  "updated_at TEXT NOT NULL DEFAULT current_timestamp)";
const char* SQL_INS =
  "INSERT INTO bt ("
  "address,"
  "name,"
  "company_name,"
  "type,"
  "lmp_version,"
  "lmp_sub_version,"
  "manufacture_name)"
  " VALUES (?, ?, ?, ?, ?, ?, ?)";
const char *SQL_UPD_NAME =
  "name=:name,";
const char *SQL_UPD_CO_NAME =
  "company_name=:coName,";
const char *SQL_UPD_TYPE =
  "type=:type,";
const char *SQL_UPD_LMP_VER =
  "lmp_version=:lmpVer,";
const char *SQL_UPD_LMP_SUB_VER =
  "lmp_sub_version=:lmpSubVer,";
const char *SQL_UPD_MANUFACTURE_NAME =
  "manufacture_name=:manufactureName,";

sqlite3 *openDB()
{
    sqlite3 *db;
    int opened = sqlite3_open(FILENAME, &db);
    if (opened) {
        printf(
          "Open SQLite database failed; %s",
          sqlite3_errmsg(db));
        exit(1);
    }
    return db;
}
void bindTxt(char *val,
             int idx,
             sqlite3_stmt *stmt,
             sqlite3 *db) {
    int sts =
      sqlite3_bind_text(stmt, idx, val, strlen(val),SQLITE_STATIC);
    if (sts != SQLITE_OK) {
        printf(
          "Can't bind text %s; %s",
          val,
          sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        exit(1);
    }
}

void bindInt(int val,
             int idx,
             sqlite3_stmt *stmt,
             sqlite3 *db) {
    int sts = sqlite3_bind_int(stmt, idx, val);
    if (sts != SQLITE_OK) {
        printf(
          "Can't bind integer %d; %s",
          val,
          sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        exit(1);
    }
}

void bindTxtOrNull(char *val,
                   int idx,
                   sqlite3_stmt *stmt,
                   sqlite3 *db) {
    if (val)
        bindTxt(val, idx, stmt, db);
    else {
        int sts = sqlite3_bind_null(stmt, idx);
        if (sts != SQLITE_OK) {
            printf(
              "Can't bind null; %s",
              sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            exit(1);
        }
    }
}

int getParamIdx(char *param, sqlite3_stmt *stmt, sqlite3 *db) {
    int idx = sqlite3_bind_parameter_index(stmt, param);
    if (idx == 0) {
        printf(
          "Parameter %s not found!",
          param);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        exit(1);
    }
 }

void CreateTblBT() {
    sqlite3 *db = openDB();
    char *errMsg;
    int sts = sqlite3_exec(db, SQL_CREATE_TBL, NULL, 0, &errMsg);
    if (sts != SQLITE_OK) {
        printf(
          "Create SQLite table failed; %s",
          errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_close(db);
}

void InstBT(struct BTStruct bt) {
    sqlite3 *db = openDB();
    sqlite3_stmt *stmt;
    int sts;
    sts = sqlite3_prepare_v2(db, SQL_INS, -1, &stmt, 0);
    if (sts != SQLITE_OK) {
        printf(
          "Insert into SQLite database failed; %s",
          sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    bindTxt(bt.addr, 1, stmt, db);
    bindTxtOrNull(bt.name, 2, stmt, db);
    bindTxtOrNull(bt.coName, 3, stmt, db);
    bindTxtOrNull(bt.type, 4, stmt, db);
    bindInt(bt.lmpVer, 5, stmt, db);
    bindInt(bt.lmpSubVer, 6, stmt, db);
    bindTxtOrNull(bt.manufactureName, 7, stmt, db);
    sts = sqlite3_step(stmt);
    if (sts != SQLITE_DONE) {
        printf(
          "Insert into SQLite database failed; %s",
          sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void UpdBT(char addr[19],
           char name[249],
           char coName[255],
           char type[50],
           uint8_t lmpVer,
           uint16_t lmpSubVer,
           char manufactureName[49]) {
    sqlite3 *db = openDB();
    sqlite3_stmt *stmt;
    int sts;
    char sql[186] = "UPDATE bt SET ";
    int updColCnt = 0;
    if (strcmp(name, "") != 0) {
        strcat(sql, SQL_UPD_NAME);
        updColCnt += 1;
    }
    if (strcmp(coName, "") != 0) {
        strcat(sql, SQL_UPD_CO_NAME);
        updColCnt += 1;
    }
    if (strcmp(type, "") != 0) {
        strcat(sql, SQL_UPD_TYPE);
        updColCnt += 1;
    }
    if (lmpVer > 0) {
        strcat(sql, SQL_UPD_LMP_VER);
        updColCnt += 1;
    }
    if (lmpSubVer > 0) {
        strcat(sql, SQL_UPD_LMP_SUB_VER);
        updColCnt += 1;
    }
    if (strcmp(manufactureName, "") != 0) {
        strcat(sql, SQL_UPD_MANUFACTURE_NAME);
        updColCnt += 1;
    }
    if (updColCnt == 0)
        return;
    strcat(sql, "updated_at=current_timestamp WHERE address=:addr");
    sts = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (sts != SQLITE_OK) {
        printf(
          "Update SQLite database failed; %s",
          sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    int addrIdx = getParamIdx(":addr", stmt, db);
    int nameIdx = 0;
    if (strcmp(name, "") != 0)
        nameIdx = getParamIdx(":name", stmt, db);
    int coNameIdx = 0;
    if (strcmp(coName, "") != 0)
        coNameIdx = getParamIdx(":coName", stmt, db);
    int typeIdx = 0;
    if (strcmp(type, "") != 0)
        typeIdx = getParamIdx(":type", stmt, db);
    int lmpVerIdx = 0;
    if (lmpVer > 0)
        lmpVerIdx = getParamIdx(":lmpVer", stmt, db);
    int lmpSubVerIdx = 0;
    if (lmpSubVer > 0)
        lmpSubVerIdx = getParamIdx(":lmpSubVer", stmt, db);
    int manufactureNameIdx = 0;
    if (strcmp(manufactureName, "") != 0) 
        manufactureNameIdx = getParamIdx(":manufactureName", stmt, db);
    bindTxt(addr, addrIdx, stmt, db);
    if (strcmp(name, "") != 0)
        bindTxt(name, nameIdx, stmt, db);
    if (strcmp(coName, "") != 0)
        bindTxt(coName, coNameIdx, stmt, db);
    if (strcmp(type, "") != 0)
        bindTxt(type, typeIdx, stmt, db);
    if (lmpVer > 0)
        bindInt(lmpVer, lmpVerIdx, stmt, db);
    if (lmpSubVer > 0)
        bindInt(lmpSubVer, lmpSubVerIdx, stmt, db);
    if (strcmp(manufactureName, "") != 0) 
        bindTxt(manufactureName, manufactureNameIdx, stmt, db);
    sts = sqlite3_step(stmt);
    if (sts != SQLITE_DONE) {
        printf(
          "Update SQLite database failed; %s",
          sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

int GetBTsCnt() {
    const char* sql = "SELECT COUNT(*) FROM bt;";
    sqlite3* db = openDB();
    sqlite3_stmt *stmt;
    int sts = sqlite3_prepare_v3(db, sql, -1, 0, &stmt, NULL);
    if (sts) {
        printf(
          "Get Bluetooth's data count from SQLite database failed; %s",
          sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close_v2(db);
        exit(1);
    }
    sqlite3_step(stmt);
    int out = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    sqlite3_close_v2(db);
    return out;
}

void GetBTs(struct BTStruct out[]) {
    const char* sql = "SELECT "
      "address,"
      "name,"
      "company_name,"
      "type,"
      "lmp_version,"
      "lmp_sub_version,"
      "manufacture_name"
      " FROM bt;";
    sqlite3 *db = openDB();
    sqlite3_stmt *stmt;
    int sts = sqlite3_prepare_v3(db, sql, -1, 0, &stmt, NULL);
    if (sts) {
        printf(
          "Get Bluetooth's data from SQLite database failed; %s",
          sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close_v2(db);
        exit(1);
    }
    int n = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* addr = sqlite3_column_text(stmt, 0);
        const char* name = sqlite3_column_text(stmt, 1);
        const char* coName = sqlite3_column_text(stmt, 2);
        const char* type = sqlite3_column_text(stmt, 3);
        uint8_t lmpVer = sqlite3_column_int(stmt, 4);
        uint16_t lmpSubVer = sqlite3_column_int(stmt, 5);
        const char* manufactureName = sqlite3_column_text(stmt, 6);
        strcpy(out[n].addr, addr);
        if (name)
            strcpy(out[n].name, name);
        if (coName)
            strcpy(out[n].coName, coName);
        if (type)
            strcpy(out[n].type, type);
        out[n].lmpVer = lmpVer;
        out[n].lmpSubVer = lmpSubVer;
        if (manufactureName)
            strcpy(out[n].manufactureName, manufactureName);
        n += 1;
    }
    sqlite3_finalize(stmt);
    sqlite3_close_v2(db);
}
