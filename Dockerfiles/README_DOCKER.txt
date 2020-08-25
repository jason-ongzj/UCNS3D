Building UCNS3D in Docker 

Note: Tested on Ubuntu Bionic 18.04, input commands may differ on other platforms. If you have an Nvidia GPU on your 
local machine, use Dockerfile_Nvidia. Otherwise, use Dockerfile_Mesa.

1. Install Docker, refer to this website: https://docs.docker.com/engine/install/

2. To build Docker image for UCNS3D, do the following in terminal:

   docker build -t ucns3d:mesa_build -f Dockerfile_mesa .

   If Docker is not setup to be used by a non-root user, add sudo to the front of each docker command.

3. To create a Docker container for the image, run:

   xhost +
   docker run --name ucns3d -v="/tmp/.X11-unix:/tmp/.X11-unix:rw" -e "DISPLAY=unix:0.0" -it \
	--privileged --device /dev/dri ucns3d:mesa_build

4. Within the Docker container, navigate to /home/UCNS3D/build/CODE/RUN and type in terminal to run   
   the TGV simulation:

   mpirun -np 4 ./ucns3d_p

5. For copying output files from container to host, do the following:
   
   docker cp ucns3d:/home/UCNS3D/build/CODE/RUN <destination_dir> 

   where <destination_dir> is the directory to be specified for copying the files into.










