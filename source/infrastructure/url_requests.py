import pycurl
from StringIO import StringIO

def create_request(configData):

    if 'machine' in configData['testURL']:
        ip = ""
        for vm in configData['vms']:
            if 'name' in vm and vm['name'] == configData['testURL']['machine']\
                    and 'floating_ip_addr' in vm:
                ip = vm['floating_ip_addr']
                break
        if not ip:
            print "no IP for the request found"
            return -1
        if 'path' in configData['testURL']:
            configData['testURL']['fullURL'] = ip + configData['testURL']['path']
        else:
            configData['testURL']['fullURL'] = ip

    elif 'website' in configData['testURL']:
        if 'path' in configData['testURL']:
            configData['testURL']['fullURL'] = configData['testURL']['website'] + configData['testURL']['path']
        else:
            configData['testURL']['fullURL'] = configData['testURL']['website']

    else:
        print "missing machine or website in testURL"
        return -1

    configData['response'] = """#!/bin/bash
echo "8b88abe6-fe60-43f9-96b2-623b8558117f"
echo "$(date)"
"""
    #TODO response
    return 1



def test_infrastructure(configData):
    if create_request(configData) == -1:
        return -1

    buffer = StringIO()
    c = pycurl.Curl()
    c.setopt(c.URL, configData['testURL']['fullURL'])
    c.setopt(c.WRITEDATA, buffer)
    try:
        c.perform()
    except pycurl.error:
        try:
            print "second attempt to send request"
            c.perform()
        except pycurl.error:
            print "empty reply"
            c.close()
            return -1
    c.close()

    body = buffer.getvalue()
    print(body)

    #TODO test
    if buffer.getvalue() == configData['response']:
        print "ja"
        return 1
    return 1 #TODO dodelat test req.
