//
//  constspritedefinition.c
//  Roll
//
//  Created by Raphaël Calabro on 24/04/2022.
//

#include "spritedefinition.h"

#include "noanimation.h"

MELAnimationDefinition * _Nullable MELSpriteDefinitionGetAnimationDefinition(MELSpriteDefinition self, unsigned int animationName, MELAnimationDirection direction) {
    MELAnimationDefinition *definition = self.animations[animationName * MELAnimationDirectionCount + direction];
    return definition != NULL ? definition : self.animations[animationName * MELAnimationDirectionCount + MELAnimationDirectionRight];
}

MELAnimation * _Nullable MELSpriteDefinitionGetAnimation(MELSpriteDefinition self, unsigned int animationName, MELAnimationDirection direction) {
    MELAnimationDefinition *definition = MELSpriteDefinitionGetAnimationDefinition(self, animationName, direction);
    if (definition != NULL) {
        MELAnimation *animation = MELAnimationAlloc(definition);
        animation->class->start(animation);
        return animation;
    } else {
        return MELNoAnimationAlloc(NULL);
    }
}

void MELSpriteDefinitionFreePalette(MELSpriteDefinition * _Nonnull self) {
    if (self->palette) {
        playdate->graphics->freeBitmapTable(self->palette);
        self->palette = NULL;
    }
}
