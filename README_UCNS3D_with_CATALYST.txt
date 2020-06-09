BUILD STEPS FOR PARAVIEW AND CATALYST(Tested on fresh install of Ubuntu 18.04)
----------------------------------------------------------------------------------------------------
// Get CMake versions 3.12 - 3.16 for Master Build 

// 1. Install dependencies
// Ensure MPIRUN is not wrapped with OpenMPI (for testing on local machine)
sudo apt remove libopenmpi-dev openmpi-bin openmpi-common

// Get mpich package and GNU compilers
sudo apt install gcc g++ gfortran libmpich-dev

// To check mpirun is not used with OpenMPI, enter:
mpirun --version

sudo yum install python3-devel mesa-libGL-devel libX11-devel libXt-devel qt5-qtbase-devel qt5-qtx11extras-devel qt5-qttools-devel qt5-qtxmlpatterns-devel tbb-devel git

// Do the following steps before compilation of paraview source
cd Desktop/
git clone https://gitlab.kitware.com/paraview/paraview.git
cd paraview

// Checkout to specific version that you need
git checkout v5.8.0
git submodule update --init --recursive

// If there is no Make installed
sudo apt install make

// Install CMake version 3.15.4 (change the cmake version as you deem fit)
version=3.15
build=4
mkdir ~/temp
cd ~/temp
wget https://cmake.prg/files/v$version/cmake-$version.$build.tar.gz
cd cmake-$version.$build/
./bootstrap
make -j2
sudo make install

// Install openblas package
sudo apt install libopenblas-dev

// 2. Paraview build compilation using GCC 7.5.0 with Ubuntu 18.04. The following 
//    steps assume Paraview is built in Desktop/pv_5.8/

cd && cd Desktop/
mkdir pv_5.8 && cd pv_5.8
cmake -DPARAVIEW_USE_PYTHON=ON \
	-DPARAVIEW_USE_MPI=ON \
	-DPARAVIEW_USE_FORTRAN=ON \
	-DPARAVIEW_BUILD_CATALYST_ADAPTORS=ON \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" \
	-DCMAKE_C_COMPILER="/usr/bin/cc" \
	-DMPI_C_COMPILER="/usr/bin/mpicc" ../paraview

make -j 4

--------------------------------
// 2a. For non-canonical builds, for e.g. catalyst with rendering capabilities. 
//     Pipe the output to a file to track missing vtk libraries to be added
//     for use in UCNS3D.

cmake -DPARAVIEW_USE_PYTHON=ON \
	-DPARAVIEW_BUILD_SHARED_LIBS=OFF \
	-DPARAVIEW_BUILD_EDITION="CATALYST_RENDERING" \
	-DPARAVIEW_USE_MPI=ON \
	-DPARAVIEW_USE_FORTRAN=ON \
	-DPARAVIEW_BUILD_CATALYST_ADAPTORS=ON \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" \
	-DCMAKE_C_COMPILER="/usr/bin/cc" \
	-DMPI_C_COMPILER="/usr/bin/mpicc" ../paraview

make -j 4 |& tee buildlog.txt

----------------------------------------------------------------
// 3. Compile UCNS3D with Catalyst
// Specify Paraview and UCNS3D build directories
cmake -DCMAKE_PREFIX_PATH="<Your_Paraview_Build_Dir>" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DUCNS3D_ROOT="<Your_UCNS3D_Code_Dir>" \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../

// Example
cmake -DCMAKE_PREFIX_PATH="/home/jason/Desktop/pv_5.8/" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DUCNS3D_ROOT="/home/jason/Desktop/UCNS3D/CODE" \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../

make

// 4. For testing of UCNS3D
export OMP_NUM_THREADS=1
cd CODE
mkdir RUN && cd RUN
cp ../../../Catalyst_Example/UCNS3D_obj/* .
cp ../ucns3d_p .
mpirun -np 4 ./ucns3d_p

--------------------------------------------------------------------------
INTEGRATION STEPS:

- Generate Python code required for visualization, which can be done using the
  "Export Catalyst Script" function under "Catalyst" tab in Paraview.
- Generate C++ code that interfaces with the vtk API.
- Generate the Fortran code that serves as the adaptor for the C code.
- Code adjustments within UCNS3D to accomodate for insitu-visualization
  -> Mesh decomposition by different processes, no combining at the end of each
     time step to reduce I/O bandwidth taken
  -> New subroutine that is similar to OutWritePara3DbP without writing to .vtk
     file format, only for updating the scalar data arrays.

NEXT STEPS:
- Static integration of Paraview-Catalyst with UCNS3D to avoid compilation on HPC.
- Attempt communication of UCNS3D-Catalyst on HPC with local machine.
- Explore data visualization using isosurfaces or volume data.


