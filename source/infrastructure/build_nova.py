
from novaclient import client as nvclient
from novaclient import exceptions
import time

authurl = "https://identity-<deployment_name>.cloudandheat.com:5000/v2.0"


def wait_for_spawn(configData):
    for vm in configData['vms']:
        nova = nova_client(configData['creds']['os_username'], \
                           configData['creds']['os_tenant_name'], \
                           configData['creds']['os_password'], \
                           vm['deployment'])

        attempts = 0
        while True:
            vm_data = nova.servers.get(vm['id'])
            if attempts > 30:
                print "vm " + vm_data.name +" unable to start"
                return -1
            if vm_data.status == 'ACTIVE':
                break
            time.sleep(5)
            attempts = attempts + 1
    return 1

def nova_client(username, tenant_name, password, deploynemt):
    return nvclient.Client(version='2',
                           username=username,
                           project_id=tenant_name,
                           api_key=password,
                           auth_url=authurl.replace('<deployment_name>', deploynemt),
                           insecure=False)

def check_rule(protocol, from_port, to_port, nova, secgroup):
    have_rule = False
    for rule in secgroup.rules:
        if rule['ip_protocol'] == protocol and rule['from_port'] == from_port and rule['to_port'] == to_port:
            have_rule = True
            break
    if not have_rule:
        nova.security_group_rules.create(secgroup.id,
                                         ip_protocol=protocol,
                                         from_port=from_port,
                                         to_port=to_port)

def launch_vms(configData):
    i=0
    for vm in configData['vms']:
        i = i+1
        vm['vm_name'] = configData['id'] + configData['launch_time'].strftime('%Y%m%d%H%M%S%f') + str(i)

        nova = nova_client(configData['creds']['os_username'],\
                           configData['creds']['os_tenant_name'],\
                           configData['creds']['os_password'],\
                           vm['deployment'] )

        try: 
            val = int(vm['flavor'])
            flavor = nova.flavors.get(vm['flavor'])
        except ValueError:
            flavor = nova.flavors.find(name = vm['flavor'])
    
        secgroup = nova.security_groups.find(name="default")
        image_id = vm['image_id']

        key_name = vm['keypair'] #TODO vyresit klice

        check_rule("tcp", 22, 22, nova, secgroup) # SSH rule
        check_rule("tcp", 80, 80, nova, secgroup) # HTTP rule


        if "_cloud_init" in vm: # TODO test cloud init souboru v parse_input
            with open(vm['_cloud_init']) as udata:
                userdata_script = udata.read()
        else:
            userdata_script = ""

        try:
            vm_data = nova.servers.create(name=vm['vm_name'],
                              image=image_id,
                              flavor=flavor.id,
                              key_name=key_name,
                              availability_zone='nova',
                              security_groups=['default'],
                              userdata=userdata_script)
        except exceptions.OverLimit:
            print "overLimit cannot build"
            return -1
        except exceptions.BadRequest:
            print "spawning unsuccessful"
            return -1
        vm['id'] = vm_data.id


    return 1

"""def associate_fip(vm_name, ip, creds):
    import subprocess
    command = "nova add-floating-ip --os-username <uname> --os-password <pass> --os-auth-url <authurl> --os-tenant-name <tenant> " + vm_name + " " + ip \
        .replace('<uname>', creds['os_username']) \
        .replace('<pass>', creds['os_password']) \
        .replace('<authurl>', authurl) \
        .replace('<tenant>', creds['os_tenant_name'])
    subprocess.call(command, shell=True)
    #TODO osetrit
    """

def allocate_fip(configData):

    for vm in configData['vms']:
        if "_floating_ip" in vm:
            nova = nova_client(configData['creds']['os_username'], \
                               configData['creds']['os_tenant_name'], \
                               configData['creds']['os_password'], \
                               vm['deployment'])
            floating_ip = ""
            for fip in nova.floating_ips.list():
                if not fip.instance_id:
                    floating_ip = fip.ip
                    break

            if not floating_ip:
                try:
                    floating_ip = nova.floating_ips.create()
                except exceptions.NotFound:
                    print "vm " + vm['id'] + " - unable to associate floating ip"
                    return -1 #TODO


            vm['floating_ip_addr'] = floating_ip
            try:
                nova.servers.add_floating_ip(vm['id'], floating_ip)
            except exceptions.BadRequest:
                print vm['floating_ip_addr'] + vm['id']
                return -1
    return 1

def reboot_vms(configData):
    for vm in configData['vms']:
        nova = nova_client(configData['creds']['os_username'], \
                           configData['creds']['os_tenant_name'], \
                           configData['creds']['os_password'], \
                           vm['deployment'])
        nova.servers.reboot(vm['id'])

def build_infrastructure(configData):
    if launch_vms(configData) == -1:
        return -1

    wait_for_spawn(configData)


    if allocate_fip(configData) == -1:
        return -1

    reboot_vms(configData)

    wait_for_spawn(configData)

    #TODO run additional scripts