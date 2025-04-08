/*
 * Scan Bluetooth's for Info
 * Copyright (C) 2025 Hermawan <minghermawan@yahoo.com>
 * https://www.linkedin.com/in/hermawan-ho-a3801194/
 * GNU General Public License (GPL) v3.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include "btinfo.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_UDEV_HWDB_NEW
#include <libudev.h>


char *getCoName(const bdaddr_t *btAddr) {
    char modalias[11];
    sprintf(
      modalias,
      "OUI:%2.2X%2.2X%2.2X",
      btAddr->b[5],
      btAddr->b[4],
      btAddr->b[3]);

    struct udev *udev = udev_new();
    if (!udev)
        return NULL;

    struct udev_hwdb *hwdb = udev_hwdb_new(udev);
    if (!hwdb)
        goto done;

    struct udev_list_entry *head = udev_hwdb_get_properties_list_entry(
      hwdb,
      modalias,
      0);

    char *out = NULL;
    struct udev_list_entry *listEntry;
    udev_list_entry_foreach(listEntry, head) {
        const char *name = udev_list_entry_get_name(listEntry);
        if (name && !strcmp(name, "ID_OUI_FROM_DATABASE")) {
            out = strdup(udev_list_entry_get_value(listEntry));
            break;
        }
    }

    hwdb = udev_hwdb_unref(hwdb);

done:

    udev = udev_unref(udev);

    return out;
}
#else
char *getCoName(const bdaddr_t *btAddr) {
    return NULL;
}
#endif

static int getConnection(int s, int devId, long arg) {
    struct hci_conn_list_req *hciConnListReq;
    struct hci_conn_info *hciConnInfo;

    if (
      !(hciConnListReq = malloc(
        10 * sizeof(*hciConnInfo) + sizeof(*hciConnListReq)
      ))
    ) {
        perror("Can't allocate memory");
        exit(1);
    }

    hciConnListReq->dev_id = devId;
    hciConnListReq->conn_num = 10;
    hciConnInfo = hciConnListReq->conn_info;

    if (ioctl(s, HCIGETCONNLIST, (void *) hciConnListReq)) {
        perror("Can't get connection list");
        exit(1);
    }

    for (int i = 0; i < hciConnListReq->conn_num; i++, hciConnInfo++)
        if (!bacmp((bdaddr_t *) arg, &hciConnInfo->bdaddr)) {
            free(hciConnListReq);
            return 1;
        }

    free(hciConnListReq);
    return 0;
}

void getType(inquiry_info *inquiryInfo, char out[50]) {
    uint8_t devClass0 = (uint8_t)(inquiryInfo->dev_class[0]);
    uint8_t devClass1 = (uint8_t)(inquiryInfo->dev_class[1]);
    uint8_t majorClass = devClass1 & 0x1f;
    uint8_t minorClass = (devClass0 & 0xfc) >> 2;

    for (int n = 0; majorClassTbl[n].str; n++) {
        if (majorClassTbl[n].val == majorClass) {
            if(!majorClassTbl[n].func) {
                strcpy(out, majorClassTbl[n].str);
                break;
            }
            strcpy(out, majorClassTbl[n].func(minorClass));
            break;
        }
    }
}

void getManufactureName(int id, char out[49]) {
    switch (id) {
      case 0:
        strcpy(out, "Ericsson Technology Licensing");
        return;
      case 1:
        strcpy(out, "Nokia Mobile Phones");
        return;
      case 2:
        strcpy(out, "Intel Corp.");
        return;
      case 3:
        strcpy(out, "IBM Corp.");
        return;
      case 4:
        strcpy(out, "Toshiba Corp.");
        return;
      case 5:
        strcpy(out, "3Com");
        return;
      case 6:
        strcpy(out, "Microsoft");
        return;
      case 7:
        strcpy(out, "Lucent");
        return;
      case 8:
        strcpy(out, "Motorola");
        return;
      case 9:
        strcpy(out, "Infineon Technologies AG");
        return;
      case 10:
        strcpy(out, "Cambridge Silicon Radio");
        return;
      case 11:
        strcpy(out, "Silicon Wave");
        return;
      case 12:
        strcpy(out, "Digianswer A/S");
        return;
      case 13:
        strcpy(out, "Texas Instruments Inc.");
        return;
      case 14:
        strcpy(out, "Ceva, Inc. (formerly Parthus Technologies, Inc.)");
        return;
      case 15:
        strcpy(out, "Broadcom Corporation");
        return;
      case 16:
        strcpy(out, "Mitel Semiconductor");
        return;
      case 17:
        strcpy(out, "Widcomm, Inc");
        return;
      case 18:
        strcpy(out, "Zeevo, Inc.");
        return;
      case 19:
        strcpy(out, "Atmel Corporation");
        return;
      case 20:
        strcpy(out, "Mitsubishi Electric Corporation");
        return;
      case 21:
        strcpy(out, "RTX Telecom A/S");
        return;
      case 22:
        strcpy(out, "KC Technology Inc.");
        return;
      case 23:
        strcpy(out, "NewLogic");
        return;
      case 24:
        strcpy(out, "Transilica, Inc.");
        return;
      case 25:
        strcpy(out, "Rohde & Schwarz GmbH & Co. KG");
        return;
      case 26:
        strcpy(out, "TTPCom Limited");
        return;
      case 27:
        strcpy(out, "Signia Technologies, Inc.");
        return;
      case 28:
        strcpy(out, "Conexant Systems Inc.");
        return;
      case 29:
        strcpy(out, "Qualcomm");
        return;
      case 30:
        strcpy(out, "Inventel");
        return;
      case 31:
        strcpy(out, "AVM Berlin");
        return;
      case 32:
        strcpy(out, "BandSpeed, Inc.");
        return;
      case 33:
        strcpy(out, "Mansella Ltd");
        return;
      case 34:
        strcpy(out, "NEC Corporation");
        return;
      case 35:
        strcpy(out, "WavePlus Technology Co., Ltd.");
        return;
      case 36:
        strcpy(out, "Alcatel");
        return;
      case 37:
        strcpy(out, "Philips Semiconductors");
        return;
      case 38:
        strcpy(out, "C Technologies");
        return;
      case 39:
        strcpy(out, "Open Interface");
        return;
      case 40:
        strcpy(out, "R F Micro Devices");
        return;
      case 41:
        strcpy(out, "Hitachi Ltd");
        return;
      case 42:
        strcpy(out, "Symbol Technologies, Inc.");
        return;
      case 43:
        strcpy(out, "Tenovis");
        return;
      case 44:
        strcpy(out, "Macronix International Co. Ltd.");
        return;
      case 45:
        strcpy(out, "GCT Semiconductor");
        return;
      case 46:
        strcpy(out, "Norwood Systems");
        return;
      case 47:
        strcpy(out, "MewTel Technology Inc.");
        return;
      case 48:
        strcpy(out, "ST Microelectronics");
        return;
      case 49:
        strcpy(out, "Synopsis");
        return;
      case 50:
        strcpy(out, "Red-M (Communications) Ltd");
        return;
      case 51:
        strcpy(out, "Commil Ltd");
        return;
      case 52:
        strcpy(out, "Computer Access Technology Corporation (CATC)");
        return;
      case 53:
        strcpy(out, "Eclipse (HQ Espana) S.L.");
        return;
      case 54:
        strcpy(out, "Renesas Technology Corp.");
        return;
      case 55:
        strcpy(out, "Mobilian Corporation");
        return;
      case 56:
        strcpy(out, "Terax");
        return;
      case 57:
        strcpy(out, "Integrated System Solution Corp.");
        return;
      case 58:
        strcpy(out, "Matsushita Electric Industrial Co., Ltd.");
        return;
      case 59:
        strcpy(out, "Gennum Corporation");
        return;
      case 60:
        strcpy(out, "Research In Motion");
        return;
      case 61:
        strcpy(out, "IPextreme, Inc.");
        return;
      case 62:
        strcpy(out, "Systems and Chips, Inc.");
        return;
      case 63:
        strcpy(out, "Bluetooth SIG, Inc.");
        return;
      case 64:
        strcpy(out, "Seiko Epson Corporation");
        return;
      case 65:
        strcpy(out, "Integrated Silicon Solution Taiwan, Inc.");
        return;
      case 66:
        strcpy(out, "CONWISE Technology Corporation Ltd");
        return;
      case 67:
        strcpy(out, "PARROT SA");
        return;
      case 68:
        strcpy(out, "Socket Mobile");
        return;
      case 69:
        strcpy(out, "Atheros Communications, Inc.");
        return;
      case 70:
        strcpy(out, "MediaTek, Inc.");
        return;
      case 71:
        strcpy(out, "Bluegiga");
        return;
      case 72:
        strcpy(out, "Marvell Technology Group Ltd.");
        return;
      case 73:
        strcpy(out, "3DSP Corporation");
        return;
      case 74:
        strcpy(out, "Accel Semiconductor Ltd.");
        return;
      case 75:
        strcpy(out, "Continental Automotive Systems");
        return;
      case 76:
        strcpy(out, "Apple, Inc.");
        return;
      case 77:
        strcpy(out, "Staccato Communications, Inc.");
        return;
      case 78:
        strcpy(out, "Avago Technologies");
        return;
      case 79:
        strcpy(out, "APT Licensing Ltd.");
        return;
      case 80:
        strcpy(out, "SiRF Technology");
        return;
      case 81:
        strcpy(out, "Tzero Technologies, Inc.");
        return;
      case 82:
        strcpy(out, "J&M Corporation");
        return;
      case 83:
        strcpy(out, "Free2move AB");
        return;
      case 84:
        strcpy(out, "3DiJoy Corporation");
        return;
      case 85:
        strcpy(out, "Plantronics, Inc.");
        return;
      case 86:
        strcpy(out, "Sony Ericsson Mobile Communications");
        return;
      case 87:
        strcpy(out, "Harman International Industries, Inc.");
        return;
      case 88:
        strcpy(out, "Vizio, Inc.");
        return;
      case 89:
        strcpy(out, "Nordic Semiconductor ASA");
        return;
      case 90:
        strcpy(out, "EM Microelectronic-Marin SA");
        return;
      case 91:
        strcpy(out, "Ralink Technology Corporation");
        return;
      case 92:
        strcpy(out, "Belkin International, Inc.");
        return;
      case 93:
        strcpy(out, "Realtek Semiconductor Corporation");
        return;
      case 94:
        strcpy(out, "Stonestreet One, LLC");
        return;
      case 95:
        strcpy(out, "Wicentric, Inc.");
        return;
      case 96:
        strcpy(out, "RivieraWaves S.A.S");
        return;
      case 97:
        strcpy(out, "RDA Microelectronics");
        return;
      case 98:
        strcpy(out, "Gibson Guitars");
        return;
      case 99:
        strcpy(out, "MiCommand Inc.");
        return;
      case 100:
        strcpy(out, "Band XI International, LLC");
        return;
      case 101:
        strcpy(out, "Hewlett-Packard Company");
        return;
      case 102:
        strcpy(out, "9Solutions Oy");
        return;
      case 103:
        strcpy(out, "GN Netcom A/S");
        return;
      case 104:
        strcpy(out, "General Motors");
        return;
      case 105:
        strcpy(out, "A&D Engineering, Inc.");
        return;
      case 106:
        strcpy(out, "MindTree Ltd.");
        return;
      case 107:
        strcpy(out, "Polar Electro OY");
        return;
      case 108:
        strcpy(out, "Beautiful Enterprise Co., Ltd.");
        return;
      case 109:
        strcpy(out, "BriarTek, Inc.");
        return;
      case 110:
        strcpy(out, "Summit Data Communications, Inc.");
        return;
      case 111:
        strcpy(out, "Sound ID");
        return;
      case 112:
        strcpy(out, "Monster, LLC");
        return;
      case 113:
        strcpy(out, "connectBlue AB");
        return;
      case 114:
        strcpy(out, "ShangHai Super Smart Electronics Co. Ltd.");
        return;
      case 115:
        strcpy(out, "Group Sense Ltd.");
        return;
      case 116:
        strcpy(out, "Zomm, LLC");
        return;
      case 117:
        strcpy(out, "Samsung Electronics Co. Ltd.");
        return;
      case 118:
        strcpy(out, "Creative Technology Ltd.");
        return;
      case 119:
        strcpy(out, "Laird Technologies");
        return;
      case 120:
        strcpy(out, "Nike, Inc.");
        return;
      case 121:
        strcpy(out, "lesswire AG");
        return;
      case 122:
        strcpy(out, "MStar Semiconductor, Inc.");
        return;
      case 123:
        strcpy(out, "Hanlynn Technologies");
        return;
      case 124:
        strcpy(out, "A & R Cambridge");
        return;
      case 125:
        strcpy(out, "Seers Technology Co. Ltd");
        return;
      case 126:
        strcpy(out, "Sports Tracking Technologies Ltd.");
        return;
      case 127:
        strcpy(out, "Autonet Mobile");
        return;
      case 128:
        strcpy(out, "DeLorme Publishing Company, Inc.");
        return;
      case 129:
        strcpy(out, "WuXi Vimicro");
        return;
      case 130:
        strcpy(out, "Sennheiser Communications A/S");
        return;
      case 131:
        strcpy(out, "TimeKeeping Systems, Inc.");
        return;
      case 132:
        strcpy(out, "Ludus Helsinki Ltd.");
        return;
      case 133:
        strcpy(out, "BlueRadios, Inc.");
        return;
      case 134:
        strcpy(out, "equinox AG");
        return;
      case 135:
        strcpy(out, "Garmin International, Inc.");
        return;
      case 136:
        strcpy(out, "Ecotest");
        return;
      case 137:
        strcpy(out, "GN ReSound A/S");
        return;
      case 138:
        strcpy(out, "Jawbone");
        return;
      case 139:
        strcpy(out, "Topcorn Positioning Systems, LLC");
        return;
      case 140:
        strcpy(out, "Qualcomm Labs, Inc.");
        return;
      case 141:
        strcpy(out, "Zscan Software");
        return;
      case 142:
        strcpy(out, "Quintic Corp.");
        return;
      case 143:
        strcpy(out, "Stollman E+V GmbH");
        return;
      case 144:
        strcpy(out, "Funai Electric Co., Ltd.");
        return;
      case 145:
        strcpy(out, "Advanced PANMOBIL Systems GmbH & Co. KG");
        return;
      case 146:
        strcpy(out, "ThinkOptics, Inc.");
        return;
      case 147:
        strcpy(out, "Universal Electronics, Inc.");
        return;
      case 148:
        strcpy(out, "Airoha Technology Corp.");
        return;
      case 149:
        strcpy(out, "NEC Lighting, Ltd.");
        return;
      case 150:
        strcpy(out, "ODM Technology, Inc.");
        return;
      case 151:
        strcpy(out, "ConnecteDevice Ltd.");
        return;
      case 152:
        strcpy(out, "zer01.tv GmbH");
        return;
      case 153:
        strcpy(out, "i.Tech Dynamic Global Distribution Ltd.");
        return;
      case 154:
        strcpy(out, "Alpwise");
        return;
      case 155:
        strcpy(out, "Jiangsu Toppower Automotive Electronics Co., Ltd.");
        return;
      case 156:
        strcpy(out, "Colorfy, Inc.");
        return;
      case 157:
        strcpy(out, "Geoforce Inc.");
        return;
      case 158:
        strcpy(out, "Bose Corporation");
        return;
      case 159:
        strcpy(out, "Suunto Oy");
        return;
      case 160:
        strcpy(out, "Kensington Computer Products Group");
        return;
      case 161:
        strcpy(out, "SR-Medizinelektronik");
        return;
      case 162:
        strcpy(out, "Vertu Corporation Limited");
        return;
      case 163:
        strcpy(out, "Meta Watch Ltd.");
        return;
      case 164:
        strcpy(out, "LINAK A/S");
        return;
      case 165:
        strcpy(out, "OTL Dynamics LLC");
        return;
      case 166:
        strcpy(out, "Panda Ocean Inc.");
        return;
      case 167:
        strcpy(out, "Visteon Corporation");
        return;
      case 168:
        strcpy(out, "ARP Devices Limited");
        return;
      case 169:
        strcpy(out, "Magneti Marelli S.p.A");
        return;
      case 170:
        strcpy(out, "CAEN RFID srl");
        return;
      case 171:
        strcpy(out, "Ingenieur-Systemgruppe Zahn GmbH");
        return;
      case 172:
        strcpy(out, "Green Throttle Games");
        return;
      case 173:
        strcpy(out, "Peter Systemtechnik GmbH");
        return;
      case 174:
        strcpy(out, "Omegawave Oy");
        return;
      case 175:
        strcpy(out, "Cinetix");
        return;
      case 176:
        strcpy(out, "Passif Semiconductor Corp");
        return;
      case 177:
        strcpy(out, "Saris Cycling Group, Inc");
        return;
      case 178:
        strcpy(out, "Bekey A/S");
        return;
      case 179:
        strcpy(out, "Clarinox Technologies Pty. Ltd.");
        return;
      case 180:
        strcpy(out, "BDE Technology Co., Ltd.");
        return;
      case 181:
        strcpy(out, "Swirl Networks");
        return;
      case 182:
        strcpy(out, "Meso international");
        return;
      case 183:
        strcpy(out, "TreLab Ltd");
        return;
      case 184:
        strcpy(out, "Qualcomm Innovation Center, Inc. (QuIC)");
        return;
      case 185:
        strcpy(out, "Johnson Controls, Inc.");
        return;
      case 186:
        strcpy(out, "Starkey Laboratories Inc.");
        return;
      case 187:
        strcpy(out, "S-Power Electronics Limited");
        return;
      case 188:
        strcpy(out, "Ace Sensor Inc");
        return;
      case 189:
        strcpy(out, "Aplix Corporation");
        return;
      case 190:
        strcpy(out, "AAMP of America");
        return;
      case 191:
        strcpy(out, "Stalmart Technology Limited");
        return;
      case 192:
        strcpy(out, "AMICCOM Electronics Corporation");
        return;
      case 193:
        strcpy(out, "Shenzhen Excelsecu Data Technology Co.,Ltd");
        return;
      case 194:
        strcpy(out, "Geneq Inc.");
        return;
      case 195:
        strcpy(out, "adidas AG");
        return;
      case 196:
        strcpy(out, "LG Electronics");
        return;
      case 197:
        strcpy(out, "Onset Computer Corporation");
        return;
      case 198:
        strcpy(out, "Selfly BV");
        return;
      case 199:
        strcpy(out, "Quuppa Oy.");
        return;
      case 200:
        strcpy(out, "GeLo Inc");
        return;
      case 201:
        strcpy(out, "Evluma");
        return;
      case 202:
        strcpy(out, "MC10");
        return;
      case 203:
        strcpy(out, "Binauric SE");
        return;
      case 204:
        strcpy(out, "Beats Electronics");
        return;
      case 205:
        strcpy(out, "Microchip Technology Inc.");
        return;
      case 206:
        strcpy(out, "Elgato Systems GmbH");
        return;
      case 207:
        strcpy(out, "ARCHOS SA");
        return;
      case 209:
        strcpy(out, "Polar Electro Europe B.V.");
        return;
      case 210:
        strcpy(out, "Dialog Semiconductor B.V.");
        return;
      case 211:
        strcpy(out, "Taixingbang Technology (HK) Co,. LTD.");
        return;
      case 212:
        strcpy(out, "Kawantech");
        return;
      case 213:
        strcpy(out, "Austco Communication Systems");
        return;
      case 214:
        strcpy(out, "Timex Group USA, Inc.");
        return;
      case 215:
        strcpy(out, "Qualcomm Technologies, Inc.");
        return;
      case 216:
        strcpy(out, "Qualcomm Connected Experiences, Inc.");
        return;
      case 217:
        strcpy(out, "Voyetra Turtle Beach");
        return;
      case 218:
        strcpy(out, "txtr GmbH");
        return;
      case 219:
        strcpy(out, "Biosentronics");
        return;
      case 220:
        strcpy(out, "Procter & Gamble");
        return;
      case 221:
        strcpy(out, "Hosiden Corporation");
        return;
      case 222:
        strcpy(out, "Muzik LLC");
        return;
      case 223:
        strcpy(out, "Misfit Wearables Corp");
        return;
      case 224:
        strcpy(out, "Google");
        return;
      case 225:
        strcpy(out, "Danlers Ltd");
        return;
      case 226:
        strcpy(out, "Semilink Inc");
        return;
      case 227:
        strcpy(out, "inMusic Brands, Inc");
        return;
      case 228:
        strcpy(out, "L.S. Research Inc.");
        return;
      case 229:
        strcpy(out, "Eden Software Consultants Ltd.");
        return;
      case 230:
        strcpy(out, "Freshtemp");
        return;
      case 231:
        strcpy(out, "KS Technologies");
        return;
      case 232:
        strcpy(out, "ACTS Technologies");
        return;
      case 233:
        strcpy(out, "Vtrack Systems");
        return;
      case 234:
        strcpy(out, "Nielsen-Kellerman Company");
        return;
      case 235:
        strcpy(out, "Server Technology, Inc.");
        return;
      case 236:
        strcpy(out, "BioResearch Associates");
        return;
      case 237:
        strcpy(out, "Jolly Logic, LLC");
        return;
      case 238:
        strcpy(out, "Above Average Outcomes, Inc.");
        return;
      case 239:
        strcpy(out, "Bitsplitters GmbH");
        return;
      case 240:
        strcpy(out, "PayPal, Inc.");
        return;
      case 241:
        strcpy(out, "Witron Technology Limited");
        return;
      case 242:
        strcpy(out, "Morse Project Inc.");
        return;
      case 243:
        strcpy(out, "Kent Displays Inc.");
        return;
      case 244:
        strcpy(out, "Nautilus Inc.");
        return;
      case 245:
        strcpy(out, "Smartifier Oy");
        return;
      case 246:
        strcpy(out, "Elcometer Limited");
        return;
      case 247:
        strcpy(out, "VSN Technologies Inc.");
        return;
      case 248:
        strcpy(out, "AceUni Corp., Ltd.");
        return;
      case 65535:
        strcpy(out, "internal use");
        return;
      default:
        strcpy(out, "not assigned");
    }
}

struct InfoStruct getHCIInfo(int devId, char addr[19]) {
    struct InfoStruct out;
    strcpy(out.addr, addr);
    struct hci_version hciVer;
    uint16_t handle;
    struct hci_dev_info hciDevInfo;
    struct hci_conn_info_req *hciConnInfoReq;
    int cc = 0;

    bdaddr_t btAddr;
    str2ba(addr, &btAddr);

    if (devId < 0)
        devId = hci_for_each_dev(HCI_UP, getConnection, (long) &btAddr);

    if (devId < 0)
        devId = hci_get_route(&btAddr);

    if (devId < 0) {
        printf("No device available or connected!");
        exit(1);
    }

    if (hci_devinfo(devId, &hciDevInfo) < 0) {
        perror("Can't get device info");
        exit(1);
    }

    int devDescriptor = hci_open_dev(devId);
    if (devDescriptor < 0) {
        perror("HCI device open failed");
        exit(1);
    }

    hciConnInfoReq = malloc(sizeof(*hciConnInfoReq) + sizeof(struct hci_conn_info));
    if (!hciConnInfoReq) {
        perror("Can't get connection info");
        close(devDescriptor);
        exit(1);
    }

    bacpy(&hciConnInfoReq->bdaddr, &btAddr);
    hciConnInfoReq->type = ACL_LINK;
    if (ioctl(
      devDescriptor,
      HCIGETCONNINFO,
      (unsigned long)hciConnInfoReq) < 0
    ) {
        if (hci_create_connection(
          devDescriptor,
          &btAddr,
          htobs(hciDevInfo.pkt_type & ACL_PTYPE_MASK),
          0,
          0x01,
          &handle,
          25000) < 0
        ) {
            printf("Can't create connection\n");
            close(devDescriptor);
            out.isSuccess = false;
            return out;
        }
        sleep(1);
        cc = 1;
    } else
        handle = htobs(hciConnInfoReq->conn_info->handle);

    char *coName = getCoName(&btAddr);
    char coOUI[9];
    if (coName) {
        ba2oui(&btAddr, coOUI);
        free(coName);
    }

    char name[249];
    if (hci_read_remote_name(
      devDescriptor,
      &btAddr,
      sizeof(name),
      name,
      25000) == 0
    ) {

    }

    uint8_t lmpVer;
    uint16_t lmpSubVer, manufactureId;
    char manufactureName[49];
    if (hci_read_remote_version(devDescriptor, handle, &hciVer, 20000) == 0) {
        char *ver = lmp_vertostr(hciVer.lmp_ver);
        lmpVer = (uint8_t)(hciVer.lmp_ver);
        lmpSubVer = (uint16_t)(hciVer.lmp_subver);
        manufactureId = hciVer.manufacturer;
        getManufactureName(manufactureId, manufactureName);
        if (ver)
            bt_free(ver);
    }

    if (cc) {
        usleep(10000);
        hci_disconnect(devDescriptor, handle, HCI_OE_USER_ENDED_CONNECTION, 10000);
    }

    hci_close_dev(devDescriptor);
    out.isSuccess = true;
    strcpy(out.name, name);
    out.coName = coName;
    out.lmpVer = lmpVer;
    out.lmpSubVer = lmpSubVer;
    if(manufactureName)
        strcpy(out.manufactureName, manufactureName);
    return out;
}
