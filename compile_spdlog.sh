#!/usr/bin/env bash
cd spdlog
cmake -H. -B_builds -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=Release
cmake --build _builds --config Release
cmake --build _builds --target install
