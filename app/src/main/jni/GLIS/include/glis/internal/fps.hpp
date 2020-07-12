#include <queue>

#pragma once

class GLIS_FPS {
    const int trackedTime = 1000;
    int frameStartTime;
    int queueAggregate = 0;
    std::queue<int> frameLengths;

    long getCurrentTime(void) {
        struct timeval tv = {0};
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000. + tv.tv_usec / 1000.;
    }

public:
    double averageFps;

    void onFrameStart() {
        frameStartTime = getCurrentTime();
    }

    void onFrameEnd() {
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
};