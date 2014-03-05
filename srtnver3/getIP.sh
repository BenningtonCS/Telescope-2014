#!/bin/sh

curl -S http://www.ipinfodb.com/my_ip_location.php | sed 's/^[ \t]*//' | grep '<li>City\|<li>IP address\|<li>Country\|<li>Latitude\|<li>Longitude' >> tmpfile.txt 
sed -e 's/<[^>]*>//g' tmpfile.txt >> ipdata.txt
rm tmpfile.txt