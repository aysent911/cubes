# cubes
OpenGL C++ demo project  
An OpenGL project to demo textures, translation, camera and mouse input  
## Installation
```
  git clone https://github.com/aysent911/cubes.git
  cd cubes
```
### Debian
#### Option 1
Install prebuilt package
```
  sudo dpkg -i cubes_0.0_amd64.deb
  cubes
```
#### Option 2
Build from source
```
  cmake -S . -B build
  cd build
  make
  cp cubes ../usr/bin/
  cd ../..
  dpkg-deb --build cubes
  sudo dpkg -i cubes.deb
  cubes
```


