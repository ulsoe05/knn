#include "knn.h"
#include "rng.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <numeric>

template <typename T>
struct Buffer2D{
    std::vector<T> m_data;
    size_t m_columns;
    size_t m_rows;
};



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
 * @return std::vector<std::vector<std::string>> columns
 */
std::vector<std::vector<std::string>> read_csv(std::filesystem::path filepath, char delimiter = ','){
    std::vector<std::vector<std::string>> columns;
    size_t num_features = 0;
    
    // Opening file
    std::ifstream file(filepath);
    if (!file.is_open()){  
        std::cout << "Error opening file" << std::endl;
        return columns;
    }

    // Reading line by line
    std::string line;
    while (std::getline(file, line)){

        size_t features_in_this_line = std::count(line.begin(), line.end(), delimiter) + 1;
        if (num_features == 0){
            num_features = features_in_this_line;
            columns.resize(num_features);
            std::cout << "Number of features: " << num_features << std::endl;
        } else if (num_features != features_in_this_line){
            std::cout << "Error: number of features in line " << columns.size() << " is different from the first line" << std::endl;
            return columns;
        }

        std::string cell;
        std::stringstream line_stream(line);
        size_t current_feature = 0;
        while (std::getline(line_stream, cell, delimiter)){
            columns[current_feature].push_back(cell);
        }
    }

    return columns;
}

/**
 * @brief Converts a vector of vectors of strings to a vector of vectors of floats
 * 
 * @param data 
 * @param invalid_entry_value 
 * @return std::vector<std::vector<float>> 
 */
std::vector<std::vector<float>> convert_to_float(const std::vector<std::vector<std::string>>& data, float invalid_entry_value = 0.0f){
    std::vector<std::vector<float>> float_data(data.size());
    for (size_t i = 0; i < data.size(); i++){
        float_data[i].resize(data[i].size());
        for (size_t j = 0; j < data[i].size(); j++){
            try {
                float_data[i][j] = std::stof(data[i][j]);
            } catch (const std::invalid_argument& ia){
                float_data[i][j] = invalid_entry_value;
            }
        }
    }
    return float_data;
}




int main(int argc, char** argv){
    using std::cout;
    using std::endl;
    using std::string;

    cout << "Hello, world!" << endl;


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

    //std::filesystem::path dataset_path = "breast_cancer_wisconsin_original";
    //dataset_path /= "breast-cancer-wisconsin.data";
    std::filesystem::path dataset_path(__FILE__);
    dataset_path = dataset_path.parent_path();
    dataset_path /= "breastcancer.data";
    cout << "Looking for dataset at: " << dataset_path << endl;
    std::vector<std::vector<string>> csv_columns = read_csv(dataset_path, ',');
    std::vector<std::vector<float>> columns = convert_to_float(csv_columns, -9999.0f);


    cout << "Number of features: " << columns.size() << endl;

    auto[train_indices, test_indices] = train_and_test_indices(100u, 0.7f, 6667u);
    cout << "num_train: " << train_indices.size() << endl;
    cout << "num_test: " << test_indices.size() << endl;

    for (size_t i = 0; i < train_indices.size(); i++){
        cout << train_indices[i] << " ";
    }




    return 0;
}