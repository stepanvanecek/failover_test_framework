#!/bin/bash

echo "Please enter your OpenStack Password: "
read -sr OS_PASSWORD_INPUT


SOURCE_DIR="source"
TMP_FILE="${SOURCE_DIR}/tmp_data.json"

#build the infrastructure
python $SOURCE_DIR/infrastructure/main.py $OS_PASSWORD_INPUT $TMP_FILE $1

#perform the measurement
if [[ $? == 10 ]]; then
	$SOURCE_DIR/measurement/program.o $OS_PASSWORD_INPUT $TMP_FILE $1 
fi

python $SOURCE_DIR/infrastructure/clean.py $OS_PASSWORD_INPUT $TMP_FILE
#rm $TMP_FILE