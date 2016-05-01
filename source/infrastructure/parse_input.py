


def check_req_data(config_data):

    if not "id" in config_data:
        # TODO error
        return -1
    if not "vms" in config_data:
        # TODO error
        return -1
    else:
        for vm in config_data['vms']:
            if not "image_id" in vm:
                # TODO error
                return -1
            if not "flavor" in vm:
                # TODO error
                return -1
            if not "deployment" in vm:
                # TODO error
                return -1
            if not "_request_path" in vm and (not "_ha" in vm or vm['_ha'] == True) :
                # TODO error
                return -1
    return 1
    if not "testUrl" in config_data:
        # TODO error
        return -1
    if not "output" in config_data:
        # TODO error
        return -1
    else:
        if not "path" in config_data['output'] and not "mail" in config_data['output']:
            # TODO error
            return -1
    if not "failover_trigger" in config_data:
        # TODO error
        return -1
    if not "creds" in config_data:
        # TODO error
        return -1
    else:
        if not "os_username" in config_data['creds']:
            # TODO error
            return -1
        if not "os_password" in config_data['creds']:
            # TODO error
            # TODO davat password do config??????
            return -1
        if not "os_tenant_name" in config_data['creds']:
            # TODO error
            return -1
    if not "timeout" in config_data:
        # TODO error
        return -1
    return 1

def check_data_validity(config_data):
    if "_floating_ip" in config_data:
        return 1
    return 1

def check_config_structure(config_data):
    if check_req_data(config_data) == -1:
        return -1

    if check_data_validity(config_data) == -1:
        return -1

    return 1
