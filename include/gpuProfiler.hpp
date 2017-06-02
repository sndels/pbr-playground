#ifndef GPUPROFILER_HPP
#define GPUPROFILER_HPP

#include <GL/gl3w.h>
#include <vector>

class GpuProfiler
{
public:
    GpuProfiler(uint32_t window);
    ~GpuProfiler() {}

    void startSample();
    void endSample();
    float getAvg();

private:
    GLuint             _queryIDs[2];
    bool               _backActive;
    std::vector<float> _times;

};

#endif // GPUPROFILER_HPP
