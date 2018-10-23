#include "point.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h> //atoi
#include <cmath>
#include <random>

using namespace std;

point::point(string name, int dim, int min, int max){
  //Initialise random real number generator
  uniform_real_distribution<double> unif(min, max);
  default_random_engine re;

  this->name = name;
  for(int i = 0; i < dim; i++){
    vals.push_back(unif(re));
  }
}

point::point(std::string line){
  int i = 0;
  for(int j = 0; j < line.length(); j++){
    if(line[j] == '\t'){
      if(i == 0){
        name = line.substr(i, j - i);
      }
      else{
        vals.push_back(atof(line.substr(i, j - i).c_str()));
      }
      i = j + 1;
    }
  }
  if(i < line.length()){
    vals.push_back(atof(line.substr(i).c_str()));
  }
}

void point::print(){
  cout << name << ": ";
  for(vector<double>::iterator it = vals.begin(); it != vals.end(); it++){
    cout << *it << ' ';
  }
  cout << '\n';
}

int point::dim(){
  return vals.size();
}

string point::getName(){
  return name;
}

double point::distance(point p){
  if(this->vals.size() != p.vals.size()){
    return -1.0;
  }
  double dist = 0.0;
  vector<double>::iterator it1 = this->vals.begin();
  vector<double>::iterator it2 = p.vals.begin();

  for(int i = 0; i < this->vals.size(); i++){
    dist += pow(*it1 - *it2, 2);

    it1++;
    it2++;
  }

  dist = sqrt(dist);
  return dist;
}
