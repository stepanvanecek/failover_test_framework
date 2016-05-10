import json
import sys
from build_nova import nova_client
from novaclient import exceptions

### MAIN
if __name__ == "__main__":

    try:
        with open(sys.argv[2]) as json_data_file:
            try:
                tmpConfigData = json.load(json_data_file)
            except ValueError:
                print "wrong data format. should be json"
                exit(1)
    except IOError:
        print "file not found/permission"
        exit(1)

    for vm in tmpConfigData['vms']:

        nova = nova_client(tmpConfigData['creds']['os_username'], \
                           tmpConfigData['creds']['os_tenant_name'], \
                           sys.argv[1], \
                           vm['deployment'])
        try:
            nova.servers.delete(vm['id'])
        except exceptions.NotFound:
            i = 1
        except exceptions.Unauthorized:
            exit(1)