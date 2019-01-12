#include "util.hpp"
#include "point.hpp"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;


/***************************** Configuration Class ****************************/


configuration::configuration(){
  //Initialize configuration with default values, or -1 if there is no default
  clusterCount = -1;

  hashFuncCount = 4;
  hashTableCount = 5;

  HCDimension = 5;
  HCMaxPointChecks = 5000;
  HCMaxVertexChecks = 20;

  maxIterations = 100;

  initialise = "random";
  assign = "lloyds";
  update = "kmeans";

  metric = "euclidean";
}

bool configuration::clusterConf(){
  if(clusterCount <= 0){
    return false;
  }

  if(initialise == "lsh"){
    if(hashFuncCount <= 0 || hashTableCount <= 0){
      return false;
    }
  }
  else if(initialise == "hypercube"){
    if(HCDimension <= 0 || HCMaxPointChecks <= 0 || HCMaxVertexChecks <= 0){
      return false;
    }
  }

  return true;
}

void configuration::print(){
  cout << "Number of clusters: " << getClusterCount() << "\n";
  if(assign == "lsh"){
    cout << "Number of hash functions: " << getHashFuncCount() << "\n";
    cout << "Number of hash tables: " << getHashTableCount() << "\n";
  }
  else if(assign == "hypercube"){
    cout << "Hypercube dimension: " << getHCDimension() << "\n";
    cout << "Max point checks: " << getHCMaxPointChecks() << "\n";
    cout << "Max vertex checks: " << getHCMaxVertexChecks() << "\n";
  }
}


/****************************** Distance Function *****************************/


float distance(point p1, point p2, string metric){
  if(metric == "euclidean"){
    return p1.euclDist(p2);
  }
  else if(metric == "cosine"){
    return p1.cosDist(p2);
  }
  else{
    return -1;
  }
}


/************************** Utility/Helping Functions *************************/


point *get_true_nn(point q, double &minDist, vector<point> *points, string metric){
  minDist = distance(q, points->at(0), metric);
  point *nn = &(points->at(0));

  for(vector<point>::iterator p = points->begin(); p != points->end(); p++){
    double dist = distance(q, *p, metric);
    if(dist < minDist){
      minDist = dist;
      nn = &(*p);
    }
  }

  return nn;
}

int getPoints(fstream &fs, vector<point> &points){
  int dim;
  int i = 0;
  string line;

  //Read a line from input file
  getline(fs, line);
  while(!fs.eof()){
    //Create a point from the line read
    point p(line);
    //Check for consistency of dimensions
    if(i == 0){
      dim = p.dim();
    }
    else{
      if(dim != p.dim()){
        return -1;
      }
    }
    i++;

    //Store new point
    points.push_back(p);
    //Read next line
    getline(fs, line);
  }
  return dim;
}

void readInputFile(string inputFileName, vector<point> &points, int &dim, string &metric){
  //Open input file
  fstream inputFile(inputFileName, fstream::in);
  //Get metric
  getline(inputFile, metric);
  metric.erase(0, 8); //erase "@metric"
  if(metric.find_first_of(" \t\n\r") != string::npos){
    metric.erase(metric.find_first_of(" \t\n\r"), metric.npos);
  }
  cout << "metric = \"" << metric << "\"\n";
  //Read and store dataset
  dim = getPoints(inputFile, points);
  if(dim == -1){
    cerr << "Error: Not all vectors are of the same dimension\n";
  }
  inputFile.close();
}

void readInputFile(string inputFileName, vector<point> &points, int &dim){
  //Open input file
  fstream inputFile(inputFileName, fstream::in);
  //Read and store dataset
  dim = getPoints(inputFile, points);
  if(dim == -1){
    cerr << "Error: Not all vectors are of the same dimension\n";
  }
  inputFile.close();
}

void readQueryFile(string queryFileName, vector<point> &queries, int &dim, double &radius){
  //Open query file
  fstream queryFile(queryFileName, ios_base::in);
  //Get radius
  string line;
  getline(queryFile, line);
  line.erase(0,8);
  radius = atof(line.c_str());
  cout << "radius = " << radius << "\n";
  //Read and store query points
  dim = getPoints(queryFile, queries);
  if(dim == -1){
    cerr << "Error: Not all vectors are of the same dimension\n";
  }
  queryFile.close();
}

void readConfigFile(string configFileName, configuration &conf){
  //Open configuration file
  fstream configFile(configFileName, ios_base::in);
  //Read a line from configuration file
  string line;
  getline(configFile, line);
  while(!configFile.eof()){
    //Extract name and value
    istringstream iss(line);
    string var, value;
    getline(iss, var, ' ');
    getline(iss, value, ' ');

    if(var == "number_of_clusters:"){
      conf.setClusterCount(stoi(value));
    }
    else if(var == "number_of_hash_functions:"){
      conf.setHashFuncCount(stoi(value));
    }
    else if(var == "number_of_hash_tables:"){
      conf.setHashTableCount(stoi(value));
    }
    else if(var == "hypercube_dimension:"){
      conf.setHCDimension(stoi(value));
    }
    else if(var == "hypercube_max_point_checks:"){
      conf.setHCMaxPointChecks(stoi(value));
    }
    else if(var == "hypercube_max_vertex_checks:"){
      conf.setHCMaxVertexChecks(stoi(value));
    }
    else if(var == "max_iterations:"){
      conf.setMaxIterations(stoi(value));
    }
    else if(var == "initialise:"){
      conf.setInitialise(value);
    }
    else if(var == "assign:"){
      conf.setAssign(value);
    }
    else if(var == "update:"){
      conf.setUpdate(value);
    }
    else if(var == "metric:"){
      conf.setMetric(value);
    }
    else{
      cerr << "Error: Unknown variable \"" << var << "\" given in configuration file\n";
    }

    //Read next line
    getline(configFile, line);
  }

  configFile.close();
}

void readLexiconFile(string lexiconFileName, unordered_map<string, float> &lexicon){
  int entries = 0;
  //Open query file
  fstream lexiconFile(lexiconFileName, ios_base::in);
  //Get words and score
  string word, score_str;
  float score;
  string line;
  getline(lexiconFile, line);
  while(!lexiconFile.eof()){
    //Extract word and score
    istringstream iss(line);
    string var, value;
    getline(iss, word, '\t');
    getline(iss, score_str, ' ');
    score = atof(score_str.c_str());

    lexicon.emplace(word, score);
    entries++;

    getline(lexiconFile, line);
  }

  cout << "Created lexicon of " << entries << " entries\n";
  lexiconFile.close();
}

void readCoinsFile(string coinsFileName, vector<string> &coins, unordered_map<string, int> &coinLexicon){
  int entries = 0;
  //Open coins file
  fstream coinsFile(coinsFileName, ios_base::in);
  //Get words and score
  string coin, keyword;
  string line;
  getline(coinsFile, line);
  while(!coinsFile.eof()){
    //Extract coin name
    istringstream iss(line);
    string var, value;
    getline(iss, coin, '\t');

    //Add new coin to structures
    coins.emplace_back(coin);
    coinLexicon.emplace(coin, entries);
    //Get all keywords about coin
    getline(iss, keyword, '\t');
    while(!iss.eof()){
      coinLexicon.emplace(keyword, entries);

      getline(iss, keyword, '\t');
    }

    entries++;
    getline(coinsFile, line);
  }

  cout << "Created lexicon of keywords for " << entries << " coins\n";
  coinsFile.close();
}

void readTweetsFile(string tweetsFileName, vector<tweet> &tweets){
  int entries = 0;
  //Open tweets file
  fstream tweetsFile(tweetsFileName, ios_base::in);
  //Get tweets
  string userId, tweetId, word;
  string line;
  getline(tweetsFile, line);
  while(!tweetsFile.eof()){
    tweet curTweet;
    //Extract ids
    istringstream iss(line);
    string var, value;
    getline(iss, userId, '\t');
    curTweet.userId = stoi(userId);
    getline(iss, tweetId, '\t');
    curTweet.tweetId = stoi(tweetId);

    //Get all words in tweet
    while(!iss.eof()){
      getline(iss, word, '\t');
      curTweet.words.emplace_back(word);
    }

    tweets.emplace_back(curTweet);
    entries++;
    getline(tweetsFile, line);
  }

  cout << "Loaded file of " << entries << " tweets\n";
  tweetsFile.close();
}

void readTweetsFile(string tweetsFileName, unordered_map<int, tweet> &tweets){
  int entries = 0;
  //Open tweets file
  fstream tweetsFile(tweetsFileName, ios_base::in);
  //Get tweets
  string userId, tweetId, word;
  string line;
  getline(tweetsFile, line);
  while(!tweetsFile.eof()){
    tweet curTweet;
    //Extract ids
    istringstream iss(line);
    string var, value;
    getline(iss, userId, '\t');
    curTweet.userId = stoi(userId);
    getline(iss, tweetId, '\t');
    curTweet.tweetId = stoi(tweetId);

    //Get all words in tweet
    while(!iss.eof()){
      getline(iss, word, '\t');
      curTweet.words.emplace_back(word);
    }

    tweets.emplace(curTweet.tweetId, curTweet);
    entries++;
    getline(tweetsFile, line);
  }

  cout << "Loaded file of " << entries << " tweets\n";
  tweetsFile.close();
}

void readClustersFile(string clustersFileName, vector<vector<int>> &clusters){
  //Make sure clusters vector is empty

  int entries = 0;
  //Open tweets file
  fstream clustersFile(clustersFileName, ios_base::in);
  //Go to contents part
  string line;
  getline(clustersFile, line);
  while(!clustersFile.eof()){
    if(line.compare("# Cluster Contents") == 0){
      break;
    }
    else{
      getline(clustersFile, line);
    }
  }
  //Get clusters
  string userId, tweetId, word;
  getline(clustersFile, line);
  while(!clustersFile.eof()){
    vector<int> curCluster;
    //Get rid of first characters
    istringstream iss(line);
    string id;
    getline(iss, id, '{');
    //Extract ids
    while(!iss.eof()){
      getline(iss, id, ' ');
      id.pop_back();
      curCluster.push_back(stoi(id.c_str()));
    }

    clusters.emplace_back(curCluster);
    entries++;
    getline(clustersFile, line);
  }

  cout << "Loaded file of " << entries << " clusters\n";
  clustersFile.close();
}
