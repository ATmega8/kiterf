if [ ! -d "build" ]; then
  mkdir build && cd build
  cmake .. -DPROJ=kiterf -DTOOLCHAIN=$K210_TOOLCHAIN_PATH -DSDK=$K210_SDK_PATH 
  make -j
else
  cd build
  make -j
fi
cd ..
