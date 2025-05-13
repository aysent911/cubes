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

## User manual
1. To show cursor press `ESC` key.
2. To toggle view, use number keys `1,2,3,4,5,6,7,8,9`.
3. To zoom in/out, scroll `up/down`
4. To rotate view in any axis, use cursor.



