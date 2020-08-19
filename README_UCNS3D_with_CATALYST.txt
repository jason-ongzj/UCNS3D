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

--------------------------------
// 2a. For non-canonical builds, for e.g. catalyst with rendering capabilities. 
//     Pipe the output to a file to track missing vtk libraries to be added
//     for use in UCNS3D.

cmake -DPARAVIEW_USE_PYTHON=ON \
	-DPARAVIEW_BUILD_EDITION="CATALYST_RENDERING" \
	-DPARAVIEW_USE_MPI=ON \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../paraview

cmake -DPARAVIEW_USE_PYTHON=OFF \
	-DPARAVIEW_BUILD_EDITION="CATALYST_RENDERING" \
	-DPARAVIEW_USE_MPI=ON \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../paraview

// 2b. Custom build on HPC. For Python build, just go with ParaView-5.8.0 module on HPC.

module load CMake/3.15.3-GCCcore-8.3.0
module load intel/2020a

cmake -DPARAVIEW_USE_PYTHON=OFF \
	-DPARAVIEW_BUILD_EDITION="CATALYST_RENDERING" \
	-DPARAVIEW_USE_MPI=ON \
	-DCMAKE_C_COMPILER="/apps/software/GCCcore/9.3.0/bin/gcc" \
	-DCMAKE_CXX_COMPILER="/apps/software/GCCcore/9.3.0/bin/c++" \
	-DCMAKE_Fortran_COMPILER="/apps/software/impi/2019.7.217-iccifort-2020.1.217/intel64/bin/mpiifort" \
	-DMPI_C_LINK_FLAGS:STRING="-Xlinker --enable-new-dtags -Xlinker -Wl,-rpath -Xlinker /apps/software/impi/2019.7.217-iccifort-2020.1.217/intel64/lib/release/libmpi.so" \
	-DMPI_C_COMPILER="/apps/software/impi/2019.7.217-iccifort-2020.1.217/intel64/bin/mpiicc" ../paraview

make -j 4 |& tee buildlog.txt

----------------------------------------------------------------
// 3a. Compile UCNS3D-Catalyst locally
// Specify Paraview and UCNS3D build directories

parentdir="$(dirname "$(pwd)")"
cmake -DPARAVIEW_BUILD="$HOME/Desktop/pv_5.8" \
	-DUCNS3D_ROOT="$parentdir/CODE" \
	-DCOMPILE_TARGET="LOCAL" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../
make

// 3b. For compilation of UCNS3D-Catalyst on Crescent, run

module load CMake/3.15.3-GCCcore-8.3.0
module load intel/2020a
module load ParaView/5.8.0-intel-2020a-Python-3.8.2-mpi
module load Python/3.8.2-GCCcore-9.3.0
export DISPLAY=hpcxserv-1.central.cranfield.ac.uk:0.0

parentdir="$(dirname "$(pwd)")"

// For compiling with individual shared object libraries 
// (Requires individual libraries to be specified in CMakeLists.txt in /CODE folder).
cmake -DUCNS3D_ROOT="$parentdir/CODE" \
	-DCOMPILE_TARGET="CRESCENT" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DCMAKE_Fortran_COMPILER="/apps/software/impi/2019.7.217-iccifort-2020.1.217/intel64/bin/mpiifort" ../

// For linking directly with ParaView library
cmake -DUCNS3D_ROOT="$parentdir/CODE" \
 -DCOMPILE_TARGET="CRESCENT" \
 -DCMAKE_BUILD_TYPE:STRING=Release \
 -DCMAKE_C_COMPILER="/apps/software/GCCcore/9.3.0/bin/gcc" \
 -DCMAKE_CXX_COMPILER="/apps/software/GCCcore/9.3.0/bin/c++" \
 -DCMAKE_Fortran_COMPILER="/apps/software/impi/2019.7.217-iccifort-2020.1.217/intel64/bin/mpiifort" \
 -DMPI_C_LINK_FLAGS:STRING="-Xlinker --enable-new-dtags -Xlinker -Wl,-rpath -Xlinker /apps/software/impi/2019.7.217-iccifort-2020.1.217/intel64/lib/release/libmpi.so" \
 -DMPI_C_COMPILER="/apps/software/impi/2019.7.217-iccifort-2020.1.217/intel64/bin/mpiicc" ../

// 3c. Compile UCNS3D with ParaView built on local Crescent node
module load CMake/3.15.3-GCCcore-8.3.0
module load intel/2020a
module load X11/20200222-GCCcore-9.3.0
export UCX_TLS=ud,sm,self
export DISPLAY=hpcxserv-1.central.cranfield.ac.uk:0.0

parentdir="$(dirname "$(pwd)")"
cmake -DPARAVIEW_BUILD="$HOME/pv_5.8_catalyst-rendering_nopy" \
	-DUCNS3D_ROOT="$parentdir/CODE" \
	-DCOMPILE_TARGET="CRESCENT" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DCMAKE_Fortran_COMPILER="/apps/software/impi/2019.7.217-iccifort-2020.1.217/intel64/bin/mpiifort" \
	-DMPI_C_LINK_FLAGS:STRING="-Xlinker --enable-new-dtags -Xlinker -Wl,-rpath -Xlinker /apps/software/impi/2019.7.217-iccifort-2020.1.217/intel64/lib/release/libmpi.so" \
	-DMPI_C_COMPILER="/apps/software/impi/2019.7.217-iccifort-2020.1.217/intel64/bin/mpiicc" ../

----------------------------------------------------------------
// 4a. For testing of UCNS3D-Catalyst locally
export OMP_NUM_THREADS=1
cd CODE
mkdir RUN && cd RUN
cp ../../../Catalyst_Example/UCNS3D_obj/* .
cp ../ucns3d_p .
mpirun -np 4 ./ucns3d_p

// 4b. For testing on Crescent
// Inside pbs script, put
module load intel/2020a
module load ParaView/5.8.0-intel-2020a-Python-3.8.2-mpi
export DISPLAY=hpcxserv-1.central.cranfield.ac.uk:0.0
export OMP_NUM_THREADS=1

mpirun -np 4 ./ucns3d_p |& tee log.txt

// Then run
qsub mpi.sub
