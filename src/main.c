//
//  main.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 22/01/2023.
//

#include "common.h"


PlaydateAPI * _Nullable playdate;
LCDFont * _Nullable roobertFont;

int REFRESH_RATE = 50;
float DELTA;

void setRefreshRate(const int refreshRate) {
    DELTA = 1.0f / refreshRate;
    playdate->display->setRefreshRate(refreshRate);
}

static void init(void) {
    playdate->graphics->setBackgroundColor(kColorWhite);
    // TODO: MELSceneMakeCurrent de la scène de titre
}

static void loadFonts(void) {
    roobertFont = LCDFontLoadOrError("fonts/font-roobert");
}

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI * _Nonnull api, PDSystemEvent event, uint32_t arg) {
    switch (event) {
        case kEventInit:
            playdate = api;
            setRefreshRate(DEFAULT_REFRESH_RATE);
            MELRandomInit();
            loadFonts();
            init();
            break;
        case kEventTerminate:
        case kEventLowPower:
            api->system->logToConsole("Terminate (%d) or low power event (%d) received: %d", kEventTerminate, kEventLowPower, event);
            // TODO: Sauvegarder
            break;
        case kEventPause:
            // TODO: Afficher la carte via setMenuImage
            break;
        default:
            break;
    }
    return 0;
}
