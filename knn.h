#pragma once
#include <vector>
#include <span>
#include <map>
#include "data2d.h"

class KnnClassifier{
public:
    KnnClassifier(){};
    ~KnnClassifier(){};
    void train(const std::vector<std::vector<float>>& data, const std::vector<int>& labels); // Could be called "fit"
    int predict(const std::vector<float>& data);
};

template <typename T>
T euclidean_distance_squared(const T* a, const T* b, size_t n){
    T distance = 0;
    for (size_t i = 0; i < n; i++){
        distance += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return distance;
}

template <typename FEATURE_TYPE, typename LABEL_TYPE>
Data2d<LABEL_TYPE> knn_naive(
        const Data2d<FEATURE_TYPE>& train_data, 
        const Data2d<LABEL_TYPE>& train_labels, 
        const Data2d<FEATURE_TYPE>& test_data, 
        size_t k){

            
    const size_t n_tests = test_data.m_rows;
    const size_t n_features = test_data.m_columns;

    Data2d<LABEL_TYPE> predictions;
    predictions.m_data.resize(n_tests);
    predictions.m_columns = 1;
    predictions.m_rows = test_data.m_rows;
    
    using pair_t = std::pair<FEATURE_TYPE, LABEL_TYPE>;
    constexpr pair_t init_pair = pair_t((FEATURE_TYPE)999999, (LABEL_TYPE)999999);
    std::vector<pair_t> sq_distances_and_labels(k, init_pair);
    std::map<LABEL_TYPE, size_t> label_votes;
    
    // Insert all train labels into the map and set the votes to 0
    for (size_t idx_train = 0; idx_train < train_labels.m_rows; idx_train++){
        label_votes[train_labels.m_data[idx_train]] = 0;
    }

    for (size_t idx_test = 0; idx_test < n_tests; idx_test++){
        const FEATURE_TYPE* test_row = &(test_data.m_data[idx_test * n_features]);

        // Reset
        std::fill(sq_distances_and_labels.begin(), sq_distances_and_labels.end(), init_pair);
        for (auto& [label, votes] : label_votes){
            votes = 0;
        }

        for (size_t idx_train = 0; idx_train < train_data.m_rows; idx_train++){
            const FEATURE_TYPE * train_row = &(train_data.m_data[idx_train * n_features]);
            const FEATURE_TYPE distance = euclidean_distance_squared(test_row, train_row, n_features);
            if (distance < sq_distances_and_labels[k-1].first){
                LABEL_TYPE label = train_labels.m_data[idx_train];
                sq_distances_and_labels[k-1] = pair_t(distance, label);
                std::sort(sq_distances_and_labels.begin(), sq_distances_and_labels.end(), 
                    [](const pair_t& a, const pair_t& b){
                        return a.first < b.first;
                    });
            }
        }
        for (size_t idx = 0; idx < k; idx++){
            const LABEL_TYPE label = sq_distances_and_labels[idx].second;
            label_votes[label]++;
        }
        
        LABEL_TYPE most_common_label = 0;
        size_t most_common_label_votes = 0;
        for (auto& [label, votes] : label_votes){
            if (votes > most_common_label_votes){
                most_common_label = label;
                most_common_label_votes = votes;
            }
        }
        predictions.m_data[idx_test] = most_common_label;
        
    }
    return predictions;
}