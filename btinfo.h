/*
 * Scan Bluetooth's for Info
 * Copyright (C) 2025 Hermawan <minghermawan@yahoo.com>
 * https://www.linkedin.com/in/hermawan-ho-a3801194/
 * GNU General Public License (GPL) v3.0
 */
#include <bluetooth/hci_lib.h>
#include <stdbool.h> 

struct InfoStruct {
    char addr[19];
    bool isSuccess;
    char name[249];
    char* coName;
    uint8_t lmpVer;
    uint16_t lmpSubVer;
    char manufactureName[49];
};
struct InfoStruct getHCIInfo(int dev_id, char *addr);

// DEVICE TYPE Area BEGIN

static const struct {
    uint8_t val;
    const char *str;
} majorClassComputerTbl[] = {
    {0x00, "Uncategorized, code for device not assigned"},
    {0x01, "Desktop workstation"},
    {0x02, "Server-class computer"},
    {0x03, "Laptop"},
    {0x04, "Handheld PC/PDA (clam shell)"},
    {0x05, "Palm sized PC/PDA"},
    {0x06, "Wearable computer (Watch sized)"},
    {0x07, "Tablet"},
    {}
};

static const char *majorClassComputer(uint8_t minor) {
    for (int n = 0; majorClassComputerTbl[n].str; n++) {
        if (majorClassComputerTbl[n].val == minor)
            return majorClassComputerTbl[n].str;
    }
    return NULL;
}

static const struct {
    uint8_t val;
    const char *str;
} majorClassPhoneTbl[] = {
    {0x00, "Uncategorized, code for device not assigned"},
    {0x01, "Cellular"},
    {0x02, "Cordless"},
    {0x03, "Smart phone"},
    {0x04, "Wired modem or voice gateway"},
    {0x05, "Common ISDN Access"},
    {}
};

static const char *majorClassPhone(uint8_t minor) {
    for (int n = 0; majorClassPhoneTbl[n].str; n++) {
        if (majorClassPhoneTbl[n].val == minor)
            return majorClassPhoneTbl[n].str;
    }
    return NULL;
}

static const struct {
    uint8_t val;
    const char *str;
} majorClassAVTbl[] = {
    {0x00, "Uncategorized, code for device not assigned"},
    {0x01, "earable Headset Device"},
    {0x02, "Hands-free Device"},
    {0x04, "Microphone"},
    {0x05, "Loudspeaker"},
    {0x06, "Headphones"},
    {0x07, "Portable Audio"},
    {0x08, "Car audio"},
    {0x09, "Set-top box"},
    {0x0a, "HiFi Audio Device"},
    {0x0b, "VCR"},
    {0x0c, "Video Camera"},
    {0x0d, "Camcorder"},
    {0x0e, "Video Monitor"},
    {0x0f, "Video Display and Loudspeaker"},
    {0x10, "Video Conferencing"},
    {0x12, "Gaming/Toy"},
    {}
};

static const char *majorClassAV(uint8_t minor) {
    for (int n = 0; majorClassAVTbl[n].str; n++) {
        if (majorClassAVTbl[n].val == minor)
            return majorClassAVTbl[n].str;
    }
    return NULL;
}

static const struct {
    uint8_t val;
    const char *str;
} majorClassWearableTbl[] = {
    {0x01, "Wrist Watch"},
    {0x02, "Pager"},
    {0x03, "Jacket"},
    {0x04, "Helmet"},
    {0x05, "Glasses"},
    {}
};

static const char *majorClassWearable(uint8_t minor) {
    for (int n = 0; majorClassWearableTbl[n].str; n++) {
        if (majorClassWearableTbl[n].val == minor)
            return majorClassWearableTbl[n].str;
    }
    return NULL;
}

static const struct {
    uint8_t val;
    const char *str;
    const char *(*func)(uint8_t minor);
} majorClassTbl[] = {
    {
      0x00,
      "Miscellaneous"},
    {
      0x01,
      "Computer (desktop, notebook, PDA, organizers)",
      majorClassComputer},
    {
      0x02,
      "Phone (cellular, cordless, payphone, modem)",
      majorClassPhone},
    {
      0x03,
      "LAN /Network Access point"},
    {
      0x04,
      "Audio/Video (headset, speaker, stereo, video, vcr)",
      majorClassAV},
    {
      0x05,
      "Peripheral (mouse, joystick, keyboards)"},
    {
      0x06,
      "Imaging (printing, scanner, camera, display)"},
    {
      0x07,
      "Wearable",
      majorClassWearable},
    {
      0x08,
      "Toy"},
    {
      0x09,
      "Health"},
    {
      0x1f,
      "Uncategorized, specific device code not specified"},
    {}
};

void getType(inquiry_info *inquiryInfo, char out[50]);

// DEVICE TYPE Area END

// MANUFACTURE Area BEGIN

void getManufactureName(int id, char out[49]);

// MANUFACTURE Area END
