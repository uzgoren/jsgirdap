#girdap

Please refer to https://uzgoren.github.io/girdap for more information.

<!-- This project's aim is to develop object-oriented c++ libraries for solving conservation equations on a self-managed grid. 

###DISCLAIMER
This project is still in a early phase, expect to see bugs.

###REQUIREMENTs
C++ compiler that accepts C++11 standards 

###COMPILE
use "mk" script (tested on Linux and Mac) 
-->
###FAQ 
####_What is girdap?_
_girdap_ provides building blocks for numerical simulations of complex transport equations. Tools are built around a grid management platform connected to finite volume method based differential operators. girdap’s primary goals are (1) _flexibility_ and (2) _accuracy_. 

Flexible tools allow <b>researchers</b> to develop new numerical algorithms and <b>educators</b> to teach students existing algorithms. Tools provided aim to shift programming focus more on physics and numerical method to avoid time consuming programming details. 

The accuracy is handled by automated grid refinement and coarsening based on the solution field. 

_girdap_ does not target audiences who would like to get immediate results for an engineering project. It involves a numerical algorithm development phase. Those who would like to skip such a development can refer to a commercial CFD/multiphysics software. 

####_Isn’t this done before?_ 
Similar projects do exist. <a href=http://www.openfoam.org/>OpenFOAM</a> is one example; and another one is <a href=http:fenicsproject.org>FEniCS</a>. Please check them out. They are really good projects. girdap just offers another flavor. 

####_What does girdap mean?_
_girdap_ is not an acronym. It means whirlpool in Turkish. 

####_What operators are included?_
Time derivative, divergence, gradient, Laplacian, and source terms can be defined for a differential equation. 

####_How can I install/use girdap?_
You can retrieve the source code (written in c++ following c++11 standards) at github. You can see examples of main_*.cpp files under src directory so that you can compile and use it in the way you like. Use the script provided in the main directory to compile. Note that our focus is mainly on the development of girdap’s skeleton rather than its transportability so you may experience problems while compiling your first application. This is of course going to change with its first release. 

####_Can you provide an example?_
A typical numerical simulation involve the following steps; <ol> 
<li>Create a new grid
<li>Declare your own field variables (defined on cell centers)
<li>State initial and boundary conditions for each variable
<li>Start a time loop <ol>
<li>Solve for var_1 governed by a partial differential equation
<li>Solve for var_2 governed by algebraic equations
<li>Grid adaptation/movement
<li>Write an output to screen/file</ol>
<li>Exit</ol>

_girdap_ provides tools for each step above. In addition, you can have as many grids and as many variables together governed by different set of equations; and interacting with each other. Such a flexibility allows developing various algorithms for fluid-structure interactions and multiphase flows. 

Furthermore, through a driver code (as main.cpp); one can add a custom source term or decide on how to linearize non-linear equations; or how to manage inner and outer loops; how to handle adaptation and many more. It is also possible to link 1D/2D domain with a 3D domain using interpolators between grids. Cell movements can be controlled as well. 

Boundary conditions can be defined as a linear function for Dirichlet and Neumann type conditions. This flexibility allows convective heat transfer coefficient to be specified at the boundaries. 

Initial conditions can be defined easily through cell iterators on grid. 

So a thin long rod interacting with 3D incompressible flow can be declared as follows:
<ol><li>	Create grid1 and grid2
<li>Declare velocity, pressure and temperature for grid1 (3D);  temperature for grid2 (1D)
<li>State initial and boundary conditions
<li>Start a time loop<ol>
<li>Link temperature field of grid1 to grid2 as a boundary condition
<li>Solve heat diffusion equation on grid 2
<li>Adapt grid2 based on its temperature field
<li>Solve flow velocity on grid1
<li>Adapt grid1 based on its velocity field
<li>Solve pressure equation on grid1
<li>Correct flow velocity on grid1
<li>Link temperature field of grid2 to grid1 as a dummy variable
<li>Solve energy equation with a source term (dummy var) on grid1
<li>Write an output to screen/file</ol>
<li>Exit</ol>

For those who are familiar, this is quite similar to scripting in Matlab. 

####_What is grid adaptation?_
Grid adaptation is used in problem with multiple length scales; so that computational time is used wisely. Regions with less interaction handled with coarse resolution; and regions influence the field are handled with fine resolution. So automated grid handling is the key for accuracy and speed. 

####_How is grid adaptation handled?_
It relies on splitting a cell into two and merging two neighboring cells. This leads to anisotropic grid refinement. Grid refinement is based on error estimation (before it is available). It is widely reported that indicators based on gradients do not produce quality mesh; and hence we use other indicators for refinement and coarsening. 

At the moment, grid focuses on quad cells. While underlying structure for hexa, tri and tetra cells are available, automated adaptive grid refinement and coarsening is available for quad cells. These overall form the envelope of girdap’s skeleton. 

####_What about parallel computing, MPI, OpenMP?_
It is not yet considered; but it is within our future plans. 

####ACKNOWLEDGEMENT
This work is part of a research project supported by a Marie Curie International Reintegration Grant within the 7th European Community Framework Programme.

<!--
#PRIORITIES:
#1. Anisotropic mesh refinement for Quad/Hexa cells. 
#2. Solution based adaptation criteria. 
#3. 2nd order (or higher) Laplace operator; 
#4. 2nd order (or higher) Divergence operator;
#5. Euler and explicit RK2/RK4 time integration 
#6. Fractional step method for transient incompressible flows
 
#SECONDARY
#1. Use of laplace operator to generate body-fitted grids. 
-->

