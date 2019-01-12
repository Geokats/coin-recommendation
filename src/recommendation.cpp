#include <string>
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

using namespace std;

string usageStr = "./cluster -i <input file> -c <clusters file> -o <output file>\n";

int main(int argc, char* const *argv) {
  //Command line arguments
  char *tweetsFileName = NULL;
  char *outputFileName = NULL;
  char *clustersFileName = NULL;
  //Other arguments
  string lexiconFileName = "data/vader_lexicon.csv";
  string coinsFileName = "data/coins_queries.csv";


  char ch;

  //Read command line arguments
  while((ch = getopt(argc, argv, "i:o:c:")) != -1){
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
  vector<vector<int>> clusters;
  readClustersFile(clustersFileName, clusters);

  //Open output file
  fstream outputFile(outputFileName, ios_base::out);

  //Calculate u score vectors for each user
  cout << "Calculating user score vectors...\n";
  unordered_map<int, point> u;
  for(auto t : tweets){
    point score = getCoinScore(t.second, lexicon, coins, coinLexicon);

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

  // for(auto entry : u){
  //   cout << "User id = " << entry.first << "\n";
  //   entry.second.print();
  // }

  //Calculate c score vectors for each cluster
  cout << "Calculating cluster score vectors...\n";
  vector<point> c;
  for(auto cluster : clusters){
    //Create vector for cluster
    point score("coin_scores", coins.size());

    for(auto tweetId : cluster){
      //For each tweet in cluster add score vector to cluster vector
      point tweetScore = getCoinScore(tweets[tweetId], lexicon, coins, coinLexicon);
      score.add(tweetScore);
    }
    c.emplace_back(score);
  }

  for(int i = 0; i < c.size(); i++){
    cout << "\nCluster #" << i+1 << ":\n";
    c.at(i).print();
  }

  outputFile.close();
  return 0;
}
