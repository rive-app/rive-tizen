#TODO: prepare integrated build script.

#rive-cpp must have original code repo.
#thorvg must have thorvg backend code (instead of skia)

#check feasibility: 

#option 1
#1. rive-tizen includes rive-cpp code and manual sync.
#2. add thorvg into rive-cpp
#3. rewrite meson build script for whole sub directories.
#4. regularly rive-cpp code sync. 


#option 2 
#1. up-to-date rive-cpp (git@github.com:rive-app/rive-cpp.git)
#1.1 check rive-cpp sub directory then git clone or git pull?
#2 build rive-cpp base except skia. (using original script?)
#2.1 if then, how to build in gbs?
#3. build thorvg using meson.


