#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <AudioToolbox/AudioToolbox.h>
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

#ifdef __APPLE__
/* Decode any WAV variant CoreAudio understands (PCM, Microsoft ADPCM,
   WAVE_FORMAT_EXTENSIBLE, ...) into interleaved signed 16-bit PCM. */
static WavData* loadWavWithCoreAudio(const char* path, const char* filename) {
    CFURLRef url;
    ExtAudioFileRef file = NULL;
    AudioStreamBasicDescription fileFormat;
    AudioStreamBasicDescription pcmFormat;
    UInt32 propertySize;
    SInt64 totalFrames = 0;
    const UInt32 framesPerRead = 16384;
    unsigned char* samples = NULL;
    size_t capacity = 0;
    size_t used = 0;
    WavData* wav;

    url = CFURLCreateFromFileSystemRepresentation(NULL, (const UInt8*)path, strlen(path), false);
    if (!url) {
        fprintf(stderr, "Warning: Failed to open WAV file for decoding: %s, using silent audio\n", filename);
        return create_silent_wav(1.0f);
    }

    if (ExtAudioFileOpenURL(url, &file) != noErr) {
        CFRelease(url);
        fprintf(stderr, "Warning: Failed to decode WAV file: %s, using silent audio\n", filename);
        return create_silent_wav(1.0f);
    }

    CFRelease(url);

    propertySize = sizeof(fileFormat);
    if ((ExtAudioFileGetProperty(file, kExtAudioFileProperty_FileDataFormat, &propertySize, &fileFormat) != noErr) ||
        (fileFormat.mChannelsPerFrame == 0)) {
        ExtAudioFileDispose(file);
        fprintf(stderr, "Warning: Failed to read WAV format for %s, using silent audio\n", filename);
        return create_silent_wav(1.0f);
    }

    memset(&pcmFormat, 0, sizeof(pcmFormat));
    pcmFormat.mSampleRate = fileFormat.mSampleRate;
    pcmFormat.mFormatID = kAudioFormatLinearPCM;
    pcmFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    pcmFormat.mBitsPerChannel = 16;
    pcmFormat.mChannelsPerFrame = fileFormat.mChannelsPerFrame;
    pcmFormat.mFramesPerPacket = 1;
    pcmFormat.mBytesPerFrame = 2 * pcmFormat.mChannelsPerFrame;
    pcmFormat.mBytesPerPacket = pcmFormat.mBytesPerFrame;

    if (ExtAudioFileSetProperty(file, kExtAudioFileProperty_ClientDataFormat, sizeof(pcmFormat), &pcmFormat) != noErr) {
        ExtAudioFileDispose(file);
        fprintf(stderr, "Warning: Failed to set PCM output format for %s, using silent audio\n", filename);
        return create_silent_wav(1.0f);
    }

    propertySize = sizeof(totalFrames);
    if ((ExtAudioFileGetProperty(file, kExtAudioFileProperty_FileLengthFrames, &propertySize, &totalFrames) != noErr) ||
        (totalFrames < 0)) {
        totalFrames = 0;
    }

    /* Preallocate from the frame count when it is known */
    if (totalFrames > 0) {
        size_t estimate = (size_t)totalFrames * pcmFormat.mBytesPerFrame;

        if (estimate >= 65536) {
            samples = (unsigned char*)malloc(estimate);
            if (samples) {
                capacity = estimate;
            }
        }
    }

    for (;;) {
        size_t needed = used + (size_t)framesPerRead * pcmFormat.mBytesPerFrame;
        AudioBufferList bufferList;
        UInt32 frames;

        if (needed > capacity) {
            size_t newCapacity = (needed < 65536) ? 65536 : needed + needed / 2;
            unsigned char* grown = (unsigned char*)realloc(samples, newCapacity);

            if (!grown) {
                free(samples);
                ExtAudioFileDispose(file);
                fprintf(stderr, "Warning: Memory allocation failed for %s, using silent audio\n", filename);
                return create_silent_wav(1.0f);
            }

            samples = grown;
            capacity = newCapacity;
        }

        bufferList.mNumberBuffers = 1;
        bufferList.mBuffers[0].mNumberChannels = pcmFormat.mChannelsPerFrame;
        bufferList.mBuffers[0].mDataByteSize = framesPerRead * pcmFormat.mBytesPerFrame;
        bufferList.mBuffers[0].mData = samples + used;

        frames = framesPerRead;
        if (ExtAudioFileRead(file, &frames, &bufferList) != noErr) {
            break;
        }

        if (frames == 0) {
            break;
        }

        used += (size_t)frames * pcmFormat.mBytesPerFrame;
    }

    ExtAudioFileDispose(file);

    if (used == 0) {
        free(samples);
        fprintf(stderr, "Warning: Failed to read audio data for %s, using silent audio\n", filename);
        return create_silent_wav(1.0f);
    }

    wav = (WavData*)malloc(sizeof(WavData));
    if (!wav) {
        free(samples);
        fprintf(stderr, "Warning: Memory allocation failed for %s, using silent audio\n", filename);
        return create_silent_wav(1.0f);
    }

    wav->data = samples;
    wav->size = (int)used;
    wav->sampleRate = (int)pcmFormat.mSampleRate;
    wav->channels = (int)pcmFormat.mChannelsPerFrame;
    wav->bitsPerSample = 16;

    printf("Successfully loaded: %s (%d Hz, %d channels, %d bits%s)\n",
           filename, wav->sampleRate, wav->channels, wav->bitsPerSample,
           (fileFormat.mFormatID != kAudioFormatLinearPCM) ? ", decoded from compressed source" : "");

    return wav;
}
#endif

static WavData* loadWav(const char* filename) {
    FILE* fp;
    char path[512];

    /* Try different paths */
    snprintf(path, sizeof(path), "%s", filename);
    fp = fopen(path, "rb");

    if (!fp) {
        snprintf(path, sizeof(path), "data/%s", filename);
        fp = fopen(path, "rb");
    }

    if (!fp) {
        snprintf(path, sizeof(path), "../data/%s", filename);
        fp = fopen(path, "rb");
    }

    if (!fp) {
        fprintf(stderr, "Warning: Failed to open WAV file (tried: %s, data/%s, ../data/%s)\n",
                filename, filename, filename);
        fprintf(stderr, "         Using silent audio\n");
        return create_silent_wav(1.0f);
    }

#ifdef __APPLE__
    /* Let CoreAudio decode the file. It understands PCM as well as
       compressed formats like Microsoft ADPCM (format 0x0002), which the
       hand-written RIFF parser below cannot read. */
    fclose(fp);
    return loadWavWithCoreAudio(path, filename);
#else
    {
    WavData* wav = NULL;

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

    /* The fmt chunk can be larger than the 16 bytes read above (cbSize or
       WAVE_FORMAT_EXTENSIBLE); skip the remainder before looking for data. */
    if (chunkSize > 16) {
        fseek(fp, (long)(chunkSize - 16), SEEK_CUR);
    }

    /* Skip to data chunk */
    if (fread(chunkID, 1, 4, fp) != 4 || fread(&chunkSize, 4, 1, fp) != 1) {
        fprintf(stderr, "Warning: Failed to find data chunk for %s, using silent audio\n", filename);
        free(wav);
        fclose(fp);
        return create_silent_wav(1.0f);
    }

    while (memcmp(chunkID, "data", 4) != 0 && !feof(fp)) {
        /* RIFF chunks are word aligned */
        fseek(fp, (long)chunkSize + (long)(chunkSize & 1), SEEK_CUR);
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
#endif
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
