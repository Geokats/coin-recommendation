#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <random>
#include <chrono>
#include <time.h>
#include <unistd.h> //getopt

#include "util.hpp"
#include "point.hpp"
#include "recommender.hpp"
#include "searcher.hpp"

using namespace std;

string usageStr = "./cluster -i <input file> -c <clusters file> -o <output file>\n";

int main(int argc, char* const *argv) {
  //Command line arguments
  char *tweetsFileName = NULL;
  char *outputFileName = NULL;
  char *clustersFileName = NULL;
  bool lshMode = false;
  bool clusterMode = false;
  //Other arguments
  string lexiconFileName = "data/vader_lexicon.csv";
  string coinsFileName = "data/coins_queries.csv";


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
  if(lshMode){
    cout << "Calculating recommendations with LSH...\n";
    start = clock();
    unordered_map<int, point> lshPredictionsA = getLSHPredictions(16, 4, uNorm, uNorm, coins.size());
    unordered_map<int, point> lshPredictionsB = getLSHPredictions(16, 4, uNorm, c, coins.size());
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

  if(clusterMode){
    //Get clustering recommendations
    configuration conf;
    conf.setClusterCount(10);
    conf.setInitialise("random");
    conf.setAssign("lloyds");
    conf.setUpdate("kmeans");
    conf.setMetric("euclidean");

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

  outputFile.close();
  return 0;
}
