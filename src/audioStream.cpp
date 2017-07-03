#include "audioStream.hpp"

#include <bass.h>
#include <iostream>

using std::cout;
using std::endl;

namespace {
    static int32_t ROW_RATE = 0;
}

void AudioStream::pauseStream(void* stream, int32_t flag)
{
    int32_t streamHandle = *(int32_t*)stream;
    if (flag) BASS_ChannelPause(streamHandle);
    else BASS_ChannelPlay(streamHandle, false);
}

void AudioStream::setStreamRow(void* stream, int32_t row)
{
    int32_t streamHandle = *(int32_t*)stream;
    uint64_t pos = BASS_ChannelSeconds2Bytes(streamHandle, row / ROW_RATE);
    BASS_ChannelSetPosition(streamHandle, pos, BASS_POS_BYTE);
}

int32_t AudioStream::isStreamPlaying(void* stream)
{
    int32_t streamHandle = *(int32_t*)stream;
    return BASS_ChannelIsActive(streamHandle) == BASS_ACTIVE_PLAYING;
}

int32_t AudioStream::getStreamHandle()
{
    return _streamHandle;
}

void AudioStream::init(const std::string& filePath, double bpm, int32_t rpb)
{
    if (!BASS_StreamFree(_streamHandle)) {
        _shouldRestart = false;
        ROW_RATE = bpm / 60 * rpb;
        if (BASS_Init(-1, 44100, 0, 0, 0)) {
            _streamHandle = BASS_StreamCreateFile(false, filePath.c_str(), 0, 0,
                                                BASS_STREAM_PRESCAN);
            if (!_streamHandle)
                cout << "[audio] error opening stream from " << filePath << endl;
        } else
            cout << "[audio] failed to init BASS" << endl;
    } else
        cout << "[audio] failed to free stream" << endl;
}

void AudioStream::play()
{
    BASS_Start();
    BASS_ChannelPlay(_streamHandle, _shouldRestart);
    _shouldRestart = false;
}

bool AudioStream::isPlaying() {
    return BASS_ChannelIsActive(_streamHandle) == BASS_ACTIVE_PLAYING;
}

void AudioStream::pause()
{
    BASS_ChannelPause(_streamHandle);
}

void AudioStream::stop()
{
    BASS_ChannelPause(_streamHandle);
    _shouldRestart = true;
}

double AudioStream::getRow() const
{
    int64_t pos = BASS_ChannelGetPosition(_streamHandle, BASS_POS_BYTE);
    double time = BASS_ChannelBytes2Seconds(_streamHandle, pos);
    return time * ROW_RATE;
}

void AudioStream::setRow(int32_t row)
{
    int64_t pos = BASS_ChannelSeconds2Bytes(_streamHandle, row / ROW_RATE);
    BASS_ChannelSetPosition(_streamHandle, pos, BASS_POS_BYTE);
}

AudioStream::AudioStream() :
    _streamHandle(0),
    _shouldRestart(false)
{ }

AudioStream::~AudioStream()
{
    BASS_StreamFree(_streamHandle);
    BASS_Free();
}
