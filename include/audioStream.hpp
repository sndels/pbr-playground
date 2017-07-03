#ifndef AUDIOSTREAM_HPP
#define AUDIOSTREAM_HPP

#include <string>

class AudioStream
{
public:
    static AudioStream& getInstance()
    {
        // The only instance
        static AudioStream instance;
        return instance;
    }

    // Enforce singleton
    AudioStream(AudioStream const&) = delete;
    void operator=(AudioStream const&) = delete;

    // Static interface for rocket
    static void pauseStream(void* stream, int32_t flag);
    static void setStreamRow(void* stream, int32_t row);
    static int32_t isStreamPlaying(void* stream);

    void init(const std::string& filePath, double bpm, int32_t rpb);
    int32_t getStreamHandle();
    void play();
    bool isPlaying();
    void pause();
    void stop();
    double getRow() const;
    void setRow(int32_t row);

private:
    AudioStream();
    ~AudioStream();

    int32_t _streamHandle;
    bool    _shouldRestart;

};

#endif // AUDIOSTREAM_HPP
