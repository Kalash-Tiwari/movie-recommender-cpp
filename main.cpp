#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <string>

using namespace std;

// Function to load the ratings matrix from a CSV file
vector<vector<int>> loadRatings(const string& filename) {
    vector<vector<int>> ratings;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        vector<int> row;
        while (getline(ss, value, ',')) {
            row.push_back(stoi(value));
        }
        ratings.push_back(row);
    }
    return ratings;
}

// Function to calculate cosine similarity between two users
double calculateSimilarity(const vector<int>& user1, const vector<int>& user2) {
    double dotProduct = 0.0, norm1 = 0.0, norm2 = 0.0;

    for (size_t i = 0; i < user1.size(); i++) {
        dotProduct += user1[i] * user2[i];
        norm1 += user1[i] * user1[i];
        norm2 += user2[i] * user2[i];
    }

    return (norm1 > 0 && norm2 > 0) ? dotProduct / (sqrt(norm1) * sqrt(norm2)) : 0.0;
}

// Function to predict a rating for a specific movie for a target user
double predictRating(const vector<vector<int>>& ratings, int targetUser, int movie, const vector<double>& similarities) {
    double weightedSum = 0.0, similaritySum = 0.0;

    for (size_t i = 0; i < ratings.size(); i++) {
        if (i != targetUser && ratings[i][movie] > 0) { // Ignore the target user and unrated movies
            weightedSum += similarities[i] * ratings[i][movie];
            similaritySum += fabs(similarities[i]);
        }
    }

    return (similaritySum > 0) ? weightedSum / similaritySum : 0.0;
}

// Function to recommend the top N movies for a target user
vector<pair<int, double>> recommendMovies(const vector<vector<int>>& ratings, int targetUser, int N) {
    vector<double> similarities(ratings.size());
    for (size_t i = 0; i < ratings.size(); i++) {
        if (i != targetUser) {
            similarities[i] = calculateSimilarity(ratings[targetUser], ratings[i]);
        }
    }

    vector<pair<int, double>> predictions;
    for (size_t movie = 0; movie < ratings[targetUser].size(); movie++) {
        if (ratings[targetUser][movie] == 0) { // Only predict for unrated movies
            double predictedRating = predictRating(ratings, targetUser, movie, similarities);
            predictions.push_back({movie, predictedRating});
        }
    }

    // Sort by predicted rating in descending order
    sort(predictions.begin(), predictions.end(), [](const pair<int, double>& a, const pair<int, double>& b) {
        return a.second > b.second;
    });

    if (N > predictions.size()) N = predictions.size();
    return vector<pair<int, double>>(predictions.begin(), predictions.begin() + N);
}

// Main function
int main() {
    string filename = "ratings.csv"; // CSV file containing ratings
    vector<vector<int>> ratings = loadRatings(filename);

    int targetUser = 0; // Index of the user for whom recommendations are made
    int topN = 3; // Number of recommendations

    vector<pair<int, double>> recommendations = recommendMovies(ratings, targetUser, topN);

    cout << "Top " << topN << " recommended movies for User " << targetUser + 1 << ":\n";
    for (const auto& rec : recommendations) {
        cout << "Movie " << rec.first + 1 << " with predicted rating " << rec.second << "\n";
    }

    return 0;
}
