#
# Copyright (c) 2016 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(i2c_bmp581)

target_include_directories(app PRIVATE lib)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/bmp5.c)
target_sources(app PRIVATE src/bt-periph.c)
target_sources(app PRIVATE src/common.c)