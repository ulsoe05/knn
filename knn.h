#pragma once
#include <vector>


class KnnClassifier{
public:
    KnnClassifier(){};
    ~KnnClassifier(){};
    void train(const std::vector<std::vector<float>>& data, const std::vector<int>& labels); // Could be called "fit"
    int predict(const std::vector<float>& data);
};