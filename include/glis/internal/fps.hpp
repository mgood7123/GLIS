#include <queue>

#pragma once

class GLIS_FPS {
    const int trackedTime = 1000;
    int frameStartTime;
    int queueAggregate = 0;
    std::queue<int> frameLengths;

    long getCurrentTime(void);

public:
    double averageFps;

    void onFrameStart();

    void onFrameEnd();
};