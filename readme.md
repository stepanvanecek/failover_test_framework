Test framework for cloud failover mechanism
===========================================


Installation
------------

####Download the libraries

- Download libcurl from this [link](https://curl.haxx.se/download.html) and follow the [installation guide](https://curl.haxx.se/docs/install.html).

- Download [Python 2.7](https://www.python.org/downloads/release/python-2711/).

- Other libraries will get installed using pip. Type 

``` {.sourceCode .bash}
cd source
sudo make install
``` 

to install the rest packages.

####Compile the soure code

To compile the source code, type:

``` {.sourceCode .bash}
#you should now be in directory 'source'
sudo make compile
``` 