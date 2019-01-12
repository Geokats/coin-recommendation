#ifndef RECOMMENDER
#define RECOMMENDER

#include "util.hpp"

#include <unordered_map>

float getTweetScore(tweet t, std::unordered_map<std::string, float> lexicon);

point getCoinScore(tweet t, std::unordered_map<std::string, float> lexicon, std::vector<std::string> coins, std::unordered_map<std::string, int> coinLexicon);

#endif
