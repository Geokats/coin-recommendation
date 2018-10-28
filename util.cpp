#include "util.hpp"
#include "point.hpp"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

point *get_true_nn(point q, double &minDist, vector<point> *points){
  minDist = q.distance(points->at(0));
  point *nn = &(points->at(0));

  for(vector<point>::iterator p = points->begin(); p != points->end(); p++){
    double dist = q.distance(*p);
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
  fstream inputFile(inputFileName, ios_base::in);
  //Get metric
  getline(inputFile, metric);
  metric.erase(0,8);
  metric.erase(metric.find_first_of(" \t\n\r"), metric.npos);
  cout << "metric = \"" << metric << "\"\n";
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
