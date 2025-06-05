if [ $# -eq 0 ] || [ $# -ne 4 ]
	then
		echo "Not enough arguments supplied. Specify particle first [proton, neutron, electron, etc.]..."
		echo "Specify kinetic energy"
		echo "Then, specify number of bins and max value of histogram plot..."
		exit
fi
echo "${PWD}"
# $1 is file name, $2 is particle, $3 the kinetic energy, $4 is amount histogram bins/range
rp=$(realpath $1)
dn=$(dirname $rp)
fn=$(basename $rp .hipo)
echo "==Making root file ${fn}.root=="
echo "Analysis for $1:"
cd ../build
make && cd $dn && ../build/analysis $1 $2 $3 $4 $fn
echo "Completed file $1:"
