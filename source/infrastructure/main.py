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

    # if len(sys.argv) > 2:
    #     configFile = sys.argv[1]
    # else:
    #     configFile = 'config.json'
    #
    # try:
    #     with open("../config_files/" + configFile) as json_data_file:
    #         try:
    #             configData = commentjson.load(json_data_file)
    #         except ValueError:
    #             print "wrong data format. should be json"
    #             exit(1)
    #         except commentjson.JSONLibraryException:
    #             print "wrong data format. should be json"
    #             exit(1)
    # except IOError:
    #     print "file not found/permission"
    #     exit(1)
    #
    # configData['creds']['os_password'] = sys.argv[2]
    #
    # print "Checking JSON structure..."
    # if check_config_structure(configData) == -1:
    #     print "problem reading config file"
    #     exit(1)
    #
    # configData['launch_time'] = datetime.datetime.now().strftime('%Y%m%d%H%M%S%f')
    # print "Building the infrastructure..."
    # if build_infrastructure(configData) == -1:
    #     print "problem building the infrastructure"
    #     exit(1)
    #
    # if wait_for_spawn(configData) == -1:
    #     print "machines didn't spawn properly"
    #     exit(1)
    #
    # raw_input("Press Enter once the HA installation is ready:")
    #
    # print "Sending test request to ensure the operability."
    # if test_infrastructure(configData) == -1:
    #     print "infrastructure not built properly"
    #     #erase built VMs
    #     exit(1)
    # print " Request received."
    # print "---"
    # with open('tmp.json', 'w') as outfile:
    #     json.dump(configData, outfile)
    # print "Testing availability of a service " + configData['test_url']['full_url']

    exit(0) # OK
"""
    if not 'path' in configData['output']:
        configData['output']['path'] = "no"
    if not 'mail' in configData['output']:
        configData['output']['mail'] = "no"
    command = "./test_framework/test_framework/program.o " + configData['test_url']['full_url'] + " " + configData['output']['path'] + " " + configData['output']['mail']
    print command
    data = subprocess.call( command , shell=True)
"""
    #TODO remove infrastructure

 #       exit(1);