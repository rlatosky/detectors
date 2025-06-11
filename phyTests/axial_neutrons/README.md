# Overview

This is a test of Neutrons dispersion after two slabs of scintillator material. 
This will be developed to match the STAR Barrel EM Calorimeter.
(referenced in: Nuclear Instruments and Methods in Physics Research A 499 (2003) 725–739)

## Setup Containers
VNC gemc viewer: 
```
podman run --platform linux/amd64 -it --rm -v ~/detectors/phyTests/axial_neutrons:/usr/local/mywork:z -p 8080:8080 jeffersonlab/gemc:dev-fedora36
```
Terminal gemc:
```
podman run --platform linux/amd64 -it --rm -v ~/detectors/phyTests/axial_neutrons:/usr/local/mywork:z jeffersonlab/gemc:dev-fedora36 bash
```

## Geometry

Two scintillator slabs, 3x3 meters, each followed by a 1mm thick flux detector.

To create: 

```
./ax_n.pl config.dat
```


## Run gemc - change number of events accordingly

```
gemc ax_n.gcard -USE_GUI=0 -N=10000 -PRINT_EVENT=1000
```

## Analysis


```
module use /cvmfs/oasis.opensciencegrid.org/jlab/hallb/clas12/sw/modulefiles
module load coatjava hipo
```

Filter only MC::True

```
hipo-utils -filter -b "MC::True" -e "MC::True" -o data.hipo test.hipo
```

Compile and run the code:

```
mkdir build
cd build
cmake ..
make -j10

 ./neutrons ../data.hipo 

```


