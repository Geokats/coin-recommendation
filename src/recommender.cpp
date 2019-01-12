#include "recommender.hpp"
#include "util.hpp"

#include <unordered_map>
#include <unordered_set>
#include <cmath>

using namespace std;

float getTweetScore(tweet t, unordered_map<string, float> lexicon){
  float totalScore = 0;
  float alpha = 15;

  for(auto word : t.words){
    auto val = lexicon.find(word);
    if(val != lexicon.end()){
      //If the current word of the tweet is in the lexicon add its value
      totalScore += val->second;
    }
  }

  float score = totalScore / sqrt(totalScore * totalScore + alpha);

  return score;
}

point getCoinScore(tweet t, unordered_map<string, float> lexicon, vector<string> coins, unordered_map<string, int> coinLexicon){
  //Initialise vector values to zeros
  vector<double> vals;
  for(int i = 0; i < coins.size(); i++){
    vals.push_back(0);
  }

  //Check which coins are mentioned in the tweet
  unordered_set<int> coinIndices;
  for(auto word : t.words){
    //Check if word is related to a coin
    auto coinIndex = coinLexicon.find(word);
    if(coinIndex != coinLexicon.end()){
      coinIndices.insert(coinIndex->second);
    }
  }

  //Add score to indices of mentioned coins
  float score = getTweetScore(t, lexicon);
  for(auto index : coinIndices){
    vals.at(index) = score;
  }

  //Add values to existing score vector
  point p("coin_scores", vals);
  return p;
}
