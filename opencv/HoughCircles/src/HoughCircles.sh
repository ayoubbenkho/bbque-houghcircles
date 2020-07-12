############### File: HoughCircles.sh ###############
 
#!/bin/bash
 
# Edit this line
PATH_TO_BOSP="/home/ayoub/BOSP"
PATH_TO_HOUGHCIRCLES="home/ayoub/BOSP/out/usr/bin"
 
# Sourcing the shell
. $PATH_TO_BOSP/out/etc/bbque/bosp_init.env
 
# Just for some logging
NUMBER_OF_TESTS=0
 
# Results are stored here
printf "%-10s %7s  %7s  %7s  %7s  %7s  %8s  %6s\n" APPNAME \
THREADS MIN_RADIUS MAX_RADIUS MIN_DIST QOS EXECTIME USAGE > test_results
 
# Running on Unmanaged Mode, with performance counters sampling,
# on the MDEV cpus with 100% CPU quota and 50MB max memory usage
export BBQUE_RTLIB_OPTS="U:p:C 1 100000 -1 0 52428800"
 
for THREADS in 1 2 3 4 5 6 7 8; do
    for MIN_RADIUS in 0 1 5 10 20; do
        for MAX_RADIUS in 20 30 50 100; do
            for MIN_DIST in 1 8 16 32 64; do 

                # Logging
                echo "Performing test $THREADS $MIN_RADIUS $MAX_RADIUS $MIN_DIST"
                let NUMBER_OF_TESTS++
     
                # Running the test
                $PATH_TO_HOUGHCIRCLES/houghcircles -j 50 -t $THREADS -i $MIN_RADIUS -a $MAX_RADIUS -m $MIN_DIST &> test_tmp
     
                # Extracting information
                USAGE=$( grep -o "[0-9\ \.]\{1,\} CPUs utilized" test_tmp | awk -F' ' '{print $1}' )
                QOS=$( echo $MAX_RADIUS"-"$MIN_RADIUS"-"$MIN_DIST | bc )
                EXECTIME=$( grep -o "Process[0-9\ \.\:]\{1,\}" test_tmp | grep -o "[0-9\.]\{1,\}" )
     
                # Storing the results
                printf "%-10s %7s  %7s  %7s  %7s  %3s  %7s  %6s\n" "hough" \
                    $THREADS $MIN_RADIUS $MAX_RADIUS $MIN_DIST $QOS $EXECTIME $USAGE >> test_results
     
                rm test_tmp
            done
        done
    done
done