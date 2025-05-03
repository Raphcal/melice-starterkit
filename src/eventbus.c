//
//  eventbus.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 17/12/2023.
//

#include "eventbus.h"

static MELBoolean EventBusEntryEquals(EventBusEntry lhs, EventBusEntry rhs) {
    return lhs.userdata == rhs.userdata;
}

MELListImplement(EventBusEntry);
MELListImplementIndexOfWithEqualsFunction(EventBusEntry, EventBusEntryEquals);

static EventBus kInstance = (EventBus) {};

void EventBusAddListener(Event event, EventListener listener, void * _Nullable userdata) {
#if CHECK_IF_ALREADY_LISTENING
    const int index = EventBusEntryListIndexOf(kInstance.listeners[event], (EventBusEntry) { .userdata = userdata });
    if (index >= 0) {
        playdate->system->error("The given userdata is already listening event %d", event);
    }
#endif
    EventBusEntryListPush(kInstance.listeners + event, (EventBusEntry) {
        .listener = listener,
        .userdata = userdata,
    });
}

void EventBusRemoveListener(Event event, void * _Nullable userdata) {
    // NOTE: Gérer le cas où une même classe écoute plusieurs fois le même événement ?
    EventBusEntryListRemoveSwapEntry(kInstance.listeners + event, (EventBusEntry) { .userdata = userdata });
}

void EventBusRemoveListeners(void * _Nullable userdata) {
    for (Event event = 0; event < EventCount; event++) {
        EventBusRemoveListener(event, userdata);
    }
}

void EventBusFireEvent(Event event, int value) {
    const EventBusEntryList entries = kInstance.listeners[event];
    for (uint32_t index = 0; index < entries.count; index++) {
        const EventBusEntry entry = entries.memory[index];
        entry.listener(entry.userdata, value);
    }
}
