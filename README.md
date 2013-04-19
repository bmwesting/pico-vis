pico-vis
========

Pico-Vis is a portable system that interfaces cloud-based visualization to wearable computing through a rich sensor, miniature computer, and computer vision algorithms.

## Installation

Build steps using CMake:

Get [TUIO](tuio.org). Get [OpenNI-2+](http://www.openni.org/openni-sdk/).

```
mkdir build
cd build
cp -r ~/TUIO_CPP/oscpack .
cp -r ~/TUIO_CPP/TUIO .
cp -r ~/OpenNI-2.x.x/Redist/* .
make
```
