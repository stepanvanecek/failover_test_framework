#!/bin/bash
apt-get -y update;
apt-get -y install haproxy;

sed -i '2s/.*/ENABLED=1/' /etc/default/haproxy;

mv /etc/haproxy/haproxy.cfg{,.original};

cat >/etc/haproxy/haproxy.cfg <<EOL
global
        log /dev/log   local0
        log 127.0.0.1   local1 notice
        maxconn 4096
        user haproxy
        group haproxy
        daemon

defaults
        log     global
        mode    http
        option  httplog
        option  dontlognull
        retries 3
        option redispatch
        maxconn 2000
        contimeout     5000
        clitimeout     50000
        srvtimeout     50000

listen webfarm 0.0.0.0:80
    mode http
    stats enable
    stats uri /haproxy?stats
    balance roundrobin
    option httpclose
    option forwardfor
    server apache1 80.156.222.96:80 check
    server apache2 87.190.238.22:80 check

EOL

service haproxy start;
