#!/bin/bash

source /opt/ros/noetic/setup.bash

echo "Will install required GIT submodules"
git submodule update --init --recursive || { echo "Failure"; exit;}

echo "Will compile MPC solver into ROS module"
cd src
python3 blimps_ros.py || { echo "Failure"; exit;}
cd ..

echo "Will compile ROS packages"
mkdir catkin_ws
mkdir catkin_ws/src
cd catkin_ws/src
catkin_init_workspace
ln -s ../../submodules
ln -s ../../src
cd ..
touch src/submodules/AirCap/packages/optional/basler_image_capture/CATKIN_IGNORE
touch src/submodules/AirCap/packages/optional/ptgrey_image_capture/CATKIN_IGNORE
catkin_make -j2 || { echo "Failure"; exit;}
source devel/setup.bash
cd ..

echo "Will compile Librepilot Firmware for SITL simulation"
cd submodules/AirCap/packages/3rdparty/airship_simulation/LibrePilot/
make simposix || { echo "Failure"; exit;}
cd ../../../../../..

echo -e "\n\n Success!"
echo "You can run the GAZEBO simulation with the following commands:"
echo "  cd experiments/sim"
echo "  ./airship_sim.sh 3"
echo "You can ithen stop the simulation with the command"
echo "  ./cleanup.sh"