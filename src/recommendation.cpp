#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <chrono>
#include <time.h>
#include <unistd.h> //getopt

#include "util.hpp"
#include "point.hpp"
#include "recommender.hpp"
#include "searcher.hpp"

using namespace std;

string usageStr = "./recommendation -i <input file> -c <clusters file> -o <output file> -m <mode>\n";

int main(int argc, char* const *argv) {
  //Command line arguments
  char *tweetsFileName = NULL;
  char *outputFileName = NULL;
  char *clustersFileName = NULL;
  bool lshMode = false;
  bool clusterMode = false;
  bool validationMode = false;
  //Other arguments
  string lexiconFileName = "data/vader_lexicon.csv";
  string coinsFileName = "data/coins_queries.csv";
  //LSH parameters
  int lshK = 16;
  int lshL = 4;
  //Clustering configuration
  configuration conf;
  conf.setClusterCount(10);
  conf.setInitialise("random");
  conf.setAssign("lloyds");
  conf.setUpdate("kmeans");
  conf.setMetric("euclidean");


  char ch;
  //Read command line arguments
  while((ch = getopt(argc, argv, "i:o:c:m:")) != -1){
    switch(ch){
      case 'i':
        tweetsFileName = optarg;
        break;
      case 'o':
        outputFileName = optarg;
        break;
      case 'c':
        clustersFileName = optarg;
        break;
      case 'm':
        if(strcmp(optarg, "lsh") == 0){
          lshMode = true;
        }
        else if(strcmp(optarg, "cluster") == 0){
          clusterMode = true;
        }
        else if(strcmp(optarg, "validation") == 0){
          validationMode = true;
        }
        break;
      default:
        cerr << "Usage: " << usageStr;
        return 1;
    }
  }

  //Check command line arguments
  if(tweetsFileName == NULL || clustersFileName == NULL || outputFileName == NULL){
    cerr << "Usage: " << usageStr;
    return 1;
  }
  if(!lshMode && !clusterMode){
    cerr << "Error: No mode was selected\n";
    cerr << "Usage: " << usageStr;
    return 1;
  }

  //Load lexicon
  unordered_map<string, float> lexicon;
  cout << "Loading lexicon from " << lexiconFileName << "...\n";
  readLexiconFile(lexiconFileName , lexicon);

  //Load coin lexicon
  vector<string> coins;
  unordered_map<string, int> coinLexicon;
  cout << "Loading coin lexicon from " << coinsFileName << "...\n";
  readCoinsFile(coinsFileName , coins, coinLexicon);

  //Load tweets
  unordered_map<int, tweet> tweets;
  cout << "Loading tweets from " << tweetsFileName << "...\n";
  readTweetsFile(tweetsFileName, tweets);

  //Load clusters
  cout << "Loading clusters from " << clustersFileName << "...\n";
  vector<vector<int>> clusters;
  readClustersFile(clustersFileName, clusters);

  //Open output file
  fstream outputFile(outputFileName, ios_base::out);

  //Timer variables
  clock_t start, end;

  //Calculate score vectors u for each user
  cout << "Calculating user score vectors...\n";
  unordered_map<int, point> u = getUsersScore(tweets, lexicon, coins, coinLexicon);
  cout << "Created score vectors for " << u.size() << " users\n";

  //Normalise user score vectors
  cout << "Normalising user score vectors...\n";
  unordered_map<int, point> uNorm = normaliseScores(u);

  //Calculate score vectors c for each cluster
  cout << "Calculating cluster score vectors...\n";
  vector<point> c = getClustersScore(clusters, tweets, lexicon, coins, coinLexicon);

  //Get LSH recommendations
  if(lshMode && !validationMode){
    cout << "Calculating recommendations with LSH...\n";
    start = clock();
    unordered_map<int, point> lshPredictionsA = getLSHPredictions(lshK, lshL, uNorm, uNorm, coins.size());
    unordered_map<int, point> lshPredictionsB = getLSHPredictions(lshK, lshL, uNorm, c, coins.size());
    end = clock();
    double lshTime = (double) (end - start)/CLOCKS_PER_SEC;
    outputFile << "Cosine LSH\n";
    for(auto entry : u){
      //Get recommended coins
      vector<string> recommendationsA = getCoinRecommendations(entry.second, lshPredictionsA.at(entry.first), coins, 5);
      vector<string> recommendationsB = getCoinRecommendations(entry.second, lshPredictionsB.at(entry.first), coins, 2);
      //Print output
      outputFile << entry.first << ": ";
      for(auto coin : recommendationsA){
        outputFile << coin + " ";
      }
      outputFile << "|| ";
      for(auto coin : recommendationsB){
        outputFile << coin + " ";
      }
      outputFile << "\n";
    }
    outputFile << "Execution time: " << lshTime << "\n\n";
  }

  //Get clustering recommendations
  if(clusterMode && !validationMode){
    cout << "Calculating recommendations with clustering...\n";
    start = clock();
    unordered_map<int, point> clusterPredictionsA = getClusteringPredictions(conf, uNorm, uNorm, coins.size());
    unordered_map<int, point> clusterPredictionsB = getClusteringPredictions(conf, uNorm, c, coins.size());
    end = clock();
    double clusteringTime = (double) (end - start)/CLOCKS_PER_SEC;
    outputFile << "Clustering\n";
    for(auto entry : u){
      //Get recommended coins
      vector<string> recommendationsA = getCoinRecommendations(entry.second, clusterPredictionsA.at(entry.first), coins, 5);
      vector<string> recommendationsB = getCoinRecommendations(entry.second, clusterPredictionsB.at(entry.first), coins, 2);
      //Print output
      outputFile << entry.first << ": ";
      for(auto coin : recommendationsA){
        outputFile << coin + " ";
      }
      outputFile << "|| ";
      for(auto coin : recommendationsB){
        outputFile << coin + " ";
      }
      outputFile << "\n";
    }
    outputFile << "Execution time: " << clusteringTime << "\n";
  }

  if(validationMode){
    cout << "Calculatine Mean Absolute Error...\n";

    int foldCount = 10;
    int foldSize = u.size() / foldCount;
    vector<int> userIds;
    for(auto entry : u){
      userIds.push_back(entry.first);
    }
    random_shuffle(userIds.begin(), userIds.end());

    double lshMAE = 0;
    int lshJ = 0;
    double clusterMAE = 0;
    int clusterJ = 0;

    for(int i = 0; i < foldCount; i++){
      unordered_map<int, point> testSet;
      unordered_map<int, point> trainSet;
      //Seperate fold in train and test sets
      for(int j = 0; j < foldCount; j++){
        vector<int>::iterator usersStart = userIds.begin() + j*foldSize;
        vector<int>::iterator usersEnd = j+1 == foldCount ? userIds.end() : usersStart + foldSize;
        if(i == j){
          //If the current fold is the test set
          for(auto it = usersStart; it != usersEnd; it++){
            testSet.emplace(*it, u.at(*it));
          }
        }
        else{
          for(auto it = usersStart; it != usersEnd; it++){
            trainSet.emplace(*it, u.at(*it));
          }
        }
      }
      //Delete some known values from test set vectors
      unordered_map<int, point> testSetErased;
      for(auto entry : testSet){
        int ratedItems = entry.second.nonZeroVals();
        if(ratedItems < 2){
          continue;
        }

        point ratings = entry.second;
        int erasedValues = 0;
        for(int j = 0; j < ratings.dim() && erasedValues; j++){
          if(ratings.get(j) != 0){
            if(erasedValues < ratedItems/2){
              ratings.set(j, 0);
            }
            else{
              entry.second.set(j, 0);
            }
            erasedValues++;
          }
        }
        testSetErased.emplace(entry.first, ratings);
      }

      //Get predictions and calculate errors
      if(lshMode){
        unordered_map<int, point> lshPredictions = getLSHPredictions(lshK, lshL, testSetErased, trainSet, coins.size());
        for(auto entry : lshPredictions){
          point expected = testSet.at(entry.first);
          for(int j = 0; j < expected.dim(); j++){
            if(expected.get(j) != 0){
              lshMAE += abs(expected.get(j) - entry.second.get(j));
              lshJ++;
            }
          }
        }
      }

      if(clusterMode){
        unordered_map<int, point> clusterPredictions = getClusteringPredictions(conf, testSetErased, trainSet, coins.size());
        for(auto entry : clusterPredictions){
          point expected = testSet.at(entry.first);
          for(int j = 0; j < expected.dim(); j++){
            if(expected.get(j) != 0){
              clusterMAE += abs(expected.get(j) - entry.second.get(j));
              clusterJ++;
            }
          }
        }
      }
    }

    //Calculate and print results
    if(lshMode){
      lshMAE = lshMAE / lshJ;
      outputFile << "LSH Recommendation MAE: " << lshMAE << "\n";
    }
    if(clusterMode){
      clusterMAE = clusterMAE / clusterJ;
      outputFile << "Clustering Recommendation MAE: " << clusterMAE << "\n";
    }
  }

  outputFile.close();
  return 0;
}
