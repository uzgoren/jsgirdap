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
  auto dt = 0.01; auto writeTime = 0.005; 
  auto t = clock(); int iter = 0; 
  Block2* grid = new Block2({0, 0, 0}, {1, 1, 0}, 10, 10);
  double time= 0; double endTime = 8; //dt*50; 

  // Add Variables 
  grid->addVar({"T", "P"}); 
  // grid->addVar("ov", 1); 
    
  auto T = grid->getVar("T");
  auto P = grid->getVar("P");
  auto u = grid->getVar("u");
  auto v = grid->getVar("v");

  // auto ov = grid->getVar("ov"); 

  // Set indicator function in T; and adapt grid to a max level; 
  T->set(0); P->set(0); 
  double pi = 4.0*atan(1);  
  for (auto j = 0; j < 4; ++j) {
    auto dx = grid->listCell[0]->edge(0).abs();
    for (auto i = grid->listCell[0]->level[0]; i < grid->levelMax[0]+1; ++i) dx *= 0; 
    
    for (auto i = 0; i < grid->listCell.size(); ++i) {
      auto x = grid->listCell[i]->getCoord(); // - Vec3(0.5, 0.5); 
      u->set(i, -2*sin(pi*x[1])*cos(pi*x[1])*sin(pi*x[0])*sin(pi*x[0]));
      v->set(i, 2*sin(pi*x[0])*cos(pi*x[0])*sin(pi*x[1])*sin(pi*x[1])); 

      auto r = 0.15 - (x - Vec3(0.5, 0.75)).abs(); 
      
      T->set(i, 1.0/(1.0 + exp(-2.0*80*(r)))); 
      // r = (x - Vec3(0.5, 0.05)).abs(); 
      // T->set(i, max(0.0, min(1.0, T->get(i) + 1.0/(1.0 + exp(-2.0*80*(0.15-r))))));
      // if (x[0] >= 0.8 && x[1] >= 0.4 && x[1] <= 0.6) 
      //  	T->set(i, max(0.0, min(1.0, T->get(i)+1.0)));
      // if (x[0] >= 0.47 && x[0] <= 0.53 && x[1] >= 0.5 && x[1] <= 0.7) 
      //  	T->set(i, max(0.0, min(1.0, T->get(i)-1.0)));      
    }    
    grid->solBasedAdapt2(grid->getError(T)); 
    grid->adapt(); 
  }

  Grid* sint = new Grid(); 
  grid->contour2(sint, T, 0.5);
  sint->checkGeo(); 
  
  // ov->set(-1); 
  // for (auto i = 0; i < grid->listVertex.size(); ++i) {
  //   if (grid->otherVertex[i] < 0) continue; 
  //   ov->set(i, 1); 
  // }

  // Assign velocity variables to it 
  sint->addVec("u",1); 

  // Get velocity on [L]
  auto us = sint->getVar("u"); auto vs = sint->getVar("v");   
  grid->passVar(sint, u, us); 
  grid->passVar(sint, v, vs); 

  // Initial grid  
  grid->writeVTK("euler"); 
  sint->writeVTK("ibm"); 
  
  // TIME LOOP
  for (auto ti = 0; ti < endTime/dt+1; ++ti) {
    cout << endl << "[ " << ti << " ] "; 

    // set time dependent velocity field on [E]   
    for (auto i = 0; i < grid->listCell.size(); ++i) {
      auto x = grid->listCell[i]->getCoord(); // - Vec3(0.5, 0.5); 
      u->set(i, -2*sin(pi*x[1])*cos(pi*x[1])*sin(pi*x[0])*sin(pi*x[0])*cos(pi*ti*dt/endTime));
      v->set(i, 2*sin(pi*x[0])*cos(pi*x[0])*sin(pi*x[1])*sin(pi*x[1])*cos(pi*ti*dt/endTime)); 
    }

    //----------------------------------------------------------------------
    // RK2 [L] advection
    //---------------------------------------------------------------------
    // Get velocity on [L]
    grid->passVar(sint, u, us); 
    grid->passVar(sint, v, vs); 
    
    vector<Vec3> xold(sint->listVertex.size());    
    for (auto i = 0; i < sint->listVertex.size(); ++i) {
      xold[i] = *sint->listVertex[i]; 
      sint->listVertex[i]->set(xold[i] + Vec3(us->get(i)*dt*0.5, vs->get(i)*dt*0.5, 0));
      // Locations changed so update the list;
      grid->updateOtherVertex(sint); 
    }
    
    // Get velocity on [L] at intermediate locations; 
    grid->passVar(sint, u, us); 
    grid->passVar(sint, v, vs); 
    for (auto i = 0; i < sint->listVertex.size(); ++i) {  
      sint->listVertex[i]->set(xold[i] + Vec3(us->get(i)*dt, vs->get(i)*dt)); 
    }
    grid->updateOtherVertex(sint);     
    // location update completed
    // --- RK2- END------------------------------------------------------

    // ?? correct grid ??  vol and connectivity; 
    
    //--------------------------------------------------------------------
    // [E] adaptation
    //-------------------------------------------------------------------

    // Contruct T at new location for adaptation;     
    grid->indicator(sint, T);  
    grid->solBasedAdapt2(grid->getError(T));
    grid->adapt();
    grid->updateOtherVertex(sint);     
    // // refresh interface
    // grid->contour2(sint, T, 0.5);
    sint->checkGeo();  
    
    grid->writeVTK("euler"); 
    sint->writeVTK("ibm"); 
    //  exit(1); 
  }
  
  exit(0); 

  double mass0=0; double mass=0; 
  for (auto i=0; i < grid->listCell.size(); ++i) {
    mass0 += grid->listCell[i]->vol().abs()*T->get(i); 
  }

  grid->writeVTK("heat"); 
  // flname="heat"+std::to_string(filecnt++)+".vtk"; 
  // myfile.open(flname); 
  // myfile << grid << endl;
  // myfile.close();   

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
    iter++; 
    for (auto i = 0; i < grid->listCell.size(); ++i) {
      auto x = grid->listCell[i]->getCoord(); // - Vec3(0.5, 0.5); 
      u->set(i, -2*sin(pi*x[1])*cos(pi*x[1])*sin(pi*x[0])*sin(pi*x[0])*cos(pi*time/endTime));
      v->set(i, 2*sin(pi*x[0])*cos(pi*x[0])*sin(pi*x[1])*sin(pi*x[1])*cos(pi*time/endTime)); 
    }
    grid->setDt(2.0*dt); 

 
    cout << setiosflags(ios::fixed) << setprecision(6); 
    cout << "------------- Processing TIME = "<< time << " ------------------"<<endl; 
    cout << "  Mass: " << mass << " Initial: " << mass0 << endl; 

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

    // remove over-shoot and undershoot
    //    for (auto i = 0; i < grid->listCell.size(); ++i) { 
    //}

    mass=0; double part=0; 
    for (auto i=0; i < grid->listCell.size(); ++i) {
      double vol = grid->listCell[i]->vol().abs();
      //      T->set(i, max(min(T->get(i)+1e-5, 1.0), 0.0)); 
      T->set(i, min(T->get(i)+1e-4, 1.0));
      T->set(i, max(T->get(i)-1e-4, 0.0)); 
      double Tval = T->get(i); 
      mass += vol*Tval;
      if (Tval > 0) part += vol; 
    }

    // double corr = (mass - mass0)/part; mass = 0; 
    // for (auto i=0; i < grid->listCell.size(); ++i) { 
    //   double vol = grid->listCell[i]->vol().abs();
    //   if (T->get(i) > 0) 
    // 	T->set(i, max(min(T->get(i)+corr*vol, 1.0), 0.0)); 
    //   mass += vol*T->get(i); 
    // }
	     
    
    //    auto gt = grid->valGrad(T); 
    if (iter%1 == 0) {
      grid->solBasedAdapt2(grid->getError(T)); 
      grid->adapt(); 
    }

    time += grid->dt; 
    writeCnt -= grid->dt; 
    grid->setDt(dt); 

    if (writeCnt <= 0 || time >= endTime) {
      grid->writeVTK("heat"); 
      writeCnt = writeTime; 
    } 

    cout << "---------------------------------------------------"<<endl; 
  }

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
