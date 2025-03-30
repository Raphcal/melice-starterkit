//
//  uuid.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 18/11/2024.
//

#include "uuid.h"

#include "random.h"

MELUUID MELUUIDMakeV4(void) {
    MELUUID self = (MELUUID) {};
    for (int index = 0; index < kUUIDByteCount; index++) {
        self.data[index] = MELRandomInt(0xFF);
    }
    // Version 4
    self.data[6] = (self.data[6] & 0x0F) | 0x40;
    // Variant
    self.data[8] = (self.data[8] & 0x3F) | 0x80;
    return self;
}

MELUUID MELUUIDMakeV7(void) {
    MELUUID self = (MELUUID) {};
    unsigned int milliseconds;
    const unsigned int seconds = playdate->system->getSecondsSinceEpoch(&milliseconds);

    const uint64_t millisecondsFromPlaydateEpoch = 946684800000; //946681200000;
    const uint64_t timestamp = ((uint64_t)seconds * 1000) + (uint64_t)milliseconds + millisecondsFromPlaydateEpoch;

    // UUID v7 structuré en :
    // - 48 bits pour le timestamp
    // - 12 bits pour une séquence aléatoire
    // - 4 bits pour la version (7)
    // - 2 bits pour le variant (RFC 4122)
    // - 62 bits aléatoires

    // Insérer les 48 bits du timestamp
    self.data[0] = (timestamp >> 40) & 0xFF;
    self.data[1] = (timestamp >> 32) & 0xFF;
    self.data[2] = (timestamp >> 24) & 0xFF;
    self.data[3] = (timestamp >> 16) & 0xFF;
    self.data[4] = (timestamp >> 8) & 0xFF;
    self.data[5] = timestamp & 0xFF;

    for (int index = 6; index < kUUIDByteCount; index++) {
        self.data[index] = MELRandomInt(0xFF);
    }

    // Régler les bits pour la version (7)
    self.data[6] = (self.data[6] & 0x0F) | 0x70;

    // Régler les bits pour le variant
    self.data[8] = (self.data[8] & 0x3F) | 0x80;
    return self;
}

void MELUUIDPrint(MELUUID uuid) {
    playdate->system->logToConsole("%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                uuid.data[0], uuid.data[1], uuid.data[2], uuid.data[3],
                uuid.data[4], uuid.data[5],
                uuid.data[6], uuid.data[7],
                uuid.data[8], uuid.data[9],
                uuid.data[10], uuid.data[11], uuid.data[12], uuid.data[13], uuid.data[14], uuid.data[15]);
}

MELBoolean MELUUIDIsEmpty(MELUUID uuid) {
    return uuid.data[6] == 0;
}

MELBoolean MELUUIDEquals(MELUUID lhs, MELUUID rhs) {
    return !memcmp(lhs.data, rhs.data, kUUIDByteCount);
}

uint32_t MELUUIDGetHashcode(MELUUID uuid) {
    uint32_t *dataAsUInt32 = (uint32_t *) &uuid.data;
    return (dataAsUInt32[0] + dataAsUInt32[1] + dataAsUInt32[2]) ^ dataAsUInt32[3];
}
