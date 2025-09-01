//
//  eventbus.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 17/12/2023.
//

#ifndef eventbus_h
#define eventbus_h

#include "melstd.h"

#include "list.h"
#include "../src/events.h"

typedef union {
    void (* _Nullable EventListenerInt)(void * _Nullable userdata, const int value);
    void (* _Nullable EventListenerVoidPointer)(void * _Nullable userdata, const void * _Nullable value);
} MELEventListener;

typedef struct {
    MELEventListener listener;
    void * _Nullable userdata;
} MELEventBusEntry;

MELListDefine(MELEventBusEntry);
MELListDefineIndexOf(MELEventBusEntry);

typedef struct {
    MELEventBusEntryList listeners[EventCount];
} MELEventBus;

void MELEventBusAddListener(Event event, void (* _Nullable listener)(void * _Nullable userdata, const int value), void * _Nullable userdata);
void MELEventBusAddListenerVoidPointer(Event event, void (* _Nullable listener)(void * _Nullable userdata, const void * _Nullable value), void * _Nullable userdata);
void MELEventBusRemoveListener(Event event, void * _Nullable userdata);
void MELEventBusRemoveListeners(void * _Nullable userdata);
void MELEventBusFireEvent(Event event, const int value);
void MELEventBusFireEventVoidPointer(Event event, const void * _Nullable value);

#endif /* eventbus_h */
