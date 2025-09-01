//
//  pdportal.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 05/10/2024.
//

#include "pdportal.h"

#include "primitives.h"
#include "melstring.h"
#include "melmath.h"

#include "eventbus.h"

extern float DELTA;

static const char * _Nonnull MELPortalCommandValue[] = {
    "l",
    "k",
    "ip",
    "dp",
    "p",
    "cpc",
    "f"
};

typedef enum {
    MELPortalResponseCommandOnConnect,
    MELPortalResponseCommandOnPeerConnData,
    MELPortalResponseCommandOnPeerOpen,
    MELPortalResponseCommandOnPeerClose,
    MELPortalResponseCommandOnPeerConnection,
    MELPortalResponseCommandOnPeerConnOpen,
    MELPortalResponseCommandOnPeerConnClose,
    MELPortalResponseCommandKeepAlive,
    MELPortalResponseCommandOnFetchSuccess,
    MELPortalResponseCommandOnFetchError,
    MELPortalResponseCommandBadCommand
} MELPortalResponseCommand;

static const uint8_t MELPortalResponseCommandValueLength[] = {
    // oc~,~
    5,
    // opcd~,~
    7,
    // opo~,~
    6,
    // opc~,~
    6,
    // opconn~,~
    9,
    // opco~,~
    7,
    // opcc~,~
    7,
    // k (pas d'argument — sûrement !)
    1,
    // ofs~,~
    6,
    // ofe~,~
    6,
};


#define portalCommandSeparator "\x1E"
#define portalArgumentSeparator "\x1F"

#define playdateCommandSeparator "~|~"
#define playdateArgumentSeparator "~,~"
static const int playdateCommandSeparatorLength = 3;

static const int quoteLength = 1;

#define KEEP_ALIVE_INTERVAL 0.5f
#define KEEP_ALIVE_RESPONSE_MAXIMUM_WAIT 0.1f

MELPortal currentPortal = (MELPortal) {};

#define BUFFER_LENGTH 256
static char buffer[BUFFER_LENGTH] = {};

#pragma mark - Méthode privées

void MELPortalSendCommand(MELPortalCommand portalCommand, const char * _Nullable args) {
    if (args) {
        playdate->system->logToConsole("%s" portalArgumentSeparator "\"%s\"" portalCommandSeparator, MELPortalCommandValue[portalCommand], args);
    } else {
        playdate->system->logToConsole("%s" portalCommandSeparator, MELPortalCommandValue[portalCommand]);
    }
}

static void onKeepAlive(void) {
    currentPortal.time = 0.0f;
    currentPortal.isWaitingForKeepAlive = false;
}

static MELPortalResponseCommand responseForCommandString(const char * _Nonnull command) {
    switch (command[0]) {
        case 'o':
            switch (command[1]) {
                case 'c':
                    if (!strncmp(command + 2, playdateArgumentSeparator, playdateCommandSeparatorLength)) {
                        return MELPortalResponseCommandOnConnect;
                    }
                    break;
                case 'p':
                    switch (command[2]) {
                        case 'c':
                            switch (command[3]) {
                                case 'd':
                                    if (!strncmp(command + 4, playdateArgumentSeparator, playdateCommandSeparatorLength)) {
                                        return MELPortalResponseCommandOnPeerConnData;
                                    }
                                    break;
                                case '~':
                                    if (!strncmp(command + 3, playdateArgumentSeparator, playdateCommandSeparatorLength)) {
                                        return MELPortalResponseCommandOnPeerClose;
                                    }
                                    break;
                                case 'o':
                                    if (!strncmp(command + 4, "nn" playdateArgumentSeparator, playdateCommandSeparatorLength + 2)) {
                                        return MELPortalResponseCommandOnPeerConnection;
                                    } else if (!strncmp(command + 4, playdateArgumentSeparator, playdateCommandSeparatorLength)) {
                                        return MELPortalResponseCommandOnPeerConnOpen;
                                    }
                                    break;
                                case 'c':
                                    if (!strncmp(command + 4, playdateArgumentSeparator, playdateCommandSeparatorLength)) {
                                        return MELPortalResponseCommandOnPeerConnClose;
                                    }
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case 'o':
                            if (!strncmp(command + 3, playdateArgumentSeparator, playdateCommandSeparatorLength)) {
                                return MELPortalResponseCommandOnPeerOpen;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case 'f':
                    switch (command[2]) {
                        case 's':
                            if (!strncmp(command + 3, playdateArgumentSeparator, playdateCommandSeparatorLength)) {
                                return MELPortalResponseCommandOnFetchSuccess;
                            }
                            break;
                        case 'e':
                            if (!strncmp(command + 3, playdateArgumentSeparator, playdateCommandSeparatorLength)) {
                                return MELPortalResponseCommandOnFetchError;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            break;
        case 'k':
            if (!strncmp(command + 1, playdateCommandSeparator, playdateCommandSeparatorLength)) {
                return MELPortalResponseCommandKeepAlive;
            }
            break;
        default:
            break;
    }
    playdate->system->logToConsole("l" portalArgumentSeparator "Unsupported command: %s" portalCommandSeparator, command);
    return MELPortalResponseCommandBadCommand;
}

static void callResponseCallback(MELPortalResponseCommand command, const char * _Nonnull arguments, int length) {
    int firstSeparatorIndex;
    char *data;
    if (length > 0) {
        // Copie obligatoire car le message donné par l'API est une constante.
        const int count = MELIntMin(length, BUFFER_LENGTH - 1);
        memcpy(buffer, arguments, count);
        buffer[count] = '\0';
    } else {
        buffer[0] = '\0';
    }
    switch (command) {
        case MELPortalResponseCommandOnConnect:
#if MELLogPortal
            playdate->system->logToConsole("l" portalArgumentSeparator "Connected to PDPortal version %s!" portalCommandSeparator, buffer);
#endif
            currentPortal.isSerialConnected = true;
            MELPortalSendCommand(MELPortalCommandInitializePeer, NULL);
            MELEventBusFireEvent(EventPDPortalOnConnect, 0);
            break;

        case MELPortalResponseCommandOnPeerConnData:
            firstSeparatorIndex = MELStringIndexOfString(buffer, playdateArgumentSeparator);
            buffer[firstSeparatorIndex] = '\0';
            data = buffer + firstSeparatorIndex + playdateCommandSeparatorLength + quoteLength;
            data[strlen(data) - quoteLength] = '\0';

            currentPortal.senderId = buffer;
            currentPortal.lastData = data;
            MELEventBusFireEvent(EventPDPortalOnPeerConnData, 0);
            currentPortal.senderId = NULL;
            currentPortal.lastData = NULL;
            break;

        case MELPortalResponseCommandOnPeerOpen:
#if MELLogPortal
            playdate->system->logToConsole("l" portalArgumentSeparator "Peer opened, peerId: %s" portalCommandSeparator, buffer);
#endif
            currentPortal.peerId = MELStringCopy(buffer);
            MELEventBusFireEvent(EventPDPortalOnPeerOpen, 0);
            break;

        case MELPortalResponseCommandOnPeerClose:
            if (currentPortal.peerId) {
                playdate->system->realloc(currentPortal.peerId, 0);
                currentPortal.peerId = NULL;
            }
            MELEventBusFireEvent(EventPDPortalOnPeerClose, 0);
            break;

        case MELPortalResponseCommandOnPeerConnection:
#if MELLogPortal
            playdate->system->logToConsole("l" portalArgumentSeparator "Hosting game, peer connected, remotePeerId: %s" portalCommandSeparator, buffer);
#endif
            // NOTE: Déplacer ça dans la logique du jeu pour gérer plus de 2 joueurs ?
            if (currentPortal.remotePeerId && !MELStringEquals(currentPortal.remotePeerId, buffer)) {
                MELPortalLog("Only 2 players supported!");
                MELPortalSendCommand(MELPortalCommandClosePeerConnection, buffer);
                break;
            }
            currentPortal.remotePeerId = MELStringCopy(buffer);
            currentPortal.isHost = true;

            MELEventBusFireEvent(EventPDPortalOnPeerConnection, true);
            break;

        case MELPortalResponseCommandOnPeerConnOpen:
#if MELLogPortal
            playdate->system->logToConsole("l" portalArgumentSeparator "Connected to remote peer, remotePeerId: %s" portalCommandSeparator, buffer);
#endif
            currentPortal.remotePeerId = MELStringCopy(buffer);
            currentPortal.isHost = false;
            MELEventBusFireEvent(EventPDPortalOnPeerConnOpen, false);
            break;

        case MELPortalResponseCommandOnPeerConnClose:
            if (MELStringEquals(currentPortal.remotePeerId, buffer)) {
                // Fermeture d'une autre connexion. Pas besoin de prévenir le jeu.
                break;
            }
            if (currentPortal.remotePeerId) {
                playdate->system->realloc(currentPortal.remotePeerId, 0);
                currentPortal.remotePeerId = NULL;
            }
            MELEventBusFireEvent(EventPDPortalOnPeerConnClose, 0);
            break;

        case MELPortalResponseCommandKeepAlive:
            onKeepAlive();
            break;

        case MELPortalResponseCommandOnFetchSuccess:
        case MELPortalResponseCommandOnFetchError:
        default:
            playdate->system->logToConsole("l" portalArgumentSeparator "Unsupported command: %d, args: %s" portalCommandSeparator, command, buffer);
            break;
    }
}

static void onSerialMessageReceived(const char * _Nonnull message) {
#if MELLogPortal
    playdate->system->logToConsole("l" portalArgumentSeparator "Received message: '%s'" portalCommandSeparator, message);
#endif

    int separatorIndex = MELStringIndexOfString(message, playdateCommandSeparator);
    if (separatorIndex >= 0) {
        const MELPortalResponseCommand responseCommand = responseForCommandString(message);
        if (responseCommand == MELPortalResponseCommandBadCommand) {
            return;
        }
        const int responseCommandLength = MELPortalResponseCommandValueLength[responseCommand];
        callResponseCallback(responseCommand, message + responseCommandLength, separatorIndex - responseCommandLength);
    }
    if (separatorIndex + 3 < strlen(message)) {
        playdate->system->logToConsole("l" portalArgumentSeparator "Trailing commands are not supported yet. Separator index: %d, len: %d" portalCommandSeparator, separatorIndex, strlen(message));
    }
}

#pragma mark - Méthodes publiques

void MELPortalRegister(void) {
    currentPortal = (MELPortal) {};
    playdate->system->setSerialMessageCallback(onSerialMessageReceived);
}

void MELPortalUnregister(void) {
    playdate->system->setSerialMessageCallback(NULL);
    currentPortal = (MELPortal) {};
}

void MELPortalUpdate(void) {
    if (!currentPortal.isSerialConnected) {
        // Sortie rapide pour éviter de prendre du temps de jeu en solo.
        return;
    }
    const float time = currentPortal.time += DELTA;
    if (currentPortal.isWaitingForKeepAlive && time >= KEEP_ALIVE_RESPONSE_MAXIMUM_WAIT) {
#if MELLogPortal
        playdate->system->logToConsole("l" portalArgumentSeparator "Keep alive not received after %fs" portalCommandSeparator, (double) time);
#endif
        currentPortal.isSerialConnected = false;
        if (currentPortal.remotePeerId) {
            playdate->system->realloc(currentPortal.remotePeerId, 0);
            currentPortal.remotePeerId = NULL;
            MELEventBusFireEvent(EventPDPortalOnPeerConnClose, 0);
        }
        if (currentPortal.peerId) {
            playdate->system->realloc(currentPortal.peerId, 0);
            currentPortal.peerId = NULL;
            MELEventBusFireEvent(EventPDPortalOnPeerClose, 0);
        }
        MELEventBusFireEvent(EventPDPortalOnDisconnect, 0);
    } else if (time >= KEEP_ALIVE_INTERVAL) {
        currentPortal.isWaitingForKeepAlive = true;
        currentPortal.time = 0.0f;
        playdate->system->logToConsole("k" portalCommandSeparator);
    }
}

void MELPortalLog(const char * _Nonnull value) {
    playdate->system->logToConsole("l" portalArgumentSeparator "%s" portalCommandSeparator, value);
}

void MELPortalSendToPeerConnection(const char * _Nonnull peerConnId, const char * _Nonnull payload) {
    playdate->system->logToConsole("p" portalArgumentSeparator "%s" portalArgumentSeparator "\"%s\"" portalCommandSeparator, peerConnId, payload);
}
