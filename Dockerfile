# FROM ubuntu:bionic AS base
FROM nvidia/cudagl:10.2-base-ubuntu18.04 AS base

ENV VERSION=3.15
ENV BUILD=4 
WORKDIR /home
RUN apt-get update

RUN apt-get remove libopenmpi-dev openmpi-bin openmpi-common

RUN apt-get install -y git build-essential libgl1-mesa-dev libxt-dev qt5-default libqt5x11extras5-dev libqt5help5 qttools5-dev qtxmlpatterns5-dev-tools libqt5svg5-dev python3-dev libtbb-dev libopenblas-dev make wget libssl-dev mesa-utils mpich

RUN mkdir ~/temp && cd ~/temp

RUN wget https://cmake.org/files/v$VERSION/cmake-$VERSION.$BUILD.tar.gz
RUN tar -xzvf cmake-$VERSION.$BUILD.tar.gz

RUN cd cmake-$VERSION.$BUILD/ && ./bootstrap && make -j4 && make install

FROM base AS pvsource

RUN git clone https://gitlab.kitware.com/paraview/paraview.git
RUN cd paraview && git checkout v5.8.0 && git submodule update --init --recursive

FROM pvsource AS pvbuild
RUN mkdir /home/pv_5.8 && cd /home/pv_5.8 && \
    cmake -DPARAVIEW_USE_PYTHON=ON \
	-DPARAVIEW_USE_MPI=ON \
	-DPARAVIEW_USE_FORTRAN=ON \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" \
	-DCMAKE_C_COMPILER="/usr/bin/cc" \
	-DMPI_C_COMPILER="/usr/bin/mpicc" ../paraview && \
    make -j4

RUN git clone https://github.com/jason-ongzj/UCNS3D.git
RUN cd UCNS3D && git checkout Catalyst_nopy && \
    mkdir build && cd build && \
    cmake -DPARAVIEW_BUILD="/home/pv_5.8" \
	-DUCNS3D_ROOT="/home/UCNS3D/CODE" \
	-DCOMPILE_TARGET="LOCAL" \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DCMAKE_Fortran_COMPILER="/usr/bin/mpifort" ../ && \
    make

RUN cd /home/UCNS3D/build/CODE && mkdir RUN && cd RUN && cp ../ucns3d_p . && \
    cp ../../../Catalyst_Test_Case/TGV/* . 

