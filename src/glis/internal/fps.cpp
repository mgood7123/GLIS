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

void GLIS_FPS::onFrameEnd() {
    int frameLength = getCurrentTime() - frameStartTime;

    frameLengths.push(frameLength);
    queueAggregate += frameLength;

    while (queueAggregate > trackedTime) {
        int oldFrame = frameLengths.front();
        frameLengths.pop();
        queueAggregate -= oldFrame;
    }

    averageFps = frameLengths.size() / (trackedTime / 1000);
}
