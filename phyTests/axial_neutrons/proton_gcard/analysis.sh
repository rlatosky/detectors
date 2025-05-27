
for i in *.gcard
do
	echo "Analysis for $i:"
	gemc $i -USE_GUI=0 -N=10000 -PRINT_EVENT=1000
	echo "Completed file $i"
done
