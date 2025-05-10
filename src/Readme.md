### Build VTK
```
D:\VTK\src> cmake-gui.exe
D:\VTK\build> ninja
```

### Build Qt
```
git clone https://github.com/qt/qt5.git  
git checkout v6.2.0  
perl.exe .\init-repository  
mkdir build  
D:/qt5/build> ../configure -prefix %CD%\qtbase -developer-build -opensource -nomake examples -nomake tests

../configure -prefix %CD%\qtbase  -debug
cmake --build . --parallel
```
