#ifndef RECOMMENDER
#define RECOMMENDER

#include "util.hpp"
#include "point.hpp"

#include <unordered_map>

float getTweetScore(tweet t, std::unordered_map<std::string, float> lexicon);

point getCoinScore(tweet t, std::unordered_map<std::string, float> lexicon, std::vector<std::string> coins, std::unordered_map<std::string, int> coinLexicon);

point getAverageScore(std::unordered_map<int, point> u, int coinCount);

std::unordered_map<int, point> getUsersScore(std::unordered_map<int, tweet> tweets, std::unordered_map<std::string, float> lexicon, std::vector<std::string> coins, std::unordered_map<std::string, int> coinLexicon);

std::vector<point> getClustersScore(std::vector<std::vector<int>> clusters, std::unordered_map<int, tweet> tweets, std::unordered_map<std::string, float> lexicon, std::vector<std::string> coins, std::unordered_map<std::string, int> coinLexicon);

std::unordered_map<int,point> normaliseScores(std::unordered_map<int,point> &scores);

std::unordered_map<int, point> getLSHPredictions(int k, int L, std::unordered_map<int, point> users, std::vector<point> data, int coinsCount);
std::unordered_map<int, point> getLSHPredictions(int k, int L, std::unordered_map<int, point> users, std::unordered_map<int, point> data, int coinsCount);

std::unordered_map<int, point> getClusteringPredictions(configuration conf, std::unordered_map<int, point> users, std::vector<point> data, int coinsCount);
std::unordered_map<int, point> getClusteringPredictions(configuration conf, std::unordered_map<int, point> users, std::unordered_map<int, point> data, int coinsCount);

std::vector<std::string> getCoinRecommendations(point userNotNormalized, point prediction, std::vector<std::string> coins, int k);

#endif
