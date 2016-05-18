Test framework for cloud failover mechanisms
============================================


Installation
------------

#### Clone the Github repository

``` {.sourceCode .bash}
git clone https://github.com/stepanvanecek/failover_test_framework.git
``` 

#### Prerequisities

It is assumed that you have downloaded the following in order to complete the installation and run the measurement:

- libcurl. If not, see the [download link](https://curl.haxx.se/download.html) and follow the [installation guide](https://curl.haxx.se/docs/install.html).

- python 2.7. You need python 2.7.9 or later. [link](https://www.python.org/downloads/)

- python pip. [link](https://pypi.python.org/pypi/pip)

####Download the libraries

``` {.sourceCode .bash}
cd source
sudo make install
``` 

to install the necessary packages.

####Compile the soure code

Assuming you are in directory 'source', type to compile the source code:

``` {.sourceCode .bash}
make compile
``` 

Run the measurement
-------------------

To run the measurement, the configuration file has to be specified. Then it is triggered with command "./run.sh config_file_name".

The test deploys the specified infrastructure, provides the measurement and deletes the VMs it has spawned for the test.


Configuration file
~~~~~~~~~~~~~~~~~~

The configuration files should be stored in directory "config_files" in the root directory of the framework. That's the directory where the framework searches for the configuration files.

The configuration file is a JSON file enabling comments (//). It has a defined structure that needs to be kept.
// * = required; o = optional

* id - identifier of the test
* vms - array with the specification of VMs
	o name - name of the VM
	* image_id - image ID to use for launching the VM
	* flavor - flavor ID or name
	* deployment - deployment to launch the VM in
	o keypair - name of the key
	o sec_group - array of security group names, "default" is used by default
	o floating_ip - if set to true, floating IP will be associated
	o cloud_init - path to the cloud-init script
* test_url
	o machine - name of a VM with a Floating IP to address
	o website - the host name of the tested service
	//either "machine" or "website" must be entered
	o path - path to send the requests on a defined service
* output
	* file - name of the output file to create
* failover_trigger - a command or script to perform to trigger the failover
* creds - openstack credentials
	* os_username
	* os_tenant_name
* precision_ms - minimal/desired gap between the requests in milliseconds
o timeout - timeout in seconds, 300 s if not set
o notes - notes further describing the 

See the example configuration file in config_files/config_example.

Workflow
~~~~~~~~

The configuration file should enable to up an infrastructure with a failover mechanism. The workflow of builing the infrastructure is the following:

1)The specified VMs will be spawned in the specified deployments. As a part of the spawning, the cloud-init scripts are performed rigth after the boot of the VM.
2)The Floating IPs are associated to specified VMs.
3)The VMs are rebooted.
4)The user is given the opportunity to perform manual scripts on the VMs. He is for example able to establish an SSH connection using the SSH keypairs. During this phase, the user must finish the set up of the failover solution. Once the user is finished with the setting, the workflow continues with consecutive steps.


Output file
-----------

 The result of the measurement is written to the output file. Its name is specified in the configuration file. Output files are generated into directory "results".

 The output file contains the values measured in the text and the specification. It contaains:

 - downtime - the downtime length in millieconds
 - id - the ID of the test as specified in the config. file
 - notes - notes taken to describe the test in the configuration file
 - precision - precision of the measurement in milliseconds
 - ts - time stamp of the test
 - vms - the array with specification of the VMs
 	- auth_url - the openstack authentication URL of the deployment
 	- deployment
 	- flavor
 	- floating_ip - indicator if the floaring IP was associated
 	- floating_ip_addr - floating IP address associated to the VM
 	- id - id of the VM
 	- image_id - ID of the image
 	- keypair
 	- name
