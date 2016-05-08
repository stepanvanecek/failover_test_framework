#!/bin/bash

echo "Please enter your OpenStack Password: "
read -sr OS_PASSWORD_INPUT

TMP_FILE="tmp_data.json"

#build the infrastructure
python infrastructure/main.py $OS_PASSWORD_INPUT $TMP_FILE $1

#perform the measurement
if [[ $? == 0 ]]; then
	echo measurement/program.o $OS_PASSWORD_INPUT $TMP_FILE $1 
	measurement/program.o $OS_PASSWORD_INPUT $TMP_FILE $1 
fi

python infrastructure/clean.py $OS_PASSWORD_INPUT $TMP_FILE
#rm $TMP_FILE