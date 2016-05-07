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
    if len(sys.argv) > 3:
        configFile = sys.argv[3]
    else:
        configFile = 'config.json'

    try:
        with open("../config_files/" + configFile) as json_data_file:
            try:
                configData = commentjson.load(json_data_file)
            except ValueError:
                print "wrong data format. should be json"
                exit(1)
            except commentjson.JSONLibraryException:
                print "wrong data format. should be json"
                exit(1)
    except IOError:
        print "file not found/permission"
        exit(1)

    configData['creds']['os_password'] = sys.argv[1]

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
        print "infrastructure not built properly"
        #erase built VMs
        with open(sys.argv[2], 'w') as outfile:
            json.dump(configData, outfile)
        exit(1)
    print " Request received."
    print "---"

    #configData['test_url']['full_url'] = "87.190.239.41" #TODO zakomentovat

    configData['creds']['os_password'] = ""

    file = configData['test_url']['full_url'] + "\n" \
           + configData['failover_trigger'] + "\n" \
           + configData['creds']['os_username'] + "\n" \
           + configData['creds']['os_tenant_name'] + "\n" \
           + configData['output']['path'] + "\n" \
           + configData['id'] + "\n" \
           + configData['launch_time'] + "\n" \
           + configData['notes'] + "\n" \
           + str(configData['precision_ms']) + "\n"

    f = open(sys.argv[2], 'w')
    f.write(file)
    f.close()

    with open( sys.argv[2], 'w') as outfile:
        json.dump(configData, outfile)

    print "Testing availability of a service " + configData['test_url']['full_url']

    exit(0) # OK

    """    file = str(len(configData['test_url']['full_url'])) + ";" + configData['test_url']['full_url'] + ";\n" \
        + str(len(configData['failover_trigger'])) + ";" + configData['failover_trigger'] + ";\n" \
        + str(len(configData['creds']['os_username'])) + ";" + configData['creds']['os_username'] + ";\n" \
        + str(len(configData['creds']['os_tenant_name'])) + ";" + configData['creds']['os_tenant_name'] + ";\n" \
        + str(configData['precision_ms']) + ";\n" """
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