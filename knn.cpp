#include "knn.h"
#include "rng.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <numeric>
#include "data2d.h"


std::pair<std::vector<size_t>, std::vector<size_t>> train_and_test_indices(uint32_t num_samples, float train_ratio, uint32_t seed){
    size_t num_train = (size_t)(num_samples * train_ratio);
    std::vector<size_t> indices(num_samples);
    std::iota(indices.begin(), indices.end(), 0);
    shuffle_durstenfeld(indices, seed);
    std::vector<size_t> test_indices(indices.begin() + num_train, indices.end());
    indices.resize(num_train);
    return std::make_pair(indices, test_indices);
}

/**
 * @brief Reads a csv file and returns a vector of vectors of strings
 *   Note that the outer vectors correspond to columns in the file
 * 
 * @param filepath 
 * @param delimiter 
 * @return Data2d<std::string>
 */
Data2d<std::string> read_csv(std::filesystem::path filepath, char delimiter = ','){
    Data2d<std::string> data;
    size_t num_features = 0;

    // Opening file
    std::ifstream file(filepath);
    if (!file.is_open())  
        return data;

    // Reading line by line
    std::string line;
    while (std::getline(file, line)){
        size_t features_in_this_line = std::count(line.begin(), line.end(), delimiter) + 1;
        if (num_features == 0){
            num_features = features_in_this_line;
            data.m_columns = num_features;
        } else if (num_features != features_in_this_line){
            return data;
        }

        std::string cell;
        std::stringstream line_stream(line);
        size_t current_feature = 0;
        while (std::getline(line_stream, cell, delimiter)){
            data.m_data.push_back(cell);
            current_feature++;
        }
        data.m_rows++;
    }
    return data;

}


/**
 * @brief Converts a vector of vectors of strings to a vector of vectors of floats
 * 
 * @param data 
 * @param invalid_entry_value 
 * @return std::vector<std::vector<float>> 
 */
Data2d<float> convert_to_float(const Data2d<std::string>& data, float invalid_entry_value = 0.0f){
    Data2d<float> float_data;
    float_data.m_columns = data.m_columns;
    float_data.m_rows = data.m_rows;
    float_data.m_data.resize(data.m_data.size());
    for (size_t i = 0; i < data.m_data.size(); i++){
        try {
            float_data.m_data[i] = std::stof(data.m_data[i]);
        } catch (const std::invalid_argument& ia){
            float_data.m_data[i] = invalid_entry_value;
        }
    }
    return float_data;
}

/**
 * @brief Slices a 2d data structure
 * 
 * @tparam T 
 * @param data original data
 * @param cols 
 * @param rows 
 * @return Data2d<T> sliced data 
 */
template <typename T>
Data2d<T> slice(const Data2d<T>& data, const std::vector<size_t> cols, const std::vector<size_t> rows){
    Data2d<T> sliced_data;
    sliced_data.m_columns = cols.size();
    sliced_data.m_rows = rows.size();
    sliced_data.m_data.resize(sliced_data.m_columns * sliced_data.m_rows);
    for (size_t i = 0; i < sliced_data.m_rows; i++){
        for (size_t j = 0; j < sliced_data.m_columns; j++){
            sliced_data.m_data[i * sliced_data.m_columns + j] = data.m_data[rows[i] * data.m_columns + cols[j]];
        }
    }
    return sliced_data;
}

int main(int argc, char** argv){
    using std::cout;
    using std::endl;
    using std::string;

    const std::vector<string> dataset_features = {"id", 
        "Clump Thickness", 
        "Uniformity of Cell Size", 
        "Uniformity of Cell Shape", 
        "Marginal Adhesion", 
        "Single Epithelial Cell Size", 
        "Bare Nuclei", 
        "Bland Chromatin", 
        "Normal Nucleoli", 
        "Mitoses", 
        "Class" };

    std::filesystem::path dataset_path(__FILE__);
    dataset_path = dataset_path.parent_path();
    dataset_path /= "breastcancer.data";
    cout << "Looking for dataset at: " << dataset_path << endl;
    Data2d<string> str_data = read_csv(dataset_path, ',');
    Data2d<float> flt_data = convert_to_float(str_data, -9999.0f);
    
    cout << "Number of features: " << str_data.m_columns << endl;

    auto[train_indices, test_indices] = train_and_test_indices(flt_data.m_rows, 0.7f, 6667u);
    cout << "num_train: " << train_indices.size() << endl;
    cout << "num_test: " << test_indices.size() << endl;

    auto train_data = slice(flt_data, {1, 2, 3, 4, 5, 6, 7, 8, 9}, train_indices);
    auto train_labels = slice(flt_data, {10}, train_indices);
    auto test_data = slice(flt_data, {1, 2, 3, 4, 5, 6, 7, 8, 9}, test_indices);
    auto test_labels = slice(flt_data, {10}, test_indices);

    // Printing shapes
    cout << "train_data: " << train_data.m_rows << " x " << train_data.m_columns << endl;
    cout << "test_data: " << test_data.m_rows << " x " << test_data.m_columns << endl;
    cout << "train_labels: " << train_labels.m_rows << " x " << train_labels.m_columns << endl;
    Data2d<float> predictions = knn_naive(train_data, train_labels, test_data, 11);
    
    // Print predictions and labels
    size_t correct_predictions = 0;
    
    for (size_t i = 0; i < predictions.m_rows; i++){
        cout << "Prediction: " << predictions.m_data[i] << " | Label: " << test_labels.m_data[i] << endl;
        if (predictions.m_data[i] == test_labels.m_data[i]){
            correct_predictions++;
        }
    }
    cout << "Accuracy: " << (float)correct_predictions / (float)predictions.m_rows << endl;


    return 0;
}