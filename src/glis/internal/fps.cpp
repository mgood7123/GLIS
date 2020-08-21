#include <glis/internal/fps.hpp>

#include <sys/time.h> // struct timeval, gettimeofday

long GLIS_FPS::getCurrentTime(void) {
    struct timeval tv = {0};
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000. + tv.tv_usec / 1000.;
}

void GLIS_FPS::onFrameStart() {
    frameStartTime = getCurrentTime();
}

void GLIS_FPS::onFrameEndLapse() {
    frameEndTime = getCurrentTime();
    frameLength = frameEndTime - frameStartTime;
}

void GLIS_FPS::onFrameEnd() {
    frameEndTime = getCurrentTime();
    frameLength = frameEndTime - frameStartTime;

    frameLengths.push(frameLength);
    queueAggregate += frameLength;

    while (queueAggregate > trackedTime) {
        int oldFrame = frameLengths.front();
        frameLengths.pop();
        queueAggregate -= oldFrame;
    }

    auto s = frameLengths.size();

    averageFps = static_cast<float>(s) / (static_cast<float>(trackedTime) / 1000.0f);
}
