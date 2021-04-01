#!/bin/sh

cd "submodule"

patch -p0 < ../script/tizen_build.diff
patch -p1 < ../script/rive_api_export.diff
