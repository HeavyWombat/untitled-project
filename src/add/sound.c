#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include <light.h>
#include <ttext.h>
#include <sound.h>

// --- OpenAL Implementation ---

static ALCdevice* alDevice = NULL;
static ALCcontext* alContext = NULL;

static ALuint alSource0;
static ALuint alSource1;
static ALuint alSource2;

static ALuint alBufferShoot;
static ALuint alBufferReload;
static ALuint alBufferRain;
static ALuint alBufferThunder;

static int rainSoundRunning = 0;
static int rainSound = 1;

// --- ---

static const char* boolToString( int boolean )
{
  return boolean? "true": "false";
}

// Simple WAV loader for OpenAL
typedef struct {
    unsigned char* data;
    int size;
    int sampleRate;
    int channels;
    int bitsPerSample;
} WavData;

/* Create silent audio buffer as fallback */
static WavData* create_silent_wav(float duration) {
    WavData* wav = (WavData*)malloc(sizeof(WavData));
    if (!wav) {
        return NULL;
    }

    wav->sampleRate = 44100;
    wav->channels = 1;
    wav->bitsPerSample = 16;
    wav->size = (int)(duration * wav->sampleRate * wav->channels * (wav->bitsPerSample / 8));

    wav->data = (unsigned char*)calloc(wav->size, 1);  /* Silent audio = all zeros */
    if (!wav->data) {
        free(wav);
        return NULL;
    }

    return wav;
}

static WavData* loadWav(const char* filename) {
    FILE* fp;
    char path[512];
    WavData* wav = NULL;
    char tried_paths[512];

    /* Try different paths */
    fp = fopen(filename, "rb");
    snprintf(tried_paths, sizeof(tried_paths), "%s", filename);

    if (!fp) {
        snprintf(path, sizeof(path), "data/%s", filename);
        fp = fopen(path, "rb");
        snprintf(tried_paths, sizeof(tried_paths), "%s, %s", tried_paths, path);
        if (!fp) {
            snprintf(path, sizeof(path), "../data/%s", filename);
            fp = fopen(path, "rb");
            snprintf(tried_paths, sizeof(tried_paths), "%s, %s", tried_paths, path);
            if (!fp) {
                fprintf(stderr, "Warning: Failed to open WAV file (tried: %s)\n", tried_paths);
                fprintf(stderr, "         Using silent audio\n");
                return create_silent_wav(1.0f);  /* 1 second of silence */
            }
        }
    }

    wav = (WavData*)malloc(sizeof(WavData));
    if (!wav) {
        fclose(fp);
        fprintf(stderr, "Warning: Memory allocation failed for %s, using silent audio\n", filename);
        return create_silent_wav(1.0f);
    }

    /* Read RIFF header */
    char chunkID[4];
    unsigned int chunkSize;
    char format[4];

    if (fread(chunkID, 1, 4, fp) != 4 || fread(&chunkSize, 4, 1, fp) != 1 || fread(format, 1, 4, fp) != 4) {
        fprintf(stderr, "Warning: Failed to read WAV header for %s, using silent audio\n", filename);
        free(wav);
        fclose(fp);
        return create_silent_wav(1.0f);
    }

    if (memcmp(chunkID, "RIFF", 4) != 0 || memcmp(format, "WAVE", 4) != 0) {
        fprintf(stderr, "Warning: Invalid WAV file: %s, using silent audio\n", filename);
        free(wav);
        fclose(fp);
        return create_silent_wav(1.0f);
    }

    /* Read fmt chunk */
    if (fread(chunkID, 1, 4, fp) != 4 || fread(&chunkSize, 4, 1, fp) != 1) {
        fprintf(stderr, "Warning: Failed to read fmt chunk for %s, using silent audio\n", filename);
        free(wav);
        fclose(fp);
        return create_silent_wav(1.0f);
    }

    unsigned short audioFormat;
    unsigned short numChannels;
    unsigned int sampleRate;
    unsigned int byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;

    if (fread(&audioFormat, 2, 1, fp) != 1 ||
        fread(&numChannels, 2, 1, fp) != 1 ||
        fread(&sampleRate, 4, 1, fp) != 1 ||
        fread(&byteRate, 4, 1, fp) != 1 ||
        fread(&blockAlign, 2, 1, fp) != 1 ||
        fread(&bitsPerSample, 2, 1, fp) != 1) {
        fprintf(stderr, "Warning: Failed to read WAV format for %s, using silent audio\n", filename);
        free(wav);
        fclose(fp);
        return create_silent_wav(1.0f);
    }

    /* Skip to data chunk */
    if (fread(chunkID, 1, 4, fp) != 4 || fread(&chunkSize, 4, 1, fp) != 1) {
        fprintf(stderr, "Warning: Failed to find data chunk for %s, using silent audio\n", filename);
        free(wav);
        fclose(fp);
        return create_silent_wav(1.0f);
    }

    while (memcmp(chunkID, "data", 4) != 0 && !feof(fp)) {
        fseek(fp, chunkSize, SEEK_CUR);
        if (fread(chunkID, 1, 4, fp) != 4 || fread(&chunkSize, 4, 1, fp) != 1) {
            break;
        }
    }

    if (memcmp(chunkID, "data", 4) != 0) {
        fprintf(stderr, "Warning: No data chunk in WAV file: %s, using silent audio\n", filename);
        free(wav);
        fclose(fp);
        return create_silent_wav(1.0f);
    }

    wav->data = (unsigned char*)malloc(chunkSize);
    if (!wav->data) {
        free(wav);
        fclose(fp);
        fprintf(stderr, "Warning: Failed to allocate audio data for %s, using silent audio\n", filename);
        return create_silent_wav(1.0f);
    }

    if (fread(wav->data, 1, chunkSize, fp) != chunkSize) {
        fprintf(stderr, "Warning: Failed to read audio data for %s, using silent audio\n", filename);
        free(wav->data);
        free(wav);
        fclose(fp);
        return create_silent_wav(1.0f);
    }

    fclose(fp);

    wav->size = chunkSize;
    wav->sampleRate = sampleRate;
    wav->channels = numChannels;
    wav->bitsPerSample = bitsPerSample;

    printf("Successfully loaded: %s (%d Hz, %d channels, %d bits)\n",
           filename, sampleRate, numChannels, bitsPerSample);

    return wav;
}

static void freeWav(WavData* wav) {
    if (wav) {
        if (wav->data) {
            free(wav->data);
        }
        free(wav);
    }
}

static ALuint createBufferFromWav(const char* filename) {
    WavData* wav = loadWav(filename);
    if (!wav) {
        fprintf(stderr, "Warning: Failed to load %s, audio will be silent\n", filename);
        /* Create a minimal silent buffer */
        wav = create_silent_wav(0.1f);
        if (!wav) {
            return 0;
        }
    }

    ALenum format;
    if (wav->channels == 1) {
        format = (wav->bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    } else {
        format = (wav->bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    }

    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, wav->data, wav->size, wav->sampleRate);

    freeWav(wav);
    return buffer;
}

// --- ---

void sound_init( void )
{
    printf("\n=== Initializing Audio ===\n");

    /* Initialize OpenAL */
    alDevice = alcOpenDevice(NULL);
    if (!alDevice) {
        fprintf(stderr, "Warning: Failed to open OpenAL device, audio will be disabled\n");
        return;
    }

    alContext = alcCreateContext(alDevice, NULL);
    if (!alContext) {
        fprintf(stderr, "Warning: Failed to create OpenAL context, audio will be disabled\n");
        alcCloseDevice(alDevice);
        alDevice = NULL;
        return;
    }

    alcMakeContextCurrent(alContext);

    /* Create sources */
    alGenSources(1, &alSource0);
    alGenSources(1, &alSource1);
    alGenSources(1, &alSource2);

    /* Load sounds */
    alBufferShoot = createBufferFromWav("shoot.wav");
    alBufferReload = createBufferFromWav("reload.wav");
    alBufferRain = createBufferFromWav("rain.wav");
    alBufferThunder = createBufferFromWav("thunder.wav");

    /* Set up looping for rain */
    alSourcei(alSource1, AL_BUFFER, alBufferRain);
    alSourcei(alSource1, AL_LOOPING, AL_TRUE);

    printf("=== Audio Initialization Complete ===\n\n");
}

void sound_run( double timeDiff )
{
    static const double rainResetThreshold = 10.0;
    static double rainReset = 0.0;
    rainReset += timeDiff;

    // ---

    if( rainSound == 1 )
    {
        if( rainSoundRunning == 0 )
        {
            alSourcePlay(alSource1);
            rainSoundRunning = 1;
        }

        float vol = 0.3f;
        alSourcef(alSource1, AL_GAIN, vol);
    }
    else
    {
        if( rainSoundRunning == 1 )
        {
            alSourceStop(alSource1);
            rainSoundRunning = 0;
        }
    }

    // ---

    if( rainReset > rainResetThreshold )
    {
        alSourceStop(alSource1);
        rainSoundRunning = 0;

        rainReset = 0.0;
    }
}

void sound_shootSound( void )
{
    ALint state;
    alGetSourcei(alSource0, AL_SOURCE_STATE, &state);
    if( state == AL_PLAYING )
        alSourceStop(alSource0);

    alSourcei(alSource0, AL_BUFFER, alBufferShoot);
    alSourcef(alSource0, AL_GAIN, 1.0f);
    alSourcei(alSource0, AL_LOOPING, AL_FALSE);
    alSourcePlay(alSource0);
}

void sound_reloadSound( void )
{
    ALint state;
    alGetSourcei(alSource0, AL_SOURCE_STATE, &state);
    if( state == AL_PLAYING )
        alSourceStop(alSource0);

    alSourcei(alSource0, AL_BUFFER, alBufferReload);
    alSourcef(alSource0, AL_GAIN, 1.0f);
    alSourcei(alSource0, AL_LOOPING, AL_FALSE);
    alSourcePlay(alSource0);
}

void sound_thunderSound( void )
{
    ALint state;
    alGetSourcei(alSource2, AL_SOURCE_STATE, &state);
    if( state == AL_PLAYING )
        alSourceStop(alSource2);

    alSourcei(alSource2, AL_BUFFER, alBufferThunder);
    alSourcef(alSource2, AL_GAIN, 1.0f);
    alSourcei(alSource2, AL_LOOPING, AL_FALSE);
    alSourcePlay(alSource2);
}
