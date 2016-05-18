#!/usr/bin/python

import os
import sys
import commentjson
import json
import datetime
from parse_input import check_config_structure
from build_nova import build_infrastructure, wait_for_spawn
from url_requests import test_infrastructure
import subprocess


### MAIN
if __name__ == "__main__":
    if len(sys.argv) > 4:
        configFile = sys.argv[4]
    else:
        print "No config file specified. Using 'config_example.json'"
        configFile = 'config_example.json'

    try:
        with open(sys.argv[3] + "/config_files/" + configFile) as json_data_file:
            try:
                configData = commentjson.load(json_data_file)
            except ValueError:
                print "Wrong data format. Should be json."
                exit(1)
            except commentjson.JSONLibraryException:
                print "Wrong data format. Should be json."
                exit(1)
    except IOError:
        print "File not found/permission was denied."
        exit(1)

    configData['creds']['os_password'] = sys.argv[1]
    configData['framework_dir'] = sys.argv[3]

    print "Checking JSON structure..."
    if check_config_structure(configData) == -1:
        print "problem reading config file"
        exit(1)

    configData['launch_time'] = datetime.datetime.now().strftime('%Y/%m/%d-%H:%M:%S')

    print "Building the infrastructure..."
    if build_infrastructure(configData) == -1:
        print "problem building the infrastructure"
        exit(1)

    if wait_for_spawn(configData) == -1:
        print "machines didn't spawn properly"
        exit(1)

    raw_input("Press Enter once the HA installation is ready:")

    print "Sending test request to ensure the operability."
    if test_infrastructure(configData) == -1:
        print "Infrastructure not built properly"
        #erase built VMs
        configData['creds']['os_password'] = ""
        with open(sys.argv[2], 'w') as outfile:
            json.dump(configData, outfile)
        exit(1)
    print " Request received."
    print "---"

    #configData['test_url']['full_url'] = "87.190.239.41" #TODO zakomentovat

    configData['creds']['os_password'] = ""

    #TODO perform always, even after an exception
    with open( sys.argv[2], 'w') as outfile:
        json.dump(configData, outfile)

    print "Testing availability of a service " + configData['test_url']['full_url']

    exit(10) # OK
