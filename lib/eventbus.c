//
//  eventbus.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 17/12/2023.
//

#include "eventbus.h"

static MELBoolean MELEventBusEntryEquals(MELEventBusEntry lhs, MELEventBusEntry rhs) {
    return lhs.userdata == rhs.userdata;
}

MELListImplement(MELEventBusEntry);
MELListImplementIndexOfWithEqualsFunction(MELEventBusEntry, MELEventBusEntryEquals);

static MELEventBus kInstance = (MELEventBus) {};

void MELEventBusAddListener(Event event, void (* _Nullable listener)(void * _Nullable userdata, const int value), void * _Nullable userdata) {
#if CHECK_IF_ALREADY_LISTENING
    const int index = MELEventBusEntryListIndexOf(kInstance.listeners[event], (MELEventBusEntry) { .userdata = userdata });
    if (index >= 0) {
        playdate->system->error("The given userdata is already listening event %d", event);
    }
#endif
    MELEventBusEntryListPush(kInstance.listeners + event, (MELEventBusEntry) {
        .listener = {
            .EventListenerInt = listener
        },
        .userdata = userdata,
    });
}

void MELEventBusAddListenerVoidPointer(Event event, void (* _Nullable listener)(void * _Nullable userdata, const void * _Nullable value), void * _Nullable userdata) {
#if CHECK_IF_ALREADY_LISTENING
    const int index = MELEventBusEntryListIndexOf(kInstance.listeners[event], (MELEventBusEntry) { .userdata = userdata });
    if (index >= 0) {
        playdate->system->error("The given userdata is already listening event %d", event);
    }
#endif
    MELEventBusEntryListPush(kInstance.listeners + event, (MELEventBusEntry) {
        .listener = {
            .EventListenerVoidPointer = listener
        },
        .userdata = userdata,
    });
}

void MELEventBusRemoveListener(Event event, void * _Nullable userdata) {
    // NOTE: Gérer le cas où une même classe écoute plusieurs fois le même événement ?
    MELEventBusEntryListRemoveSwapEntry(kInstance.listeners + event, (MELEventBusEntry) { .userdata = userdata });
}

void MELEventBusRemoveListeners(void * _Nullable userdata) {
    for (Event event = 0; event < EventCount; event++) {
        MELEventBusRemoveListener(event, userdata);
    }
}

void MELEventBusFireEvent(Event event, int value) {
    const MELEventBusEntryList entries = kInstance.listeners[event];
    for (uint32_t index = 0; index < entries.count; index++) {
        const MELEventBusEntry entry = entries.memory[index];
        entry.listener.EventListenerInt(entry.userdata, value);
    }
}

void MELEventBusFireEventVoidPointer(Event event, const void * _Nullable value) {
    const MELEventBusEntryList entries = kInstance.listeners[event];
    for (uint32_t index = 0; index < entries.count; index++) {
        const MELEventBusEntry entry = entries.memory[index];
        entry.listener.EventListenerVoidPointer(entry.userdata, value);
    }
}
