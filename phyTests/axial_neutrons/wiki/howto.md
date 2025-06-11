## Set up Simulation

### Setting up your development env
1. Clone this repository
1. SSH into the ifarm via this command:
`ssh -J [user]@scilogin.jlab.org [user]@ifarm.jlab.org`
2. Compile root into your home directory (follow [this guide](https://root.cern/install/#download-a-pre-compiled-binary-distribution))
3. Then type in:
```
$ zsh

module use /cvmfs/oasis.opensciencegrid.org/jlab/geant4/modules
module load gemc/dev

module use /cvmfs/oasis.opensciencegrid.org/jlab/hallb/clas12/sw/modulefiles
module load coatjava hipo
```
4. Clone this repository:
```
git clone https://github.com/rlatosky/detectors/
```
### Setting up the Geometry
1. Change the geometry outlined in file "geometry.pl" located in the "axial_neutrons" folder
2. Run `./ax_n.pl config.dat` while in the "axial_neutrons" folder

### Verifying geometry (**Optional**)
1. Mount the axial_neutrons directory on your personal machine using SSHFS (should work on Windows/Mac/Linux):
```
sshfs latosky@ifarm.jlab.org:/home/latosky/detectors/phyTests/axial_neutrons /media/rylen/ifarm/
```
#### Using Personal Machine
1. Clone this repository on your personal machine
2. Download the files `ax_n__geometry_default.txt` and `ax_n__materials_default.txt` from the ifarm onto your personal machine and insure they are inside the axial_neutrons folder.
2. On your personal system, run:
```
docker run --platform linux/amd64 -it --rm -v /[folder of ax_n files]:/usr/local/mywork:z -p 8080:8080 jeffersonlab/gemc:dev-fedora36
```
**This involves docker - make sure you install it here: [Download Docker Desktop](https://www.docker.com/)**
3. Go to web address the container prints out
4. In the terminal window inside the VNC connection, run:
```
module switch gemc/dev
gemc ./neutron_gcard/ax_n.gcard -CHECK_OVERLAPS=1 -USE_GUI=2 -OUTPUT="txt, out.txt"
```
5. View the graphical window and verify geometry is what you want.

### Setting up the analysis code
1. Run the provided `analysis.sh` inside any folder with part of the name as "_gcard"
2. This will run all simulations specified in .gcard files
3. Additionally, you can follow this command to run a specific gcard file:
```
gemc ax_n.gcard -USE_GUI=0 -N=10000 -PRINT_EVENT=1000
```
4. After the analysis is complete, you should see a `.hipo` file created
5. Here, use the `export-to-rootfile.sh` script in the `axial_neutrons` folder to analyze the `.hipo` and produce a `.root` file. Example usage:
```
./export-to-rootfile.sh ./[].hipo [particle] [Kinetic MeV] [NumOfHistBins/MaxNum]
```
6. Use ROOT's online viewer located [here](https://root.cern.ch/js/latest/)
