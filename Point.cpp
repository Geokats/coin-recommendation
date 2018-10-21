#include "Point.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h> //atoi

using namespace std;

Point::Point(std::string line){
  int i = 0;
  for(int j = 0; j < line.length(); j++){
    if(line[j] == '\t'){
      if(i == 0){
        name = line.substr(i, j - i);
      }
      else{
        vals.push_back(atoi(line.substr(i, j - i).c_str()));
      }
      i = j + 1;
    }
  }
  if(i < line.length()){
    vals.push_back(atoi(line.substr(i).c_str()));
  }
}

void Point::print(){
  cout << name << ": ";
  for(vector<int>::iterator it = vals.begin(); it != vals.end(); it++){
    cout << *it << ' ';
  }
  cout << '\n';
}

int Point::dim(){
  return vals.size();
}
