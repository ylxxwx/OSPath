#!/bin/bash

pushd ..
make kernel.img
popd

cp ../kernel.img ./kernel
