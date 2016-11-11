/*
 ***************************************************************************
 *   Copyright (C) 2015, Eray Uzgoren                                      *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************
*/
#include <girdap>

// --- SECTION 1 -- Constructors; 
// Base constructor for setting defaults; 
Grid::Grid() {                   
  nCellSize = 0;
  for (auto i = 0; i<3; ++i) {
    levelLowBound[i] = 0; levelHighBound[i] = 4; cfl = 0.5;
  }
  filecnt=0;
}

// For large grids; better to start with reserved memory;
// Note it can go beyond cap; 
Grid::Grid(int_8 cap):Grid::Grid() {
  listCell.reserve(cap);
  listVertex.reserve(cap);
}

// Start with a single vertex ???
Grid::Grid(initializer_list<double > pts):Grid::Grid() {
  addVertex(pts);
}
// Vec3 form 
Grid::Grid(Vec3 pts): Grid::Grid() {
  addVertex(pts); 
}

// Start with many vertices; size adjusted; 
Grid::Grid(initializer_list<initializer_list<double > > pts):Grid::Grid() {
  listCell.reserve(2*pts.size());
  listVertex.reserve(2*pts.size());
  addVertex(pts);  
}

// Start with vertices; and cells
Grid::Grid(initializer_list<initializer_list<double> > pts, 
	   initializer_list<initializer_list<int_8> > cell): Grid::Grid(pts) {
  addCell(cell);  
  setCurrentLevels(); 
  makeFace(); 
  //setQuadBoundary(); 
  cout << "Block2: Cells: " << listCell.size(); 
  cout << " Faces: " << nFace << endl; 
  addVec("u");  
}
// -- End section constructors; 


// Section 2 - Deconstructors
Grid::~Grid() {
  listFace.clear();
  listCell.clear();
  listVertex.clear();
  otherVertex.clear();
}
// End section 2 - Deconstructors

// Section 3 - Copy
// Grid::Grid(const Grid& copy) {
//   filecnt = copy.filecnt;
  
// }



void Grid::addVar(std::string n, int t) {
  if (!getVar(n)) {
    listVar.emplace_back(shared_ptr<Var>(new Var(n, t))); 
    if (t == 1) {
      (*listVar.rbegin())->data.resize(listVertex.size());
    } else if (t == 2) {
      (*listVar.rbegin())->data.resize(listFace.size());
    } else {
      (*listVar.rbegin())->data.resize(listCell.size());
    }
    (*listVar.rbegin())->grid = this; 
    (*listVar.rbegin())->solver = "BiCGSTAB";
    (*listVar.rbegin())->initBC(); 
  } else 
    cout << "Skipping " << n << " as it exists"<< endl; 
} 

void Grid::addVar(initializer_list<std::string> nl, int t) {
  for (auto n: nl) addVar(n, t); 
}

void Grid::addVec(std::string n, int t) {
  if (n == "u") {
    addVar({"u", "v", "w"}, t); 
    getVar("u")->isVec = true; 
    getVar("v")->isVec = true; 
    getVar("w")->isVec = true; 
  } else {
    addVar(n+"x", t); getVar(n+"x")->isVec = true;  
    addVar(n+"y", t); getVar(n+"y")->isVec = true;  
    addVar(n+"z", t); getVar(n+"z")->isVec = true;  
  }
}

void Grid::addVec(initializer_list<std::string> nl, int t) {
  for (auto n: nl) addVec(n, t); 
}

shared_ptr<Var> Grid::getVar(std::string n) { 
  auto matching_iter = std::find_if(listVar.begin(), listVar.end(),
				    [&n](const shared_ptr<Var> p) {
				      return n == p->name;
				    });    
  return (matching_iter!=listVar.end()) ? *matching_iter : NULL; 
}
  
void Grid::lockBC(shared_ptr<Var> v) {
  thisVar = v;
  v->grad = valGrad(v); 
  //  v->grad.assign(listCell.size(), 0)
  // v->grad.clear(); 
  // v->grad.resize(listCell.size()); 
  // for (auto i = 0; i < listCell.size(); ++i) {
  //   v->grad[i] = listCell[i]->grad(v); //v->grad = valGrad(v); 
  // }
}
void Grid::unlockBC() {
  thisVar.reset(); 
}
