@echo off

scons --clean
scons --clean dev_build=yes
scons --clean library_type=shared_library
scons --clean library_type=shared_library dev_build=yes