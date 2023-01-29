# echo "Configuring and building ORB_SLAM3 ..."

# mkdir build
# cd build
# #cmake .. -DCMAKE_BUILD_TYPE=Release
# cmake .. -DCMAKE_BUILD_TYPE=Debug
# make -j4


echo "Configuring and building ORB_SLAM3 ..."

echo ${workspaceFolder}

rm -rf build
mkdir build
cd build
#cmake .. -DCMAKE_BUILD_TYPE=Release
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
