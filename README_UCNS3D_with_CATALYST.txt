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

sudo apt-get install git libgl1-mesa-dev libxt-dev qt5-default libqt5x11extras5-dev libqt5help5 qttools5-dev qtxmlpatterns5-dev-tools libqt5svg5-dev python3-dev libtbb-dev

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
wget https://cmake.org/files/v$version/cmake-$version.$build.tar.gz
tar -xzvf cmake-$version.$build.tar.gz
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

// Without Python
cmake -DPARAVIEW_USE_PYTHON=OFF \
	-DPARAVIEW_USE_MPI=ON \
	-DPARAVIEW_USE_FORTRAN=ON \
	-DPARAVIEW_BUILD_CATALYST_ADAPTORS=ON \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" \
	-DCMAKE_C_COMPILER="/usr/bin/cc" \
	-DMPI_C_COMPILER="/usr/bin/mpicc" ../paraview

-------------------------------------------------
// For building on HPC

cmake -DPARAVIEW_USE_PYTHON=ON \
	-DPARAVIEW_USE_MPI=ON \
        -DPARAVIEW_USE_QT=OFF \
	-DPARAVIEW_USE_FORTRAN=ON \
	-DPARAVIEW_BUILD_CATALYST_ADAPTORS=ON \
	-DPARAVIEW_BUILD_QT_GUI=OFF \
  	-DVTK_USE_X=OFF \
  	-DOPENGL_INCLUDE_DIR=IGNORE \
  	-DOPENGL_xmesa_INCLUDE_DIR=IGNORE \
  	-DOPENGL_gl_LIBRARY=IGNORE \
  	-DVTK_OPENGL_HAS_OSMESA=ON \
  	-DVTK_USE_OFFSCREEN=OFF \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" \
	-DCMAKE_C_COMPILER="/usr/bin/cc" \
	-DMPI_C_COMPILER="/usr/bin/mpicc" ../paraview

// For testing
cmake -DPARAVIEW_USE_PYTHON=ON \
	-DPARAVIEW_USE_MPI=ON \
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
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" \
	-DCMAKE_C_COMPILER="/usr/bin/cc" \
	-DMPI_C_COMPILER="/usr/bin/mpicc" ../paraview

cmake -DPARAVIEW_USE_PYTHON=ON \
	-DPARAVIEW_BUILD_SHARED_LIBS=OFF \
	-DPARAVIEW_BUILD_EDITION="CATALYST_RENDERING" \
	-DPARAVIEW_USE_MPI=ON \
	-DPARAVIEW_USE_FORTRAN=ON \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../paraview

cmake -DPARAVIEW_USE_PYTHON=ON \
	-DPARAVIEW_BUILD_EDITION="CATALYST_RENDERING" \
	-DPARAVIEW_USE_MPI=ON \
	-DPARAVIEW_USE_FORTRAN=ON \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../paraview

cmake -DPARAVIEW_USE_PYTHON=OFF \
	-DPARAVIEW_BUILD_EDITION="CATALYST_RENDERING" \
	-DPARAVIEW_USE_MPI=ON \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../paraview

cmake -DPARAVIEW_USE_PYTHON=ON \
	-DPARAVIEW_BUILD_EDITION="CATALYST" \
	-DPARAVIEW_USE_MPI=ON \
	-DPARAVIEW_USE_FORTRAN=ON \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../paraview

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

// Original (Use this for reference)
cmake -DPARAVIEW_BUILD="$HOME/Desktop/pv_5.8" \
	-DUCNS3D_ROOT="/home/jason/Desktop/UCNS3D/CODE" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../

// With manual dynamic compilation
cmake -DPARAVIEW_BUILD="$HOME/Desktop/pv_5.8" \
	-DUCNS3D_ROOT="/home/jason/Desktop/UCNS3D/CODE" \
	-DCOMPILE_TARGET="LOCAL_SIMPLE" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DPARAVIEW_SOURCE="/home/jason/Desktop/paraview" \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../

// Release
parentdir="$(dirname "$(pwd)")"
cmake -DPARAVIEW_BUILD="$HOME/Desktop/pv_5.8" \
	-DUCNS3D_ROOT="$parentdir/CODE" \
	-DCOMPILE_TARGET="LOCAL" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../

cmake -DPARAVIEW_BUILD="$HOME/Desktop/pv_5.8" \
	-DUCNS3D_ROOT="/home/jason/Desktop/UCNS3D/CODE" \
	-DCOMPILE_TARGET="LOCAL" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../

cmake -DPARAVIEW_BUILD="$HOME/Desktop/pv_5.8_catalyst-rendering_nopy" \
	-DUSE_PYTHON:BOOL=FALSE \
	-DUCNS3D_ROOT="/home/jason/Desktop/UCNS3D/CODE" \
	-DCOMPILE_TARGET="LOCAL" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../

cmake -DPARAVIEW_BUILD="$HOME/Desktop/pv_5.8_catalyst-rendering" \
	-DUCNS3D_ROOT="/home/jason/Desktop/UCNS3D/CODE" \
	-DCOMPILE_TARGET="LOCAL" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../

cmake -DPARAVIEW_BUILD="$HOME/Desktop/pv_5.8_catalyst-rendering-static" \
	-DUCNS3D_ROOT="/home/jason/Desktop/UCNS3D/CODE" \
	-DCOMPILE_TARGET="LOCAL" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../

// Debug
cmake -DPARAVIEW_BUILD="$HOME/Desktop/pv_5.8" \
	-DUCNS3D_ROOT="/home/jason/Desktop/UCNS3D/CODE" \
	-DCOMPILE_TARGET="LOCAL" \
	-DCMAKE_BUILD_TYPE:STRING=Debug \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../

make

// 4. For testing of UCNS3D locally
export OMP_NUM_THREADS=1
cd CODE
mkdir RUN && cd RUN
cp ../../../Catalyst_Example/UCNS3D_obj/* .
cp ../ucns3d_p .
mpirun -np 4 ./ucns3d_p

// 4a. For testing on Crescent
// Inside pbs script, put
module load intel/2020a
module load ParaView/5.8.0-intel-2020a-Python-3.8.2-mpi
export DISPLAY=hpcxserv-1.central.cranfield.ac.uk:0.0
export OMP_NUM_THREADS=1

mpirun -np 4 ./ucns3d_p |& tee log.txt

// Then run
qsub mpi.sub

// 5. For compilation of UCNS3D-Catalyst on Crescent, run

module load intel/2020a
module load CMake/3.15.3-GCCcore-8.3.0
module load ParaView/5.8.0-intel-2020a-Python-3.8.2-mpi
module load Python/3.8.2-GCCcore-9.3.0
export DISPLAY=hpcxserv-1.central.cranfield.ac.uk:0.0

cmake -DUCNS3D_ROOT="$HOME/UCNS3D/CODE" \
   -DCOMPILE_TARGET="CRESCENT" \
   -DCMAKE_BUILD_TYPE:STRING=Release \
   -DCMAKE_Fortran_COMPILER="/apps/software/impi/2019.7.217-iccifort-2020.1.217/intel64/bin/mpiifort" ../

// 6. For visualization of UCNS3D-Catalyst from Paraview, run a vis terminal
module spider binutils/2.27-GCCcore-6.3.0
module load binutils/2.27-GCCcore-6.3.0
module load libxcb/1.11.1-intel-2017a
export DISPLAY=hpcxserv-1.central.cranfield.ac.uk:0.0
vglrun paraview

// 7. For testing with Catalyst examples in Paraview directory,
cmake -DCMAKE_PREFIX_PATH="/home/jason/Desktop/pv_5.8/" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DBUILD_FORTRAN_EXAMPLES:STRING=ON \
	-DBUILD_TESTING:BOOL=ON \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../

cmake -DCMAKE_PREFIX_PATH="/home/jason/Desktop/pv_5.8_catalyst-rendering/" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DBUILD_FORTRAN_EXAMPLES:STRING=ON \
	-DBUILD_TESTING:BOOL=ON \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../

cmake -DCMAKE_PREFIX_PATH="/home/jason/Desktop/pv_5.8_catalyst-rendering-static/" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DBUILD_FORTRAN_EXAMPLES:STRING=ON \
	-DBUILD_TESTING:BOOL=ON \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../

cmake -DCMAKE_PREFIX_PATH="/home/jason/Desktop/pv_5.8_catalyst/" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DBUILD_TESTING:BOOL=ON \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../

cmake -DCMAKE_PREFIX_PATH="/home/jason/Desktop/pv_5.8_catalyst-rendering_nopy/" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DBUILD_TESTING:BOOL=ON \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../



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

- Use a Catalyst build without Python, integrate UCNS3D with Catalyst using only
  C++

