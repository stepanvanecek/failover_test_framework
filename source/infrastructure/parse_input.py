
def check_req_data(config_data):

    if not "id" in config_data:
        print "The mandatory key 'id' is missing."
        exit(2)
    if not "vms" in config_data:
        print "The mandatory key 'vms' is missing."
        exit(2)
    else:
        for vm in config_data['vms']:
            if not "image_id" in vm:
                print "The mandatory key 'image_id' is missing."
                exit(2)
            if not "flavor" in vm:
                print "The mandatory key 'flavor' is missing."
                exit(2)
            if not "deployment" in vm:
                print "The mandatory key 'deployment' is missing."
                exit(2)
            # if not "request_path" in vm and (not "ha" in vm or vm['ha'] == True) :
            #     print "The mandatory key 'request_path' is missing."
            #     exit(2)
    if not "test_url" in config_data:
        print "The mandatory key 'testUrl' is missing."
        # TODO scan test url
        exit(2)
    if not "output" in config_data:
        print "The mandatory key 'output' is missing."
        exit(2)
    else:
        if not "file" in config_data['output'] and not "mail" in config_data['output']:
            print "Either 'file' or 'mail' must be filled in in 'output'."
            exit(2)
    if not "failover_trigger" in config_data:
        print "The mandatory key 'failover_trigger' is missing."
        exit(2)
    # TODO load creds from env variables?
    if not "creds" in config_data:
        print "The mandatory key 'creds' is missing."
        exit(2)
    else:
        if not "os_username" in config_data['creds']:
            print "The mandatory key 'os_username' is missing."
            exit(2)
        if not "os_tenant_name" in config_data['creds']:
            print "The mandatory key 'os_tenant_name' is missing."
            exit(2)
    if not "timeout" in config_data:
        print "The mandatory key 'timeout' is missing."
        exit(2)
    return 1

def check_data_validity(config_data):
    return 1

def check_config_structure(config_data):
    check_req_data(config_data)

    check_data_validity(config_data)

    return 1
