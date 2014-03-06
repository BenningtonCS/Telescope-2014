#!/bin/sh

# From a IP-to-Geolocation website, parse out the data so we are left with the location, latitude, and longitude
curl -S http://www.ipinfodb.com/my_ip_location.php | sed 's/^[ \t]*//' | grep '<li>Latitude\|<li>Longitude\|<li>City' | sed -e 's/<[^>]*>//g;s/^.*://' >> ipdata.txt 
# Get the Location, Lat, and Long stored in var1, var2, and var3, respectively
eval $(awk '{print "var"NR"="$1}' ipdata.txt)
# Append the geolocation information with the keyword to the srt.cat file
echo "STATION $var2 $var3 $var1" | tr -d '\r' >> srt.cat