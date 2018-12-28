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

using namespace std;

string usageStr = "./cluster -i <input file> -o <output file>\n";

int main(int argc, char* const *argv) {
  //Command line arguments
  char *tweetsFileName = NULL;
  char *outputFileName = NULL;
  //Other arguments
  string lexiconFileName = "data/vader_lexicon.csv";
  string coinsFileName = "data/coins_queries.csv";


  char c;

  //Read command line arguments
  while((c = getopt(argc, argv, "i:o:")) != -1){
    switch(c){
      case 'i':
        tweetsFileName = optarg;
        break;
      case 'o':
        outputFileName = optarg;
        break;
      default:
        cerr << "Usage: " << usageStr;
        return 1;
    }
  }

  //Check command line arguments
  if(tweetsFileName == NULL || outputFileName == NULL){
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
  vector<tweet> tweets;
  cout << "Loading tweets from " << tweetsFileName << "...\n";
  readTweetsFile(tweetsFileName, tweets);

  //Open output file
  fstream outputFile(outputFileName, ios_base::out);


  outputFile.close();
  return 0;
}
