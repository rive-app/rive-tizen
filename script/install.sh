#!/bin/sh

cd "submodule"

patch -p0 < ../script/tizen_build.diff

