[![Build](https://github.com/rive-app/rive-tizen/actions/workflows/build.yml/badge.svg)](https://github.com/rive-app/rive-tizen/actions/workflows/build.yml)

# Rive-Tizen

## Build
### Prerequiste
Rive-Tizen requires [ThorVG](https://github.com/Samsung/thorvg) to render Rive graphical units.
Please visit [ThorVG](https://github.com/Samsung/thorvg) and install that library by its guidance.

### Prepare Sub Modules
Rive-Tizen extends [rive-cpp](https://github.com/rive-app/rive-cpp)  project for exchanging backend rendering engine to Tizen specific.
Thus, you can immediately clone the rive-cpp project and build it on Rive-Tizen repo.
```
git submodule update --init --recursive
```

### Build Rive-Tizen
Basically, Rive-Tizen supports [meson](https://mesonbuild.com/) build system.

Install [meson](http://mesonbuild.com/Getting-meson.html) and [ninja](https://ninja-build.org/) if not installed yet.

Run meson to configure Rive-Tizen.
```
meson build
```
Run ninja to build & install Rive-Tizen
```
ninja -C build install
```
