#!/bin/bash
apt-get -y update;
apt-get -y install apache2;

echo $(cat /var/lib/cloud/data/instance-id) > /var/www/html/index.html;

service apache2 start;
