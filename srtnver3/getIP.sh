#!/bin/sh

echo "> Trying to get your geolocation from IP..."
# From a IP-to-Geolocation website, parse out the data so we are left with the location, latitude, and longitude
curl -s http://www.ipinfodb.com/my_ip_location.php | sed 's/^[ \t]*//' | grep '<li>Latitude\|<li>Longitude\|<li>City' | sed -e 's/<[^>]*>//g;s/^.*://' >> ipdata.txt 
# Get the Location, Lat, and Long stored in var1, var2, and var3, respectively
eval $(awk '{print "var"NR"="$1}' ipdata.txt)


manualStation(){
	local loc=$1
	local lat=$2
	local long=$3

	echo "STATION $2 $3 $1" | tr -d '\r' >> srt.cat; echo "> Station added to srt.cat."
}


while true; do
	echo "> Location: $var1"
	echo "> Latitude: $var2"
	echo "> Longitude: $var3"
	read -p "> Is this correct? [y/n] " yn
	case $yn in
		[Yy]* ) echo "STATION $var2 $var3 $var1" | tr -d '\r' >> srt.cat; echo "> Station added to srt.cat."; break;;
		[Nn]* ) echo "> Manually input station information. If you wish to skip this now, you will need to add your station in the srt.cat file."; echo -n "> Location: "; read location; echo -n "> Latitude: "; read latitude; echo -n "> Longitude: "; read longitude; manualStation $location $latitude $longitude; break;;
	esac
done

rm ipdata.txt

# Append the geolocation information with the keyword to the srt.cat file
#echo "STATION $var2 $var3 $var1" | tr -d '\r' >> srt.cat
