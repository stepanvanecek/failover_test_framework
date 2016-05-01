#!/usr/bin/python

import os
import sys
import json
import datetime
from parse_input import check_config_structure
from build_nova import build_infrastructure, wait_for_spawn
from url_requests import test_infrastructure
import subprocess


### MAIN
if __name__ == "__main__":

    if len(sys.argv) > 1:
        print sys.argv[1]
        configFile = sys.argv[1]
    else:
        configFile = 'config.json'

    with open(configFile) as json_data_file:
        try:
            configData = json.load(json_data_file)
        except ValueError:
            print "wrong data format. should be json"
            exit(1);

    if check_config_structure(configData) == -1:
        print "problem reading config file"
        exit(1);

    configData['launch_time'] = datetime.datetime.now()
    print configData['launch_time']
    if build_infrastructure(configData) == -1:
        print "problem building the infrastructure"
        exit(1)

    if wait_for_spawn(configData) == -1:
        print "machines didn't spawn properly"
        exit(1)


    if test_infrastructure(configData) == -1:
        print "infrastructure not built properly"
        #erase built VMs
        exit(1)

    if not 'path' in configData['output']:
        configData['output']['path'] = "no"
    if not 'mail' in configData['output']:
        configData['output']['mail'] = "no"
    command = "./test_framework/test_framework/program.o " + configData['testURL']['fullURL'] + " " + configData['output']['path'] + " " + configData['output']['mail']
    print command
    data = subprocess.call( command , shell=True)

    #TODO remove infrastructure

 #       exit(1);