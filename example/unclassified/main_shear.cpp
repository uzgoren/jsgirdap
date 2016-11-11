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
  auto dt = 0.5; auto writeTime = 0.01; 
  auto t = clock(); 
  Block2* grid = new Block2({0, 0, 0}, {1, 1, 0}, 20, 20);
  //Block2* grid = new Block2({0, 0, 0}, {0.02, 0.02, 0}, 10, 1);
  double time= 0; double endTime = 4; //dt*50; 

  // Field variables; 
  double rho=10000, cp=1000, k=1; 

  grid->addVar({"T", "p"}); 
    
  auto T = grid->getVar("T");
  auto u = grid->getVar("u");
  auto v = grid->getVar("v");
  
  //T->solver = "Gauss"; 
  
  v->set(0); 
  T->set(0); 
    double pi = 4.0*atan(1); 
  
  for (auto j = 0; j < 5; ++j) {
    for (auto i = 0; i < grid->listCell.size(); ++i) {
      auto x = grid->listCell[i]->getCoord(); // - Vec3(0.5, 0.5); 
      u->set(i, 2*x[1]-1.0); //-2*sin(pi*x[1])*cos(pi*x[1])*sin(pi*x[0])*sin(pi*x[0]));

      //      auto r = (grid->listCell[i]->getCoord() - Vec3(0.5, 0.5)).abs(); 

      //T->set(i, 1.0/(1.0 + exp(-2.0*80*(0.15-r)))); 
      if (x[0] > 0.35 && x[0] < 0.65 && x[1] > 0.35 && x[1]< 0.65 ) {
       	T->set(i, 1); 
      }
    }
    auto gt = grid->valGrad(T); 
    grid->solBasedAdapt(gt, 0.5); 
    grid->refine2(); 
  }	 

  int filecnt = 0; int it = 0, writeInt = 1; 
  ofstream myfile;   
  std::string flname="heat"+std::to_string(filecnt++)+".vtk"; 
  myfile.open(flname); 
  myfile << grid << endl;
  myfile.close();   

  cin.ignore().get(); 

  T->setBC("west", "val", 0);
  T->setBC("south", "val", 0); 
  T->setBC("east", "val", 0); 
  T->setBC("north", "val", 0); 
  // T->setBC("south", "grad", -20, 100);   
  // T->setBC("north", "grad", -20, 100);  
  T->itmax = 1000; 
  T->tol = 1e-6;

  // u->set(1); 
  // v->set(1); 


  auto writeCnt = writeTime; 
  while (time < endTime) {
    for (auto i = 0; i < grid->listCell.size(); ++i) {
      auto x = grid->listCell[i]->getCoord(); // - Vec3(0.5, 0.5); 
      u->set(i, 2*x[1]-1.0);
    //   u->set(i, -2*sin(pi*x[1])*cos(pi*x[1])*sin(pi*x[0])*sin(pi*x[0])*cos(pi*time/endTime));
    //   v->set(i, 2*sin(pi*x[0])*cos(pi*x[0])*sin(pi*x[1])*sin(pi*x[1])*cos(pi*time/endTime)); 
    }
    grid->setDt(dt); 

 
    cout << setiosflags(ios::fixed) << setprecision(6); 
    cout << "------------- Processing TIME = "<< time << " ------------------"<<endl; 

    auto vel = grid->getVel();
    
    grid->lockBC(T); 
    T->solve (grid->ddt(1.0) 
     	      +grid->divRK2E(vel, 1.0)
	      //- grid->laplace(k/cp/rho) 
	      //- grid->source(0, 100000/cp/rho)
	      //- grid->laplace(1.0) 
	      //- grid->source(-25, 25*20)
	       ); 
    grid->unlockBC();     

    auto gt = grid->valGrad(T); 
    grid->solBasedAdapt(gt, 0.5); 
    grid->refine2(); 


    time += grid->dt; 
    writeCnt -= grid->dt; 
    grid->setDt(dt); 

    if (writeCnt <= 0) {
      std::string flname="heat"+std::to_string(filecnt++)+".vtk"; 
      myfile.open(flname); 
      myfile << grid << endl;
      myfile.close();   
      writeCnt = writeTime; 
     } 

    cout << "---------------------------------------------------"<<endl; 
  }


  // myfile.open("reg.vtk"); 
  // myfile << grid << endl;
  // myfile.close();    
 
  //grid->writeFace(); 
 
  //grid->writeFace(); 

  delete(grid); 


  // LinSys ls(6); 
  // VecX<double> x(6); x.uncompress();
  // ls.A = {
  //   { 2, -1,  0,  0,  0,  0}, 
  //   {-1,  2, -1,  0,  0,  0}, 
  //   { 0, -1,  2, -1,  0,  0}, 
  //   { 0,  0, -1,  2, -1,  0}, 
  //   { 0,  0,  0, -1,  2, -1},
  //   { 0,  0,  0,  0, -1,  2}    
  // };
  // // ls.A[5] = 
  // // //vel.A[5][4] = -0.5; 

  // ls.A.info(); 
  // ls.b.info(); 

  // x = {1, -2, 3, -4, 9, 3}; 
  // cout << ls.A << endl; 
  // cout << ls.b << endl; 
  // cout << x<<endl; 
  // ls.b = (ls.A)*(x);
  // cout << ls.b << endl;
  // x = {0, 0, 0, 0, 0, 0}; 

  // ls.x = &x;
  // ls.setLimits(1e-8); 

  // // //vel.Gauss_Seidel();
  // ls.BiCGSTAB();
  // cout <<"x: "<< x<< endl;



  return 0; 
}
