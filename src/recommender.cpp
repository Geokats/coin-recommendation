#include "recommender.hpp"
#include "util.hpp"
#include "searcher.hpp"
#include "point.hpp"
#include "clusterCreator.hpp"

#include <map>
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
  string name = to_string(t.userId) + "_coin_scores";
  point p(name, vals);
  return p;
}

unordered_map<int, point> getUsersScore(unordered_map<int, tweet> tweets, unordered_map<string, float> lexicon, vector<string> coins, unordered_map<string, int> coinLexicon){
  point zeros("zeros", coins.size());

  unordered_map<int, point> u;
  for(auto t : tweets){
    point score = getCoinScore(t.second, lexicon, coins, coinLexicon);
    if(score.equal(zeros)){
      //If tweet doesn't give any information, don't add it
      continue;
    }

    auto entry = u.find(t.second.userId);
    if(entry == u.end()){
      //If user vector hasn't been created yet, add a new entry
      u.emplace(t.second.userId, score);
    }
    else{
      //If user already has a vector, add score to the vector
      entry->second.add(score);
    }
  }
  return u;
}

vector<point> getClustersScore(vector<vector<int>> clusters, unordered_map<int, tweet> tweets, unordered_map<string, float> lexicon, vector<string> coins, unordered_map<string, int> coinLexicon){
  point zeros("zeros", coins.size());

  vector<point> c;
  for(auto cluster : clusters){
    //Create vector for cluster
    point score("coin_scores", coins.size());

    for(auto tweetId : cluster){
      //For each tweet in cluster add score vector to cluster vector
      point tweetScore = getCoinScore(tweets[tweetId], lexicon, coins, coinLexicon);
      score.add(tweetScore);
    }

    if(score.equal(zeros)){
      //If tweet doesn't give any information, don't add it
      continue;
    }
    c.emplace_back(score);
  }
  return c;
}

point getAverageScore(unordered_map<int, point> u, int coinCount){
  point average("average_score", coinCount);
  for(auto entry : u){
    average.add(entry.second);
  }
  average.div((float) coinCount);
  return average;
}

unordered_map<int,point> normaliseScores(unordered_map<int,point> scores){
  unordered_map<int,point> newScores;
  for(auto entry : scores){
    double mean = entry.second.sumVals() / (double) entry.second.nonZeroVals();
    entry.second.setZeroVals(mean);

    newScores.emplace(entry.first, entry.second);
  }

  return newScores;
}

vector<point> normaliseScores(vector<point> scores){
  vector<point> newScores;
  for(auto entry : scores){
    double mean = entry.sumVals() / (double) entry.nonZeroVals();
    entry.setZeroVals(mean);

    newScores.emplace_back(entry);
  }

  return newScores;
}

point getScorePrediction(point user, vector<point*> neighbors){
  point sum("sum", user.dim());
  float simSum = 0;

  for(auto n : neighbors){
    if(n->equal(user)){
      //If the user is in the neighbors, then skip him
      continue;
    }

    float sim = user.cosDist(*n);
    simSum += abs(sim);

    point temp(user);
    temp.sub(*n);
    temp.mult(sim);
    sum.add(temp);
  }
  if(simSum != 0){
    sum.div(simSum);
  }
  point prediction = user.sum(sum);

  return prediction;
}

point getScorePrediction(point user, unordered_set<point*> neighbors){
  vector<point*> newNeighbors;

  for(auto n : neighbors){
    newNeighbors.emplace_back(n);
  }

  return getScorePrediction(user, newNeighbors);
}

vector<string> getCoinRecommendations(point userNotNormalized, point prediction, vector<string> coins, int k){
  multimap<double, string> coinScores;

  for(int i = 0; i < prediction.dim(); i++){
    if(userNotNormalized.get(i) == 0.0){
      //If user hadn't talked about the coin
      coinScores.emplace(prediction.get(i), coins[i]);
    }
  }

  int recommendationCount = k > coinScores.size() ? coinScores.size() : k;
  vector<string> recommendations;

  auto entry = coinScores.end();
  entry--;
  for(int i = 0; i < recommendationCount; i++){
    recommendations.emplace_back(entry->second);
    entry--;
  }

  return recommendations;
}

unordered_map<int, point> getLSHPredictions(int k, int L, unordered_map<int, point> users, vector<point> data, int coinsCount){
  unordered_map<int, point> predictions;
  //Initialise LSH
  lsh srch = lsh(k, L, coinsCount, "cosine", &data);

  for(auto entry : users){
    unordered_set<point*> neighbors;
    int tries = 0;
    float radius = 0.01;
    do{
      neighbors.clear();
      tries++;
      neighbors = srch.rnn(entry.second, radius);
      radius *= 2;
    }while(neighbors.size() < 20 && tries < 50);

    //Make prediction based on neighbors
    point prediction = getScorePrediction(entry.second, neighbors);
    predictions.emplace(entry.first, prediction);
  }

  return predictions;
}

unordered_map<int, point> getLSHPredictions(int k, int L, unordered_map<int, point> users, unordered_map<int, point> data, int coinsCount){
  //Get scores to be used as points for LSH
  vector<point> scores;
  for(auto entry : data){
    scores.emplace_back(entry.second);
  }
  return getLSHPredictions(k, L, users, scores, coinsCount);
}

unordered_map<int, point> getClusteringPredictions(configuration conf, unordered_map<int, point> users, vector<point> data, int coinsCount){
  unordered_map<int, point> predictions;

  //Initialise clustering
  clusterCreator cl = clusterCreator(&data, conf);
  cl.makeClusters();

  vector<point*>* clusters =  cl.getClusters();
  vector<point>* centroids =  cl.getCentroids();

  for(auto entry : users){
    //Find in which cluster it belongs
    int clusterIndex = 0;
    double minDist = (*centroids).at(0).euclDist(entry.second);

    for(int i = 1; i < conf.getClusterCount(); i++){
      if(clusters[i].size() == 0){
        //If cluster is empty, then skip it
        continue;
      }
      float dist = (*centroids).at(i).euclDist(entry.second);
      if(dist < minDist){
        minDist = dist;
        clusterIndex = i;
      }
    }

    //Make prediction based on cluster
    point prediction = getScorePrediction(entry.second, clusters[clusterIndex]);
    predictions.emplace(entry.first, prediction);
  }

  return predictions;
}

unordered_map<int, point> getClusteringPredictions(configuration conf, unordered_map<int, point> users, unordered_map<int, point> data, int coinsCount){
  //Get scores to be used as points for clustering
  vector<point> scores;
  for(auto entry : data){
    scores.emplace_back(entry.second);
  }
  return getClusteringPredictions(conf, users, scores, coinsCount);
}

void calculateCumulativeMAE(unordered_map<int, point> predictions, unordered_map<int, point> expectations, double &MAE, int &J){
  for(auto entry : predictions){
    point expected = expectations.at(entry.first);
    for(int j = 0; j < expected.dim(); j++){
      if(expected.get(j) != 0){
        MAE += abs(expected.get(j) - entry.second.get(j));
        J++;
      }
    }
  }
}
