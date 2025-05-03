//
//  pdportal.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 05/10/2024.
//

#include "melstd.h"

#ifndef pdportal_h
#define pdportal_h

typedef struct {
    float time;
    char * _Nullable peerId;
    char * _Nullable remotePeerId;
    char * _Nullable lastData;
    char * _Nullable senderId;
    MELBoolean isSerialConnected;
    MELBoolean isWaitingForKeepAlive;
    MELBoolean isHost;
} MELPortal;

typedef enum {
    /**
     * Log the given arguments to the browser console.
     */
    MELPortalCommandLog,
    /**
     * Keepalive command, automatically sent by the Lua side of pdportal.
     */
    MELPortalCommandKeepAlive,
    /**
     * Tell pdportal to initialize connection to the peer server for communicating with another Playdate.
     * Not required for `fetch`.
     */
    MELPortalCommandInitializePeer,
    /**
     * Tell pdportal to disconnect from the peer server.
     */
    MELPortalCommandDestroyPeer,
    /**
     * Takes two strings, the destination peer ID, and a JSON string. Sends the JSON to that peer.
     */
    MELPortalCommandSendToPeerConnection,
    /**
     * Takes one string, the peer ID to close the connection to.
     */
    MELPortalCommandClosePeerConnection,
    /**
     * HTTP fetch.
     */
    MELPortalCommandFetch,
} MELPortalCommand;

extern MELPortal currentPortal;

void MELPortalRegister(void);
void MELPortalUnregister(void);

/**
 * Gère le keepalive.
 */
void MELPortalUpdate(void);

void MELPortalSendCommand(MELPortalCommand portalCommand, const char * _Nullable args);
void MELPortalSendToPeerConnection(const char * _Nonnull peerConnId, const char * _Nonnull payload);
void MELPortalLog(const char * _Nonnull value);

#endif /* pdportal_h */
