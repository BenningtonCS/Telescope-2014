#!/bin/sh

#----------------------------------------------------------
#
#	Name:	getIp.sh
#	About:	Attempts to get geolocation info from IP
#		and set STATION in config file
#	Author: Erick Daniszewski, Bennington College
#	Date:	6 March 2014
#
#----------------------------------------------------------


echo "> Trying to get your geolocation from IP..."
# From a IP-to-Geolocation website, parse out the data so we are left with the location, latitude, and longitude
curl -s http://www.ipinfodb.com/my_ip_location.php | sed 's/^[ \t]*//' | grep '<li>Latitude\|<li>Longitude\|<li>City' | sed -e 's/<[^>]*>//g;s/^.*://' >> ipdata.txt 
# Get the Location, Lat, and Long stored in var1, var2, and var3, respectively
eval $(awk '{print "var"NR"="$1}' ipdata.txt)

# Function which will append the geolocation data when manually entered
manualStation(){
	# Declare the location, latitude, and longitude variables to be passed in
	local loc=$1
	local lat=$2
	local long=$3
	# Append to srt.cat and alert user that info was added.
	echo "STATION $2 $3 $1" | tr -d '\r' >> srt.cat; echo "> Station added to srt.cat."
}

# Display the geolocation information got from the curl above and wait for verification
while true; do
	# Display location, latitude, and longitude
	echo "> Location: $var1"
	echo "> Latitude: $var2"
	echo "> Longitude: $var3"
	# Ask user if the displayed information is correct
	read -p "> Is this correct? [y/n] " yn
	case $yn in
		# If the info is correct, append it to srt.cat and alert the user it was added
		[Yy]* ) echo "STATION $var2 $var3 $var1" | tr -d '\r' >> srt.cat; echo "> Station added to srt.cat."; break;;
		# If the info is incorrect, prompt the user to give the correct info and pass it to the manualStation function
		[Nn]* ) echo "> Manually input station information. If you wish to skip this now, you will need to add your station in the srt.cat file."; echo -n "> Location: "; read location; echo -n "> Latitude: "; read latitude; echo -n "> Longitude: "; read longitude; manualStation $location $latitude $longitude; break;;
	esac
done

# Remove the temp file ipdata.txt
rm ipdata.txt
