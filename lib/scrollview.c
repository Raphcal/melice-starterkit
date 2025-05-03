//
//  scrollview.c
//  Montgolfier
//
//  Created by Raphaël Calabro on 31/03/2025.
//

#include "scrollview.h"

#include "sprite.h"
#include "camera.h"
#include "controller.h"

static const float kMaximumScrollSpeed = 400.0f;
static const float kScrollAcceleration = 600.0f;

typedef struct {
    MELSprite super;
    MELSize innerSize;
    MELPoint camera;
    LCDSpriteRefList content;
    float scrollSpeed;
    MELBoolean enableScrolling;
    MELBoolean positionFixed;
    MELBoolean autoResize;
} MELScrollView;

static void update(LCDSprite * _Nonnull sprite);
static void draw(LCDSprite * _Nonnull sprite, PDRect bounds, PDRect drawrect);

static const MELSpriteClass MELScrollViewClass = (MELSpriteClass) {
    .destroy = MELSpriteDealloc,
    .update = update,
};

LCDSprite * _Nonnull MELScrollViewMake(MELRectangle frame, MELSize innerSize) {
    MELScrollView *self = playdate->system->realloc(NULL, sizeof(MELScrollView));

    *self = (MELScrollView) {
        .super = {
            .class = &MELScrollViewClass,
            .frame = frame,
        },
        .innerSize = innerSize,
        .enableScrolling = true,
    };

    LCDBitmap *image = playdate->graphics->newBitmap(frame.size.width, frame.size.height, kColorClear);
    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->setDrawFunction(sprite, draw);
    playdate->sprite->moveTo(sprite, frame.origin.x - camera.frame.origin.x, frame.origin.y - camera.frame.origin.y);
    playdate->sprite->addSprite(sprite);
    return sprite;
}

void MELScrollViewAddSprite(LCDSprite * _Nonnull scrollViewSprite, LCDSprite * _Nonnull spriteToAdd) {
    MELScrollView *self = playdate->sprite->getUserdata(scrollViewSprite);
    playdate->sprite->removeSprite(spriteToAdd);
    LCDSpriteRefListPush(&self->content, spriteToAdd);

    if (self->autoResize) {
        // MELSprite *other = playdate->sprite->getUserdata(spriteToAdd);
        // TODO: Agrandir la taille
    }
}

static void update(LCDSprite * _Nonnull sprite) {
    MELScrollView *self = playdate->sprite->getUserdata(sprite);

    // Gestion du scrolling
    if (self->enableScrolling) {
        MELController controller = MELControllerMake();
        // TODO: Gérer le scrolling horizontal ?
        if (controller.axe.y < 0) {
            self->scrollSpeed = MELFloatMax(self->scrollSpeed - kScrollAcceleration * DELTA, -kMaximumScrollSpeed);
        } else if (controller.axe.y > 0) {
            self->scrollSpeed = MELFloatMin(self->scrollSpeed + kScrollAcceleration * DELTA, kMaximumScrollSpeed);
        } else {
            self->scrollSpeed = 0.0f;
        }
        const float originYPlusCrank = self->camera.y + MELCrankGetAcceleratedChange() + self->scrollSpeed * DELTA;
        const float bottom = self->innerSize.height - self->super.frame.size.height;
        self->camera.y = MELFloatBound(0, originYPlusCrank, bottom);
    }

    // Mise à jour des sprites
    LCDSpriteRefList sprites = self->content;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        melSprite->class->update(sprite);
    }
}

static void draw(LCDSprite * _Nonnull sprite, PDRect bounds, PDRect drawrect) {
    MELScrollView *self = playdate->sprite->getUserdata(sprite);

    const MELPoint camera = self->camera;

    // Affichage des sprites
    LCDSpriteRefList sprites = self->content;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        LCDBitmap *image = playdate->sprite->getImage(sprite);
        const MELRectangle frame = LCDSpriteGetFrame(sprite);
        // TODO: Gérer le flip ?
        playdate->graphics->drawBitmap(image, MELRectangleOriginIsCenterGetLeft(frame) - camera.x, MELRectangleOriginIsCenterGetTop(frame) - camera.y, kBitmapUnflipped);
    }

    // TODO: Dessiner la scrollbar
}
