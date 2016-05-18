
#include<unistd.h>
#include<list>
#include <sys/time.h>

#include "jsoncpp/json/json-forwards.h"
#include "jsoncpp/json/json.h"
#include <fstream>

#include "UlrRequester.hpp"
#include "ResponseScan.hpp"

using namespace std;

int WriteOutputStructure(TestData * t);

int main(int argc, const char * argv[])
{
    TestData * t;
    if(argc >= 2)
        t  = new TestData(argv);
    else
    {
        exit(1);
    }
    
    UrlRequester *r = new UrlRequester((void*)t);
    r->Run();

    //usleep(200000);

    ResponseScan c(t);
    c.Run();
    //usleep(30000000);
    cout << "failover len: " << t->result_failover_len_ms << endl;
    
    WriteOutputStructure(t);
    cout << "Deallocating the resources..." << endl;
    delete r;
    delete t;
    cout << "DONE." << endl;
    return 0;
    
}

int WriteOutputStructure(TestData * t)
{
    Json::Value root;
    Json::Value * config = t->config;
    root["id"] = (*config)["id"];
    root["ts"] = (*config)["launch_time"];
    root["downtime"] = t->result_failover_len_ms;
    root["precision"] = t->result_failvoer_precision_ms;
    root["vms"] = (*config)["vms"];;
    root["notes"] = (*config)["notes"];
    
    Json::StyledStreamWriter writer;
    cout << "Writing to output file " << (*config)["output"]["file"].asString() << endl;
    ofstream test1((*config)["output"]["file"].asString());
    writer.write(test1,root);
    
    return 1;
}
