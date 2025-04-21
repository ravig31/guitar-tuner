#pragma once

#include <vector>

class Spectrogram {
public:
    void display(const std::vector<double>& spectrum, int startIndex, int spectroSize, int displaySize = 100) const;
};
