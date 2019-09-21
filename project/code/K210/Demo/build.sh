if [ ! -f "build/Makefile" ]; then
  if [ -d "build" ]; then
    rm -r build
  fi
  mkdir build && cd build
  cmake .. -DPROJ=kiterf -DTOOLCHAIN=$K210_TOOLCHAIN_PATH -DSDK=$K210_SDK_PATH 
  make -j
else
  cd build
  make -j
fi
if [ $? -ne 0 ]; then
    echo "build failed"
    echo "Please try to delete build dir and execute this script again!"
    cd ..
    exit 1
else
    echo "build succeed"
    cd ..
    exit 0
fi

