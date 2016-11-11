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
#include <iostream>
#include <math.h>
#include <fstream>
#include <sstream>
#include <time.h>
#include <iomanip>

#include <field/Var>
#include <grid2/Grid>


int main() {
  auto dt = 0.5; auto writeTime = 0.02; 
  auto t = clock(); 
  Block2* grid = new Block2({0, 0, 0}, {1, 1, 0}, 10, 10);
  //Block2* grid = new Block2({0, 0, 0}, {0.02, 0.02, 0}, 10, 1);
  double time= 0; double endTime = 8; //dt*50; 

  grid->addVar({"T"}); 
    
  auto T = grid->getVar("T");
  T->setBC("west", "val", 0);
  T->setBC("south", "val", 0); 
  T->setBC("east", "val", 0); 
  T->setBC("north", "val", 0); 

  auto u = grid->getVar("u"); 
  auto v = grid->getVar("v"); 

  int filecnt = 0; int it = 0, writeInt = 1; 
  ofstream myfile;   
  std::string flname;

  flname="heat"+std::to_string(filecnt++)+".vtk"; 
  myfile.open(flname); 
  myfile << grid << endl;
  myfile.close();   

  double pi = 4.0*atan(1);
  for (auto k = 0; k < 60; ++k) {
  for (auto j0 = 0; j0 < 6; ++j0) {
    for (auto i = 0; i < grid->listCell.size(); ++i) {
      auto xc = 0.5 + 0.25*cos(pi/2 - k*pi/60); 
      auto yc = 0.5 + 0.25*sin(pi/2 - k*pi/60); 
      auto r = (grid->listCell[i]->getCoord() - Vec3(xc, yc)).abs(); 
      T->set(i, 1.0/(1.0 + exp(-2.0*80*(0.15-r)))); 
    }
    auto err = grid->getError(T); 
    u->set(err.comp(0));
    v->set(err.comp(1)); 

    flname="heat"+std::to_string(filecnt++)+".vtk"; 
    myfile.open(flname); 
    myfile << grid << endl;
    myfile.close();     

    //    auto gt = grid->valGrad(T); 
    grid->solBasedAdapt2(err);

    flname="heat"+std::to_string(filecnt++)+".vtk"; 
    myfile.open(flname); 
    myfile << grid << endl;
    myfile.close();     

    grid->adapt(); 

    // // write file
    flname="heat"+std::to_string(filecnt++)+".vtk"; 
    myfile.open(flname); 
    myfile << grid << endl;
    myfile.close();     
  } 

  // for (auto j0 = 0; j0 < 5; ++j0) {
  //   for (auto i = 0; i < grid->listCell.size(); ++i) {
  //     auto r = (grid->listCell[i]->getCoord() - Vec3(0.75, 0.5)).abs(); 
  //     T->set(i, 1.0/(1.0 + exp(-2.0*80*(0.15-r)))); 
  //   }
  //   auto err = grid->getError(T); 
  //   u->set(err.comp(0));
  //   v->set(err.comp(1)); 

  //   flname="heat"+std::to_string(filecnt++)+".vtk"; 
  //   myfile.open(flname); 
  //   myfile << grid << endl;
  //   myfile.close();     

  //   //    auto gt = grid->valGrad(T); 
  //   grid->solBasedAdapt2(err);    

  //   // // write file
  //   flname="heat"+std::to_string(filecnt++)+".vtk"; 
  //   myfile.open(flname); 
  //   myfile << grid << endl;
  //   myfile.close();     

  //   grid->adapt(); 

  //   flname="heat"+std::to_string(filecnt++)+".vtk"; 
  //   myfile.open(flname); 
  //   myfile << grid << endl;
  //   myfile.close();     


  // } 
  }

  delete(grid); 

  return 0; 
}
