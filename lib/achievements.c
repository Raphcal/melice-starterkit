//
//  achievements.c
//  Montgolfier
//
//  Created by Raphaël Calabro on 23/05/2025.
//

#include "achievements.h"

#include "metadata.h"
#include "inputstream.h"
#include "outputstream.h"
#include "files.h"
#include "melstring.h"
#include "dictionary.h"
#include "melmath.h"

#include "achievementtoast.h"

#define kBufferSize 255

MELListImplement(MELAchievement);
MELListImplement(MELAchievementStatus);

typedef struct {
    MELOutputStream outputStream;
    uint8_t level;
    MELBoolean isFileStart;
    MELBoolean isFirst;
} MELJsonWriter;

typedef uint32_t MELHash;
MELDictionaryDefine(MELAchievementStatusRef);
MELDictionaryImplement(MELAchievementStatusRef, NULL);

static MELAchievementData achievementData = (MELAchievementData) {};
static MELAchievementStatusList achievementStatus = (MELAchievementStatusList) {};

#pragma mark - JSON Writer

static void writeRawString(MELOutputStream * _Nonnull self, const char * _Nonnull value) {
    const unsigned int count = (unsigned int) strlen(value);
    MELOutputStreamWrite(self, value, sizeof(char) * count);
}

static void writeIndent(MELJsonWriter * _Nonnull self) {
    const uint8_t level = self->level;
    for (uint8_t index = 0; index < level; index++) {
        writeRawString(&self->outputStream, "  ");
    }
}

static void writeObjectStart(MELJsonWriter * _Nonnull self) {
    if (!self->isFirst) {
        MELOutputStreamWriteInt8(&self->outputStream, ',');
    }
    self->isFirst = false;
    if (!self->isFileStart) {
        MELOutputStreamWriteInt8(&self->outputStream, '\n');
    }
    self->isFileStart = false;
    writeIndent(self);
    self->level++;
    self->isFirst = true;
    MELOutputStreamWriteInt8(&self->outputStream, '{');
}

static void writeObjectEnd(MELJsonWriter * _Nonnull self) {
    self->level--;
    self->isFirst = false;
    MELOutputStreamWriteInt8(&self->outputStream, '\n');
    writeIndent(self);
    writeRawString(&self->outputStream, "}");
}

static void writePropertyString(MELJsonWriter * _Nonnull self, const char * _Nonnull key, const char * _Nullable value) {
    if (value == NULL || *value == '\0') {
        return;
    }
    if (!self->isFirst) {
        MELOutputStreamWriteInt8(&self->outputStream, ',');
    }
    self->isFirst = false;
    MELOutputStreamWriteInt8(&self->outputStream, '\n');
    writeIndent(self);
    MELOutputStreamWriteInt8(&self->outputStream, '"');
    writeRawString(&self->outputStream, key);
    writeRawString(&self->outputStream, "\": \"");
    writeRawString(&self->outputStream, value);
    MELOutputStreamWriteInt8(&self->outputStream, '"');
}

static void writePropertyLocalizableString(MELJsonWriter * _Nonnull self, const char * _Nonnull key, const MELLocalizedString value) {
    if (currentLanguage == MELLanguageJapanese) {
        writePropertyString(self, key, value.text[MELLanguageEnglish]);
    } else {
        writePropertyString(self, key, MELLocalizedStringGetText(value));
    }
}

static void writePropertyBoolean(MELJsonWriter * _Nonnull self, const char * _Nonnull key, const MELBoolean value) {
    if (!value) {
        return;
    }
    if (!self->isFirst) {
        MELOutputStreamWriteInt8(&self->outputStream, ',');
    }
    self->isFirst = false;
    MELOutputStreamWriteInt8(&self->outputStream, '\n');
    writeIndent(self);
    MELOutputStreamWriteInt8(&self->outputStream, '"');
    writeRawString(&self->outputStream, key);
    writeRawString(&self->outputStream, "\": true");
}

static void writePropertyUInt32(MELJsonWriter * _Nonnull self, const char * _Nonnull key, const uint32_t value) {
    if (value == 0) {
        return;
    }
    if (!self->isFirst) {
        MELOutputStreamWriteInt8(&self->outputStream, ',');
    }
    self->isFirst = false;
    MELOutputStreamWriteInt8(&self->outputStream, '\n');
    writeIndent(self);
    MELOutputStreamWriteInt8(&self->outputStream, '"');
    writeRawString(&self->outputStream, key);
    writeRawString(&self->outputStream, "\": ");
    char buffer[11];
    MELUInt32ToStringWithFixedSizeBuffer(value, buffer, 11);
    writeRawString(&self->outputStream, buffer);
}

static void writePropertyArrayStart(MELJsonWriter * _Nonnull self, const char * _Nonnull key) {
    if (!self->isFirst) {
        MELOutputStreamWriteInt8(&self->outputStream, ',');
    }
    MELOutputStreamWriteInt8(&self->outputStream, '\n');
    writeIndent(self);
    self->level++;
    MELOutputStreamWriteInt8(&self->outputStream, '"');
    writeRawString(&self->outputStream, key);
    writeRawString(&self->outputStream, "\": [");
    self->isFirst = true;
}

static void writePropertyArrayEnd(MELJsonWriter * _Nonnull self) {
    MELOutputStreamWriteInt8(&self->outputStream, '\n');
    self->level--;
    self->isFirst = false;
    writeIndent(self);
    MELOutputStreamWriteInt8(&self->outputStream, ']');
}

#pragma mark - JSON reader

typedef enum {
    /// Attente de l'ouverture de l'objet parent.
    MELJsonReaderRegionOutside,
    MELJsonReaderRegionMain,
    MELJsonReaderRegionAchievementArray,
    MELJsonReaderRegionAchievement,
} MELJsonReaderRegion;

typedef enum {
    MELJsonReaderStatusInitial,
    MELJsonReaderStatusWaitingForKey,
    MELJsonReaderStatusKey,
    MELJsonReaderStatusWaitingForValue,
    MELJsonReaderStatusStringValue,
    MELJsonReaderStatusBooleanValue,
    MELJsonReaderStatusNumericValue,
    MELJsonReaderStatusArrayValue,
    MELJsonReaderStatusObjectValue,
} MELJsonReaderStatus;

static uint32_t readJsonUInt32Value(MELInputStream * _Nonnull self) {
    uint32_t value = 0;
    for (int c = MELInputStreamReadInt8(self); c != EOF && c >= '0' && c <= '9'; c = MELInputStreamReadInt8(self)) {
        value = value * 10 + (c - '0');
    }
#if DEBUG
    if (value == '.') {
        playdate->system->error("Unable to read achievements.json file, expected type: uint32_t, but was: float");
    }
#endif
    return value;
}
static int32_t readJsonInt32Value(MELInputStream * _Nonnull self) {
    uint32_t value = 0;
    int c = MELInputStreamReadInt8(self);
    const MELBoolean isNegative = c == '-';
    if (c != EOF && !isNegative) {
        value = c - '0';
    }
    for (; c != EOF && c >= '0' && c <= '9'; c = MELInputStreamReadInt8(self)) {
        value = value * 10 + (c - '0');
    }
#if DEBUG
    if (value == '.') {
        playdate->system->error("Unable to read achievements.json file, expected type: int32_t, but was: float");
    }
#endif
    if (isNegative) {
        value = -value;
    }
    return value;
}

static void readJsonString(MELInputStream * _Nonnull self, char * _Nullable outString, int maximumLength, unsigned int * _Nullable outLength) {
    MELBoolean lastWasEscape = false;
    int index = 0;
    int c = MELInputStreamReadUInt8(self);
    if (c == '"') {
        c = MELInputStreamReadUInt8(self);
    }
    for (; c != EOF; c = MELInputStreamReadUInt8(self)) {
        if (lastWasEscape && (c == '"' || c == '\\')) {
            if (index < maximumLength - 1) {
                outString[index++] = '\\';
            }
            lastWasEscape = false;
        }
        else if (c == '"') {
            break;
        }
        else if (c == '\\') {
            lastWasEscape = true;
        }
        else {
            if (lastWasEscape) {
                if (index < maximumLength - 1) {
                    outString[index++] = '\\';
                }
                lastWasEscape = false;
            }
            if (index < maximumLength - 1) {
                outString[index++] = c;
            }
        }
    }
    outString[MELIntMin(index, maximumLength - 1)] = '\0';
    if (outLength) {
        *outLength = index;
    }
}

static MELBoolean isBlank(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}
static MELBoolean isNumberStart(char c) {
    return c == '-' || (c >= '0' && c <= '9');
}
 
static int skipToNextItemStart(MELInputStream * _Nonnull self) {
     int c;
     for (c = MELInputStreamReadUInt8(self); c != EOF && c != '{' && c != '}' && c != '[' && c != '"'; c = MELInputStreamReadUInt8(self));
     return c;
}

static void skipToSeparator(MELInputStream * _Nonnull self) {
    for (int c = MELInputStreamReadInt8(self); c != ':'; c = MELInputStreamReadInt8(self)) {
        if (c == EOF || !isBlank(c)) {
            playdate->system->logToConsole("Unable to read JSON, expected: ':', but was: %c", c);
        }
    }
}

static int skipToValueStart(MELInputStream * _Nonnull self) {
    for (int c = MELInputStreamReadInt8(self); c != EOF; c = MELInputStreamReadInt8(self)) {
        switch (c) {
            case '{':
            case '[':
            case '"':
            case 't':
            case 'f':
            case 'n':
            case '0'...'9':
            case '-':
                MELInputStreamUnreadUInt8(self);
                return c;
            default:
                break;
        }
    }
    return EOF;
}

static void skipTrueOrNull(MELInputStream * _Nonnull self) {
    MELInputStreamSkipBytes(self, 4);
}
static void skipFalse(MELInputStream * _Nonnull self) {
    MELInputStreamSkipBytes(self, 5);
}
static void skipNumber(MELInputStream * _Nonnull self) {
    for (int c = MELInputStreamReadUInt8(self); c != EOF && (c == '-' ||  (c >= '0' && c <= '9')); c = MELInputStreamReadUInt8(self));
    MELInputStreamUnreadUInt8(self);
}
static void skipString(MELInputStream * _Nonnull self) {
    char buffer[255];
    readJsonString(self, buffer, 255, NULL);
}

static int skipValue(MELInputStream * _Nonnull self) {
    const int valueType = skipToValueStart(self);
    switch (valueType) {
        case 'n':
        case 't':
            skipTrueOrNull(self);
            break;
        case 'f':
            skipFalse(self);
            break;
        case '0'...'9':
        case '-':
            skipNumber(self);
            break;
        case '"':
            skipString(self);
            break;
        default:
            playdate->system->logToConsole("Unable to skip given valueType: %c. Parsing will likely fail.", valueType);
            break;
    }
    int c = MELInputStreamReadInt8(self);
    for (; c != EOF && c != ',' && c != ']' && c != '}'; c = MELInputStreamReadInt8(self));
    return c;
}

void parseFile(MELInputStream * _Nonnull self, const MELAchievementStatusRefDictionary statusDictionary) {
    // Recherche du début de l'objet.
    int nextItemStart = skipToNextItemStart(self);
    if (nextItemStart != '{') {
        // Valeur incorrecte : le JSON ne commence pas par un objet.
        return;
    }

    // Buffers
    char id[kBufferSize];
    char key[kBufferSize];
    memset(id, 0, kBufferSize);
    memset(key, 0, kBufferSize);

    // Recherche de la clef "achievements".
    nextItemStart = skipToNextItemStart(self);
    while (nextItemStart != EOF && nextItemStart != '}') {
        if (nextItemStart == '"') {
            readJsonString(self, key, kBufferSize, NULL);
        } else {
            // Valeur incorrecte.
            playdate->system->logToConsole("Unable to parse achievements.json, expected: '\"', but was: %c", nextItemStart);
            return;
        }
        skipToSeparator(self);
        if (strcmp(key, "achievements") == 0) {
            break;
        }
        skipValue(self);
        nextItemStart = skipToNextItemStart(self);
    }
    if (nextItemStart == EOF || nextItemStart == '}') {
        // Clef "achievements" non trouvée.
        playdate->system->logToConsole("Unable to parse achievements.json, 'achievements' property was not found. Expected: '\"', but was: %c", nextItemStart);
        return;
    }

    // Recherche du début du tableau "achievements".
    nextItemStart = skipToNextItemStart(self);
    if (nextItemStart != '[') {
        // Valeur incorrecte : le JSON ne commence pas par un objet.
        playdate->system->logToConsole("Unable to parse achievements.json, expected 'achievements' array start: '[', but was: %c", nextItemStart);
        return;
    }

    // Itération sur les succès contenus dans le tableau "achievements".
    nextItemStart = skipToNextItemStart(self);
    while (nextItemStart == '{') {
        nextItemStart = skipToNextItemStart(self);
        MELAchievementStatus status = (MELAchievementStatus) {};
        while (nextItemStart != EOF && nextItemStart != '}') {
            if (nextItemStart == '"') {
                readJsonString(self, key, kBufferSize, NULL);
            } else {
                // Valeur incorrecte.
                playdate->system->logToConsole("Unable to parse achievements.json, expected '\"' but was: %c", nextItemStart);
                return;
            }
            skipToSeparator(self);
            nextItemStart = skipToValueStart(self);
            if (strcmp(key, "id") == 0 && nextItemStart == '"') {
                readJsonString(self, id, kBufferSize, NULL);
            }
            else if (strcmp(key, "progress") == 0 && isNumberStart(nextItemStart)) {
                status.progress = readJsonInt32Value(self);
            }
            else if (strcmp(key, "grantedAt") == 0 && isNumberStart(nextItemStart)) {
                status.grantedAt = readJsonUInt32Value(self);
            } else if (nextItemStart != '}') {
                nextItemStart = skipValue(self);
            }
            if (nextItemStart != '}') {
                nextItemStart = skipToNextItemStart(self);
            }
        }
        MELAchievementStatus *statusRef = MELAchievementStatusRefDictionaryGet(statusDictionary, id);
        if (statusRef) {
            *statusRef = status;
        } else {
            playdate->system->logToConsole("Unknown achievement id found in achievements.json: %s", id);
        }
        playdate->system->logToConsole("Status of achievement \"%s\": progress: %d, grantedAt: %u", id, status.progress, status.grantedAt);
        nextItemStart = skipToNextItemStart(self);
    }
}

#pragma mark - File copy

static void copyToDirectory(const char * _Nullable file, const char * _Nonnull directory) {
    if (file == NULL) {
        return;
    }
    MELPathConcat(target, directory, file);
    char *lastSlash = strrchr(target, '/');
    if (lastSlash) {
        *lastSlash = '\0';
        playdate->file->mkdir(target);
        *lastSlash = '/';
    }
    if (MELFileExists(file) && !MELFileExists(target)) {
        MELFileCopy(file, target);
    }
}

#pragma mark - Achievement status

void MELAchievementSaveStatus(void) {
    if (!achievementData.achievements.count) {
        playdate->system->logToConsole("Unable to save achievements, uninitialized");
        return;
    }

    const MELMetadata metadata = MELMetadataGet();

    MELPathConcat(rootPath, "/Shared/achievements", metadata.bundleID);
    playdate->file->mkdir(rootPath);

    MELPathConcat(achievementsJsonPath, rootPath, "achievements.json");

    if (achievementData.cardPath) {
        MELPathAddExtension(cardPath, achievementData.cardPath, "pdi");
        copyToDirectory(cardPath, rootPath);
    }
    if (achievementData.iconPath) {
        MELPathAddExtension(iconPath, achievementData.iconPath, "pdi");
        copyToDirectory(iconPath, rootPath);
    }

    MELJsonWriter jsonWriter = (MELJsonWriter) {
        .outputStream = MELOutputStreamOpen(achievementsJsonPath),
        .isFirst = true,
        .isFileStart = true,
    };
    if (!jsonWriter.outputStream.file) {
        playdate->system->logToConsole("Unable open achievement file in write mode at path: %s", achievementsJsonPath);
        playdate->system->realloc(achievementsJsonPath, 0);
        playdate->system->realloc(rootPath, 0);
        return;
    }

    writeObjectStart(&jsonWriter);
    writePropertyString(&jsonWriter, "$schema", "https://github.com/PlaydateSquad/pd-achievements/raw/refs/heads/main/achievements.schema.json");
    writePropertyString(&jsonWriter, "specVersion", "1.0.0");
    writePropertyString(&jsonWriter, "gameID", metadata.bundleID);
    writePropertyString(&jsonWriter, "name", metadata.name);
    writePropertyString(&jsonWriter, "description", metadata.description);
    writePropertyString(&jsonWriter, "author", metadata.author);
    writePropertyString(&jsonWriter, "version", metadata.version);
    writePropertyString(&jsonWriter, "iconPath", "icon");
    writePropertyString(&jsonWriter, "cardPath", "card-achievements");
    writePropertyArrayStart(&jsonWriter, "achievements");

#if EXPORT_ACHIEVEMENT_ICONS
    MELPathConcat(iconRoot, rootPath, "AchievementImages");
#endif

    for (unsigned int index = 0; index < achievementData.achievements.count; index++) {
        const MELAchievement achievement = achievementData.achievements.memory[index];
        const MELAchievementStatus status = achievementStatus.memory[index];

        writeObjectStart(&jsonWriter);
        writePropertyString(&jsonWriter, "id", achievement.id);
        writePropertyLocalizableString(&jsonWriter, "name", achievement.name);
        writePropertyString(&jsonWriter, "description", achievement.description.text[MELLanguageEnglish]);
        writePropertyString(&jsonWriter, "descriptionLocked", achievement.descriptionLocked.text[MELLanguageEnglish]);
        writePropertyString(&jsonWriter, "icon", achievement.icon);
        writePropertyString(&jsonWriter, "iconLocked", achievement.iconLocked);
        writePropertyUInt32(&jsonWriter, "scoreValue", achievement.scoreValue);
        writePropertyBoolean(&jsonWriter, "isSecret", achievement.isSecret);
        writePropertyUInt32(&jsonWriter, "progressMax", achievement.progressMax);
        writePropertyBoolean(&jsonWriter, "progressIsPercentage", achievement.progressIsPercentage);
        writePropertyUInt32(&jsonWriter, "progress", status.progress);
        writePropertyUInt32(&jsonWriter, "grantedAt", status.grantedAt);
        writeObjectEnd(&jsonWriter);

#if EXPORT_ACHIEVEMENT_ICONS
        if (achievement.icon) {
            MELPathAddExtension(iconPath, achievement.icon, "pdi");
            copyToDirectory(iconPath, iconRoot);
        }
        if (achievement.iconLocked) {
            MELPathAddExtension(iconLockedPath, achievement.iconLocked, "pdi");
            copyToDirectory(iconLockedPath, iconRoot);
        }
#endif
    }

    writePropertyArrayEnd(&jsonWriter);
    writeObjectEnd(&jsonWriter);
    MELOutputStreamClose(&jsonWriter.outputStream);
}

void MELAchievementLoadStatus(const MELAchievementData data) {
    achievementData = data;
    const MELMetadata metadata = MELMetadataGet();

    if (metadata.bundleID == NULL) {
        playdate->system->logToConsole("Error loading achievements: unable to read bundleID from pdxinfo.");
        return;
    }

    MELPathConcat(rootPath, "/Shared/achievements", metadata.bundleID);
    MELPathConcat(achievementsJsonPath, rootPath, "achievements.json");

    playdate->system->logToConsole("Will read achievements from file: %s", achievementsJsonPath);

    const unsigned int achievementCount = data.achievements.count;
    MELAchievementStatusListEnsureCapacity(&achievementStatus, achievementCount);
    achievementStatus.count = achievementCount;
    memset(achievementStatus.memory, 0, sizeof(MELAchievementStatus) * achievementCount);

    MELAchievementStatusRefDictionary statusDictionary = (MELAchievementStatusRefDictionary) {};
    for (unsigned int index = 0; index < achievementCount; index++) {
        const char *id = data.achievements.memory[index].id;
        MELAchievementStatusRefDictionaryPut(&statusDictionary, id, achievementStatus.memory + index);
    }

    MELInputStream inputStream = MELInputStreamOpen(achievementsJsonPath, kFileReadData);
    if (inputStream.file) {
        playdate->system->logToConsole("Parsing achievements...");
        parseFile(&inputStream, statusDictionary);
    }
    MELInputStreamClose(&inputStream);
    MELAchievementStatusRefDictionaryDeinit(&statusDictionary);
}

void MELAchievementGrant(unsigned int achievementIndex) {
    MELAchievementGrantAt(achievementIndex, playdate->system->getSecondsSinceEpoch(NULL));
}

void MELAchievementGrantAt(unsigned int achievementIndex, uint32_t grantedAt) {
    if (achievementIndex >= achievementStatus.count) {
        playdate->system->logToConsole("Unable to grant achievement: bad index. Expected less than: %d, but was: %d", achievementStatus.count, achievementIndex);
        return;
    }
    MELAchievementStatus *status = achievementStatus.memory + achievementIndex;
    if (!status->grantedAt) {
        status->grantedAt = grantedAt;
        MELAchievementToastConstructor(achievementData.achievements.memory[achievementIndex]);
    }
}

void MELAchievementRevoke(unsigned int achievementIndex) {
    if (achievementIndex >= achievementStatus.count) {
        playdate->system->logToConsole("Unable to revoke achievement: bad index. Expected less than: %d, but was: %d", achievementStatus.count, achievementIndex);
        return;
    }
    achievementStatus.memory[achievementIndex].grantedAt = 0;
}

void MELAchievementAdvanceTo(unsigned int achievementIndex, int32_t advanceTo) {
    if (achievementIndex >= achievementStatus.count) {
        playdate->system->logToConsole("Unable to advance achievement: bad index. Expected less than: %d, but was: %d", achievementStatus.count, achievementIndex);
        return;
    }
    const MELAchievement achievement = achievementData.achievements.memory[achievementIndex];
    if (!achievement.progressMax) {
        playdate->system->logToConsole("Unable to advance achievement: achievement %d does has a progressMax value of 0.", achievementIndex);
        return;
    }
    MELAchievementStatus *status = achievementStatus.memory + achievementIndex;

    const int32_t progress = MELIntMin(advanceTo, achievement.progressMax);
    status->progress = progress;
    if (progress == achievement.progressMax) {
        status->grantedAt = playdate->system->getSecondsSinceEpoch(NULL);
        MELAchievementToastConstructor(achievement);
    } else {
        status->grantedAt = 0;
    }
}
