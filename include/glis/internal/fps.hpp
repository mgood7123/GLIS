#include <queue>

#pragma once

class GLIS_FPS {
    const int trackedTime = 1000;
    int queueAggregate = 0;
    std::queue<int> frameLengths;

    long getCurrentTime(void);

public:
    int frameStartTime;
    int frameEndTime;
    int frameLength;
    double averageFps;

    void onFrameStart();
    void onFrameEndLapse();
    void onFrameEnd();
};