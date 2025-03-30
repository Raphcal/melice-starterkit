//
//  animationdefinition.c
//  shmup
//
//  Created by Raphaël Calabro on 28/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "animationdefinition.h"

#include "primitives.h"

MELListImplement(MELAnimationDefinition);

MELAnimationDefinition MELAnimationDefinitionMakeWithAnimationDefinition(MELAnimationDefinition other) {
    MELAnimationDefinition self = other;
    self.frames = MELArrayCopy(other.frames, sizeof(MELAnimationFrame) * other.frameCount);
    return self;
}

MELAnimationDefinition MELAnimationDefinitionMakeWithInputStream(MELInputStream * _Nonnull inputStream) {
    MELAnimationDefinition self;
    /* name = */ MELInputStreamReadString(inputStream);
    self.frequency = MELInputStreamReadInt(inputStream);
    const MELBoolean looping = MELInputStreamReadBoolean(inputStream);
    
    // Directions et frames
    const int32_t directionCount = MELInputStreamReadInt(inputStream);
    
    MELAnimationFrame * _Nullable mainFrames = NULL;
    int32_t mainFrameCount = 0;
    
    for (int32_t direction = 0; direction < directionCount; direction++) {
        const double key = MELInputStreamReadDouble(inputStream);
        
        // Frames
        const int32_t frameCount = MELInputStreamReadInt(inputStream);
        MELAnimationFrame * _Nonnull frames = playdate->system->realloc(NULL, frameCount * sizeof(MELAnimationFrame));
        
        for (int32_t frameIndex = 0; frameIndex < frameCount; frameIndex++) {
            frames[frameIndex] = MELAnimationFrameMakeWithInputStream(inputStream);
        }
        
        if (key == 0 || mainFrames == NULL) {
            if (mainFrames != NULL) {
                playdate->system->realloc(mainFrames, 0);
            }
            mainFrames = frames;
            mainFrameCount = frameCount;
        } else {
            playdate->system->realloc(frames, 0);
        }
    }
    
    self.frameCount = mainFrameCount;
    self.frames = mainFrames;
    self.type = MELAnimationTypeForFrameCountAndLooping(mainFrameCount, looping);
    return self;
}

MELTimeInterval MELAnimationDefinitionDuration(MELAnimationDefinition self) {
    return (MELTimeInterval)self.frameCount / self.frequency;
}

void MELAnimationDefinitionDeinit(MELAnimationDefinition *_Nonnull self) {
	free(self->frames);
	self->frames = NULL;
	self->frameCount = 0;
	self->frequency = 0;
	self->type = MELAnimationTypeNone;
}
