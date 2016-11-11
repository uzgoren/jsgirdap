#ifndef ADAPT
#define ADAPT

void solBasedAdapt(VecX<double> phi, double a=1.0, double b = 0.5) {
  double icnt=0, sumx=0, varx=0, sumy=0, vary=0, sumz=0, varz=0; 
  //double a = 0.5; 
  if (phi.size() != listCell.size()) {
    cout << "solBasedAdapt: CANNOT process a variable with a different size than cell size!"<<endl;
    exit(1); 
  }
  for (auto i = 0; i < listCell.size(); ++i) { 
    auto qx = abs(phi[i])*pow(listCell[i]->dx(),1.5);  
    auto qy = abs(phi[i])*pow(listCell[i]->dy(),1.5);  
    auto qz = abs(phi[i])*pow(listCell[i]->dz(),1.5);  
    ++icnt; 
    sumx += qx; varx += qx*qx; 
    sumy += qy; vary += qy*qy; 
    sumz += qz; varz += qz*qz; 
  }
  varx = sqrt((varx - sumx*sumx/icnt)/(icnt-1)); 
  vary = sqrt((vary - sumy*sumy/icnt)/(icnt-1)); 
  varz = sqrt((varz - sumz*sumz/icnt)/(icnt-1)); 
  sumx /=icnt; 
  sumy /=icnt; 
  sumz /=icnt; 
    
  for (auto i = 0; i < listCell.size(); ++i) {
    auto c=listCell.at(i);
    auto qx = abs(phi[i])*pow(c->dx(),1.5);  
    auto qy = abs(phi[i])*pow(c->dy(),1.5);  
    auto qz = abs(phi[i])*pow(c->dz(),1.5);  

    if ((qx - sumx) > a*varx)
      c->adapt[0] = min(1, levelHighBound[0] - c->level[0]);
    if ((qy - sumy) > a*vary)
      c->adapt[1] = min(1, levelHighBound[1] - c->level[1]);
    if ((qz - sumz) > a*varz)
      c->adapt[2] = min(1, levelHighBound[2] - c->level[2]);
  } 
}

void solBasedAdapt3(VecX<double> phi, double lowlim=0.02, double highlim=0.1) {

  for (auto i = 0; i < listFace.size(); ++i) {
    auto f = listFace[i]; 
    auto n = f->next; 
    auto p = f->prev; 
      
    if (phi[i] > highlim) { 
      if (n >= 0) {
	auto c = listCell[n]; 
	c->adapt[f->orient] = min(1, levelHighBound[f->orient] - c->level[f->orient]);
      }
      if (p >= 0) {
	auto c = listCell[p]; 
	c->adapt[f->orient] = min(1, levelHighBound[f->orient] - c->level[f->orient]);
      }
    } else if (phi[i] < lowlim) {
      if (n >= 0) {
	auto c = listCell[n]; 
	c->adapt[f->orient] = max(-1, levelLowBound[f->orient] - c->level[f->orient]);
      }
      if (p >= 0) {
	auto c = listCell[p]; 
	c->adapt[f->orient] = max(-1, levelLowBound[f->orient] - c->level[f->orient]);
      }
    }
  }       
}


void solBasedAdapt2(VecX<Vec3> phi, double lowlim=5e-6, double highlim=8e-5) {
  // refine if err > 0.03; coarsen if err < 1e-6; 
  //  auto lowlim = 5e-6; auto highlim = 8e-5;
  for (auto i = 0; i < listCell.size(); ++i) {
    auto c = listCell[i]; 
    auto qx = abs(phi[i].x());//*listCell[i]->vol().abs();  
    auto qy = abs(phi[i].y());//*listCell[i]->vol().abs();   //*listCell[i]->dy();  
    auto qz = abs(phi[i].z());//*listCell[i]->vol().abs();   //*listCell[i]->dz();  
      
    if (qx > highlim)// 8e-5) 
      c->adapt[0] = min(1, levelHighBound[0] - c->level[0]);
    else if (qx < lowlim)//5e-6) 
      c->adapt[0] = max(-1, levelLowBound[0] - c->level[0]);
    if (qy > highlim) //8e-5) //      if (0.5*qy > sumy)
      c->adapt[1] = min(1, levelHighBound[1] - c->level[1]);
    else if (qy < lowlim)//5e-6)
      c->adapt[1] = max(-1, levelLowBound[1] - c->level[1]);
    if (qz > highlim) //if (0.5*qz > sumz)
      c->adapt[2] = min(1, levelHighBound[2] - c->level[2]);
    else if (qz < lowlim) //5e-6)
      c->adapt[2] = max(-1, levelLowBound[2] - c->level[2]);      
  }       
}

void solBasedAdapt(VecX<Vec3> phi, double a=1.0, double b=0.1) {
  double icnt=0, sumx=0, varx=0, sumy=0, vary=0, sumz=0, varz=0;
  // double a = 0.5; 
  if (phi.size() != listCell.size()) {
    cout << "solBasedAdapt: CANNOT process a variable with a different size than cell size!"<<endl;
    exit(1); 
  }
  for (auto i = 0; i < listCell.size(); ++i) {      
    auto qx = abs(phi[i].x())*pow(listCell[i]->dx(),1.5);  
    auto qy = abs(phi[i].y())*pow(listCell[i]->dy(),1.5);  
    auto qz = abs(phi[i].z())*pow(listCell[i]->dz(),1.5);  
    ++icnt; 
    sumx += qx; varx += qx*qx; 
    sumy += qy; vary += qy*qy; 
    sumz += qz; varz += qz*qz; 
  }
  varx = sqrt((varx - sumx*sumx/icnt)/(icnt-1)); 
  vary = sqrt((vary - sumy*sumy/icnt)/(icnt-1)); 
  varz = sqrt((varz - sumz*sumz/icnt)/(icnt-1)); 
  sumx /=icnt; 
  sumy /=icnt; 
  sumz /=icnt; 

  for (auto i = 0; i < listCell.size(); ++i) {
    auto c=listCell.at(i); 
    auto qx = abs(phi[i].x())*pow(c->dx(),1.5);  
    auto qy = abs(phi[i].y())*pow(c->dy(),1.5);  
    auto qz = abs(phi[i].z())*pow(c->dz(),1.5);  
    if (qx - sumx > a*varx)
      c->adapt[0] = min(1, levelHighBound[0] - c->level[0]);
    else if (qx - sumx < b*varx)
      c->adapt[0] = max(-1, levelLowBound[0] - c->level[0]);
    if (qy - sumy > a*vary)
      c->adapt[1] = min(1, levelHighBound[1] - c->level[1]);
    else if (qy - sumy < b*vary)
      c->adapt[1] = max(-1, levelLowBound[1] - c->level[1]);
    if (qz - sumz > a*varz)
      c->adapt[2] = min(1, levelHighBound[2] - c->level[2]);
    else if (qz - sumz < b*varz)
      c->adapt[2] = max(-1, levelLowBound[2] - c->level[2]);
  }
}

void setCurrentLevels() { 
  if (listCell.empty()) {
    cout << "setCurrentLevels: There is no cell to set levels!"<< endl; 
    exit(1); 
  }
  for (auto j = 0; j < 3; ++j) { 
    levelMin[j] = listCell.at(0)->level[j]; 
    levelMax[j] = listCell.at(0)->level[j];
  } 
  for (auto i = 0; i < listCell.size(); ++i) {
    for (auto j = 0; j < 3; ++j) {
      levelMin[j] = min(levelMin[j], listCell[i]->level[j]); 
      levelMax[j] = max(levelMax[j], listCell[i]->level[j]); 
    }
  }
}


void updateLevels() {
  // in x-direction; 
  for (auto j = 0; j < 3; ++j) {
    for (auto lx = levelMax[j]; lx > levelMin[j]-1; --lx) {
      for (auto i = 0; i < listCell.size(); ++i) {
	auto level0 = listCell[i]->level[j]; 
	auto adapt0 = listCell[i]->adapt[j]; 
	if (level0 + adapt0 != lx + 1) continue; // true maximum 
	auto ngbrList = listCell[i]->ngbrCellList(); 
	for (auto n : ngbrList) {
	  auto level1 = listCell[n]->level[j]; 
	  auto adapt1 = listCell[n]->adapt[j];
	  if (level1 + adapt1 > level0 + adapt0 - 2) continue; 
	  listCell[n]->adapt[j] = adapt0+level0-1;
	}
      }
    }
  }
}
  

void adapt() {  
  auto t = clock(); 
  // [0] FIND cells that needs to be adapted for continuous levels
  for (auto j = 0; j < 3; ++j) {
    for (auto l = levelMax[j]; l > levelMin[j]-1; --l) {
      for (auto i = 0; i < listCell.size(); ++i) {
	auto c = listCell[i]; 
	if (c->level[j] != l) continue; 
	c->checkNgbrLevel(j); 
      }
    }
  }
  //
  for (auto j = 0; j < 3; ++j) {
    for (auto l = levelMax[j]; l > levelMin[j]-1; --l) {
      for (auto i = 0; i < listCell.size(); ++i) {
	auto c = listCell[i]; 
	if (c->level[j] != l) continue; 
	c->checkIslandLevels(j); 
      }
    }
  }
  // [2] Refine cells;
  for (auto j = 0; j < 3; ++j) {
    auto lmin = levelMin[j]; auto lmax = levelMax[j]+1;
    auto l = lmin; 
    while (l < lmax) {
      auto icnt = 0; 
      auto nCell = listCell.size();
      for (auto i = 0; i < nCell; ++i) {
	auto c = listCell[i]; 
	if (c->level[j] != l) continue;
	if (c->adapt[j] <= 0) continue;
	c->refine(j); 
      }
      ++l;
    }
  }
    
  bool isCellRemoved = false; 
  // [3] Coarsen cells (index directions - 3 to be defined)
  if (true) {
    for (auto pass =0; pass < 3; ++pass) { 
      for (auto j = 0; j < 2; ++j) {	
	auto lmin = levelMin[j]; auto lmax = levelMax[j];
	for (auto l = lmax; l > lmin-1; --l) {
	  for (auto i = 0; i < listCell.size(); ++i) {
	    auto c = listCell[i]; 
	    if (!c->isAlive) continue; 
	    if (c->adapt[j] >= 0) continue;
	    if (c->level[j] != l) continue; 
	    if (c->coarsen(j) && !isCellRemoved) { 
	      isCellRemoved = true;
	    }
	  }
	}
      }
    }    
  }

  // [4] correct cell lists;     
  if (isCellRemoved) cleanGrid(); 
  checkGrid(); 

  setCurrentLevels();
  makeFace(); 

  for (auto j = 0; j < 3; ++j) {
    for (auto i = 0; i < listCell.size(); ++i) {
      auto c= listCell[i]; 
      c->adapt[j] = 0;
    }
  }

  t = clock() - t;   
 
  cout << " Cells: " << listCell.size(); 
  cout << " Faces: " << nFace << " in " << ((float)t)/CLOCKS_PER_SEC << " secs."<< endl; 

}



void refine2() {
  auto t = clock(); 
  for (auto i = 0; i < listCell.size(); ++i) { 
    for (auto j = 0; j < 3; ++j) {
      listCell[i]->checkNgbrLevel(j); 
    }
  }
  for (auto i = 0; i < listCell.size(); ++i) { 
    for (auto j = 0; j < 3; ++j) {
      listCell[i]->checkIslandLevels(j); 
    }
  }
  for (auto j = 0; j < 3; ++j) {
    auto lmin = levelMin[j]; auto lmax = levelMax[j]+1;
    auto l = lmin; 
    while (l < lmax) {
      auto icnt = 0; 
      auto nCell = listCell.size();
      for (auto c:listCell) {
	c->checkNgbrLevel(j); 
      }

      for (auto i = 0; i < nCell; ++i) {
	auto c = listCell[i]; 
	if (c->level[j] != l) continue;
	if (c->adapt[j] == 0) continue;
	c->refine(j); 
	icnt++; 
      }
      if (icnt == 0) ++l; 
      setCurrentLevels();
      makeFace(); 
    }
  }
  t = clock() - t;   
 
  cout << " Cells: " << listCell.size(); 
  cout << " Faces: " << nFace << " in " << ((float)t)/CLOCKS_PER_SEC << " secs."<< endl; 

  return; 
}
    
bool coarsen(int_2 dir) {
  bool isAnyCoarsened = false; 
  auto lmin = levelMin[dir]-1; auto lmax = levelMax[dir];
  for (auto l = lmax; l > lmin; --l) {
    for (auto i = 0; i < listCell.size(); ++i) { 
      auto c = listCell[i]; 
      if (c->level[dir] != l) continue; 
      if (dir == 0 && !c->masterx[l]) continue; 
      if (dir == 1 && !c->mastery[l]) continue; 
      if (c->coarsen(dir) && !isAnyCoarsened) {
	isAnyCoarsened=true;
      } 
    }
  }
  return true; 
}

void refine() { 
  auto t = clock(); 
  bool isFinished = false; 
  auto maxLevel = 0; auto l = 0;  auto minLevel=0; 
  vector<int > levels; 
  cout << "Refine: "; 
  while (!isFinished) {
    isFinished = true;
    for (auto v : listVertex) {
      auto c1 = v->getCell(1); 
      auto c2 = v->getCell(2); 
      auto c3 = v->getCell(3); 
      if (c1 && c2 && c1!=c2) {
	l = (*c1)->adapt[0] + (*c1)->level[0]; if (levels.size() < l+1) {levels.resize(l+1); levels[l]=1;} else ++levels[l]; 
	l = (*c2)->adapt[0] + (*c2)->level[0]; if (levels.size() < l+1) {levels.resize(l+1); levels[l]=1;} else ++levels[l]; 
	maxLevel = max(maxLevel, (*c1)->adapt[0] + (*c1)->level[0]);
	maxLevel = max(maxLevel, (*c2)->adapt[0] + (*c2)->level[0]);
	if (((*c1)->adapt[0])+((*c1)->level[0])-((*c2)->adapt[0])-((*c2)->level[0]) > 1 ) {
	  (*c2)->adapt[0] = ((*c1)->level[0])+((*c1)->adapt[0])-((*c2)->level[0])-1; 
	  isFinished = false;
	} else if (((*c2)->adapt[0])+((*c2)->level[0])-((*c1)->adapt[0])-((*c1)->level[0]) > 1 ) {
	  (*c1)->adapt[0] = ((*c2)->level[0])+((*c2)->adapt[0])-((*c1)->level[0])-1;
	  isFinished = false; 
	}
      }
      if (c2 && c3 && c2!=c3) {
	l = (*c3)->adapt[1] + (*c3)->level[1]; if (levels.size() < l+1) {levels.resize(l+1); levels[l]=1;} else ++levels[l]; 
	l = (*c2)->adapt[1] + (*c2)->level[1]; if (levels.size() < l+1) {levels.resize(l+1); levels[l]=1;} else ++levels[l]; 
	maxLevel = max(maxLevel, (*c3)->adapt[1] + (*c3)->level[1]);
	maxLevel = max(maxLevel, (*c2)->adapt[1] + (*c2)->level[1]);
	if (((*c3)->adapt[1])+((*c3)->level[1])-((*c2)->adapt[1])-((*c2)->level[1]) > 1 ) {
	  (*c2)->adapt[1] = ((*c3)->level[1])+((*c3)->adapt[1])-((*c2)->level[1])-1;
	  isFinished = false; 
	} else if (((*c2)->adapt[1])+((*c2)->level[1])-((*c3)->adapt[1])-((*c3)->level[1]) > 1 ) {
	  (*c3)->adapt[1] = ((*c2)->level[1])+((*c2)->adapt[1])-((*c3)->level[1])-1;
	  isFinished = false; 
	}	
      }
    }
  }
  for (auto i=0; i<levels.size(); ++i) if (levels[i] > 0) { minLevel = i; break; }
  cout << " Levels: ("<< minLevel << "-" << maxLevel << ") : "; 

  isFinished = false; 
  auto nCell = 0; 
  while (!isFinished && listCell.size() != nCell) {
    vector<bool > visited(listCell.size(), false);
    nCell = listCell.size(); 
      
    isFinished = true; 
    for (auto l = minLevel; l < maxLevel+1; ++l) {
      int nlevel=0;      
      for (auto i = 0; i< nCell; ++i) {
	if (((*(cbegin()+i))->level[0] > l) || ((*(cbegin()+i))->level[1] > l)|| visited[i]) continue; 
	visited[i] = true; 
	if ((*(cbegin()+i))->refine()) isFinished = false;
      }
    }    
  }
  setCurrentLevels(); 
  makeFace(); 
  t = clock() - t;   
  
  cout << " Cells: " << listCell.size(); 
  cout << " Faces: " << nFace << " in " << ((float)t)/CLOCKS_PER_SEC << " secs."<< endl; 

}

VecX<Vec3> getError(shared_ptr<Var> const &a) {
  VecX<Vec3> err(a->data.size()); 
  VecX<double> sum(a->data.size()); 
  auto grad = valGrad(a); 
  for (auto j = 0; j < listFace.size(); ++j) {
    auto f = listFace[j]; 
    auto xf = f->getCoord(); auto area = f->vol(); 
    auto pf1 = f->phi(a).eval(a); 
    auto n = f->next; auto p = f->prev; 
    if (n >= 0) {
      auto xc = listCell[n]->getCoord(); 
      auto pf0 = a->data[n] + grad[n]*(xf - xc); 
      err[n] += abs(pf1 - pf0)*area;
      sum[n] += area.abs(); 
    }
    if (p >= 0) {
      auto xc = listCell[p]->getCoord(); 
      auto pf0 = a->data[p] + grad[p]*(xf - xc); 
      err[p] += abs(pf1 - pf0)*area;
      sum[p] += area.abs(); 
    } 	
  }   
  // //  double mean=0; 
  for (auto j = 0; j < sum.size(); ++j) {
    //  err[j] /= sum[j]; 
    (err[j]).data[0] = abs((err[j]).data[0])/sum[j];
    (err[j]).data[1] = abs((err[j]).data[1])/sum[j];
    (err[j]).data[2] = abs((err[j]).data[2])/sum[j];
  }
  //   // if (err[j][0] > mx) mx = err[j][0]; 
  //   // if (err[j][1] > mx) mx = err[j][1]; 
  //   // if (err[j][2] > mx) mx = err[j][2]; 
  // }
  // // if (mx > 0) { 
  // //   for (auto j = 0; j < sum.size(); ++j) {
  // //     err[j] /= mx; 
  // //   }    
  // // }
  // // cout << mx <<  endl;   
  return err; 
}


VecX<Vec3> getError2(shared_ptr<Var>  &a) {
  VecX<Vec3> err(a->data.size()); 
  auto grad = valGrad(a); 
  for (auto j = 0; j < listFace.size(); ++j) {
    auto f = listFace[j]; 
    auto xf = f->getCoord(); auto area = f->vol(); 
    //    auto pf1 = f->phi(a).eval(a); 
    auto pfl = listVertex[f->node[0]]->evalPhi(a); 
    auto pfu = listVertex[f->node[1]]->evalPhi(a);
    auto pf1 = 0.5*(pfl+pfu);
    auto m = abs(pfl - pfu); 
    if (m < 1e-6) continue; 
    auto n = f->next; auto p = f->prev; 
    auto orient = (f->orient + 1)%2;
    if (n >= 0) {
      auto xc = listCell[n]->getCoord();
      //      auto m = abs(pf1 - a->get(n)); 
      //      if (m < 1e-10) continue; 
      auto pf0 = a->data[n] + grad[n]*(xf - xc); 
      if (abs(pf1 - pf0) < 1e-6) continue; 
      err[n][orient] = max(err[n][orient], abs(pf1 - pf0)/m); //abs(pfu - pfl); //*area;
    }
    if (p >= 0) {
      auto xc = listCell[p]->getCoord(); 
      //      auto m = abs(pf1 - a->get(p)); 
      //if (m < 1e-10) continue; 
      auto pf0 = a->data[p] + grad[p]*(xf - xc); 
      if (abs(pf1 - pf0) < 1e-6) continue; 
      err[p][orient] = max(err[p][orient], abs(pf1 - pf0)/m); //abs(pfu - pfl); //*area;
    } 	

    // auto row = (f->next < 0) ? f->prev : f->next; 
    // auto xc = listCell[row]->getCoord(); 
    // auto pf0 = a->get(row) + grad[row]*(xf - xc); 
    // if (abs(pfu - pfl) < 1e-6) continue; 
    // err[j] += abs(pf1 - pf0)/abs(pfu - pfl); //*area;
  }
  //   // if (err[j][0] > mx) mx = err[j][0]; 
  //   // if (err[j][1] > mx) mx = err[j][1]; 
  //   // if (err[j][2] > mx) mx = err[j][2]; 
  // }
  // // if (mx > 0) { 
  // //   for (auto j = 0; j < sum.size(); ++j) {
  // //     err[j] /= mx; 
  // //   }    
  // // }
  // // cout << mx <<  endl;   
  return err; 
}

void adaptCriteria() { // For now it is a circle; 
  Vec3 x0 = Vec3(0.5, 0.5, 0); 
  double tol = 1./10.; 
  for (auto i = 0; i < 2 ; ++i) {
    for (auto v : listVertex) {
      if (abs((Vec3(*v)-x0).abs() - 0.2) <= tol) {
	auto c = (v)->getCell(0); 
	if (c && *c) {
	  ++((*c)->adapt[0]); 
	  ++((*c)->adapt[1]); 
	}
      }
    }
    tol = tol/2; 
    refine();       
  }
}

void cleanGrid() {
  vector<int_8> o2n_cell(listCell.size(), -1); int_8 icell = 0; 
  for (auto it = cbegin(); it != cend(); ) {
    if ((*it)->isAlive) {
      o2n_cell[(*it)->id] = icell++; 
      ++it; 
    } else { 
      if ((*it).use_count() > 1) {
	cout << "Cannot clear cell list: USED a lot "<<  endl; 
	exit(1); 
      }
      it = listCell.erase(it); 
    }	
  }
  vector<int_8> o2n_node(listVertex.size(), -1); int_8 inode = 0; 
  for (auto it = vbegin(); it != vend(); ) {
    (*it)->coefUpdate = true; 
    auto isConnected = false; 
    for (auto jt = 0; jt < (*it)->cell.size(); ++jt) {
      if ((*it)->cell[jt] >= 0) {isConnected = true; break;}
    }
    if (isConnected && (*it)->cell.size() == 4) { 
      if (((*it)->cell[0] == (*it)->cell[1] && (*it)->cell[2] == (*it)->cell[3]) ||
	  ((*it)->cell[0] == (*it)->cell[3] && (*it)->cell[1] == (*it)->cell[2])) {
	isConnected = false; 
      }
    }
    if (!isConnected) {
      if ((*it).use_count() > 1) {
	cout << " vertex count exceeds one "<< endl; 
	exit(1); 
      }
      it = listVertex.erase(it); 
    } else {      
      o2n_node[(*it)->id] = inode++;
      ++it; 
    }
  }

  for (auto j = 0; j < listCell.size(); ++j) {
    auto c = listCell[j]; 
    for (auto var : listVar) {
      if (var->loc == 0) var->data[j] = var->data[c->id]; 
    }
    c->id = j; 
    for (auto i = 0; i < c->node.size(); ++i) {
      if (c->node[i] >= 0) c->node[i] = o2n_node[c->node[i]]; 
    }
  }

  for (auto j = 0; j < listVertex.size(); ++j) {
    auto v = listVertex[j]; 
    for (auto var : listVar) {
      if (var->loc == 1) var->data[j] = var->data[v->id]; 
    }
    if (otherVertex.size() == o2n_node.size()) otherVertex[j] = otherVertex[v->id]; 
    v->id = j; 
    for (auto i = 0; i < v->cell.size(); ++i) {
      if (v->cell[i] >= 0) v->cell[i] = o2n_cell[v->cell[i]]; 
    } 
  }
  otherVertex.resize(listVertex.size(),-1); 
  for (auto v : listVar) { 
    if (v->loc == 0) v->data.resize(listCell.size()); 
  }

  o2n_cell.clear(); 
  o2n_node.clear(); 
}

void valAdapt(shared_ptr<Var> phi, double low, double high) {
  auto mid = 0.5*(low + high); 
  // for (auto j = 0; j < 3; ++j) {
  //   auto dl = (mid - low)/(levelHighBound[j] - levelLowBound[j]);
  //   for (auto i = 0; i < listCell.size(); ++i) {
  //     int_2 targetLevel = levelHighBound[j] - ((int)(abs(phi->get(i) - mid)/dl)); 
  //     if (listCell[i]->level[j] < targetLevel) {
  // 	listCell[i]->adapt[j] = min(1, levelHighBound[j] - listCell[i]->level[j]);
  //     } else if (listCell[i]->level[j] > targetLevel) {
  // 	listCell[i]->adapt[j] = max(-1, levelLowBound[j] - listCell[i]->level[j]);  
  //     }
  //     if (j==0) 
  // 	cout << levelHighBound[j] << " " << targetLevel << " " << listCell[i]->level[j] << " " << listCell[i]->adapt[j] << endl; 

  //   }
  //     if (abs(phi->get(i) - mid) < (k+1)*dl) {

  //     }
    
   for (auto i = 0; i < listCell.size(); ++i) {
    
     if (phi->get(i) > low && phi->get(i) < high) {
       for (auto j=0; j <3; ++j) 
	 listCell[i]->adapt[j] = min(1, levelHighBound[j] - listCell[i]->level[j]);
          } else {
       for (auto j=0; j <3; ++j) 
		listCell[i]->adapt[j] = max(-1, levelLowBound[j] - listCell[i]->level[j]);  
     }
  }
}


void geoAdapt(Grid* cGrid) {
  // auto adaptNeeded; 
  // for (auto i = 0; i < cGrid->otherVertex.size(); ++i) {
  //   if (cGrid->otherVertex[i] < 0) continue; 
  //   auto v1 = cGrid->listVertex[i]; 
  //   auto v1n = v1->ngbr(1); 
  //   if (n == nullptr) continue; 
  //   auto i1 = cGrid->otherVertex[i];
  //   auto i1n = cGrid->otherVertex[n->id]; 
  //   if (i1n < 0) continue; 
    

  //   auto v0 = listVertex[i1]; 
  //   auto n = v0->ngbr(1); 
  //   if (n == nullptr) continue; 
    
  //   for (auto j = 0; j < v0->cell.size(); ++j) {
  //     auto i0 = v0->cell[j]; 
  //     if (i0 < 0) continue; 
  //     listCell[i0]->adapt[0] += 1; 
  //     listCell[i0]->adapt[1] += 1;
  //   }
  // }
}


  
#endif
