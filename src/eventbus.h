//
//  eventbus.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 17/12/2023.
//

#ifndef eventbus_h
#define eventbus_h

#include "common.h"
#include "../lib/melice.h"

typedef enum {
    EventLifeLost,
    EventDisappeared,
    EventTakeOff,
    EventGhostStepAdded,
    EventMapChanged,

    /** La langue du jeu a changé. */
    EventLanguageChanged,
    /** Indique que le dialogue affiché actuellement a changé. */
    EventDialogChanged,
    /** Indique que le dialogue est en pause. */
    EventDialogPaused,
    /** Indique que le joueur a appuyé sur A pour continuer le dialogue. */
    EventDialogResumed,
    /** Indique que le dialogue est terminé. */
    EventDialogHasEnded,

    /**
     * Called by pdportal web app after serial connection established. Example
     * handler: Update UI to show that the serial connection was successful.
     */
    EventPDPortalOnConnect,
    /**
     * Called by Lua code when serial keepalive fails. Example
     * handler: Update UI to show that the serial has been disconnected.
     */
    EventPDPortalOnDisconnect,
    /**
     * Peer connection established, remote peer connection possible. Example
     * handler: Update UI to show this Playdate's peer ID. Other users can enter
     * the ID to connect.
     * @see https://peerjs.com/docs/#peeron-open
     */
    EventPDPortalOnPeerOpen,
    /**
     * Peer connection destroyed, remote peer connection no longer possible.
     * Example handler: Update UI to show that peer connection is no longer
     * possible. Prompt user to reconnect?
     * @see https://peerjs.com/docs/#peeron-close
     */
    EventPDPortalOnPeerClose,
    /**
     * A remote peer has connected to us via the peer server. The given
     * `remotePeerId` can be used to send data to this peer with
     * `PdPortal.PortalCommand.SendToPeerConn`. Example handler: Start game!
     * @see https://peerjs.com/docs/#peeron-connection
     */
    EventPDPortalOnPeerConnection,
    /**
     * We've connected to a remote peer via the peer server. The given
     * `remotePeerId` can be used to send data to this peer with
     * `PdPortal.PortalCommand.SendToPeerConn`. Example handler: Start game!
     * @see https://peerjs.com/docs/#dataconnection-on-open
     */
    EventPDPortalOnPeerConnOpen,
    /**
     * Connection to a remote peer has been lost. Example handler: End game.
     * @see https://peerjs.com/docs/#dataconnection-on-close
     */
    EventPDPortalOnPeerConnClose,
    /**
     * We've received data from a remote peer. Example handler: Read `payload` and
     * act appropriately based on the `remotePeerId` and current app state. Note
     * that `payload` is JSON-encoded, so you probably want to
     * `local decodedPayload = json.decode(payload)` before use.
     * @see https://peerjs.com/docs/#dataconnection-on-data
     * @see https://sdk.play.date/inside-playdate#M-json
     */
    EventPDPortalOnPeerConnData,
    EventCount
} Event;

typedef void (* _Nullable EventListener)(void * _Nullable userdata, int value);

typedef struct {
    EventListener listener;
    void * _Nullable userdata;
} EventBusEntry;

MELListDefine(EventBusEntry);
MELListDefineIndexOf(EventBusEntry);

typedef struct {
    EventBusEntryList listeners[EventCount];
} EventBus;

void EventBusAddListener(Event event, EventListener listener, void * _Nullable userdata);
void EventBusRemoveListener(Event event, void * _Nullable userdata);
void EventBusRemoveListeners(void * _Nullable userdata);
void EventBusFireEvent(Event event, int value);

#endif /* eventbus_h */
