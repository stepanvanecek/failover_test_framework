import pycurl
from StringIO import StringIO
import time

def create_request(configData):

    if 'machine' in configData['test_url']:
        ip = ""
        for vm in configData['vms']:
            if 'name' in vm and vm['name'] == configData['test_url']['machine']\
                    and 'floating_ip_addr' in vm:
                ip = vm['floating_ip_addr']
                break
        if not ip:
            print "no IP for the request found"
            return -1
        if 'path' in configData['test_url']:
            configData['test_url']['full_url'] = ip + configData['test_url']['path']
        else:
            configData['test_url']['full_url'] = ip

    elif 'website' in configData['test_url']:
        if 'path' in configData['test_url']:
            configData['test_url']['full_url'] = configData['test_url']['website'] + configData['test_url']['path']
        else:
            configData['test_url']['full_url'] = configData['test_url']['website']

    else:
        print "missing machine or website in testURL"
        return -1

    return 1



def test_infrastructure(configData):
    if create_request(configData) == -1:
        return -1

    buffer = StringIO()
    c = pycurl.Curl()
    c.setopt(c.URL, configData['test_url']['full_url'])
    c.setopt(c.WRITEDATA, buffer)
    try:
        c.perform()
    except pycurl.error:
        try:
            time.sleep(5)
            print "second attempt to send request"
            c = pycurl.Curl()
            c.setopt(c.URL, configData['test_url']['full_url'])
            c.setopt(c.WRITEDATA, buffer)
            c.perform()
        except pycurl.error:
            print "empty reply"
            c.close()
            return -1
    c.close()

    body = buffer.getvalue()

    if 'incorrect_responses' in configData:
        for ir in configData['incorrect_responses']:
            if body == ir:
                return -1

    return 1
