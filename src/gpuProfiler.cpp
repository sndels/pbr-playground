#include "gpuProfiler.hpp"

GpuProfiler::GpuProfiler(uint32_t window) :
    _backActive(false),
    _times(window, 0.f)
{
    glGenQueries(2, _queryIDs);
    // Set the "back" counter
    GLint done = 0;
    glBeginQuery(GL_TIME_ELAPSED, _queryIDs[1]);
    glEndQuery(GL_TIME_ELAPSED);
    while (!done) glGetQueryObjectiv(_queryIDs[1], GL_QUERY_RESULT_AVAILABLE, &done);
}

void GpuProfiler::startSample()
{
    glBeginQuery(GL_TIME_ELAPSED, _queryIDs[_backActive ? 1 : 0]);
}

void GpuProfiler::endSample()
{
    glEndQuery(GL_TIME_ELAPSED);
    _times.erase(_times.begin());
    GLint64 elapsed;
    glGetQueryObjecti64v(_queryIDs[_backActive ? 0 : 1], GL_QUERY_RESULT, &elapsed);
    _times.emplace_back(elapsed * 0.000001f);
    _backActive = !_backActive;
}

float GpuProfiler::getAvg()
{
    float sum = 0.f;
    for (auto i : _times) sum += i;
    return sum / _times.size();
}
