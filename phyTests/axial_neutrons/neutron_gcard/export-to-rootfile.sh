if [ $# -eq 0 ] || [ $# -ne 4 ]
	then
		echo "Not enough arguments supplied. Specify particle first [proton, neutron, electron, etc.]..."
		echo "Then, specify number of bins and max value of histogram plot..."
		exit
fi
rp=$(realpath $1)
dn=$(dirname $rp)
fn=$(basename $rp .hipo)
echo "Analysis for $1:"
cd ../build
make && cd $dn && ../build/analysis $1 $2 $fn $3 $4
echo "Completed file $1:"
