
#include<unistd.h>
#include<list>
#include <sys/time.h>

#include "jsoncpp/json/json-forwards.h"
#include "jsoncpp/json/json.h"
#include <fstream>

#include "UlrRequester.hpp"
#include "ResponseScan.hpp"

using namespace std;

int WriteOutputStructure(TestData * t, const char *);

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

    ResponseScan c(t);
    c.Run();

    if(!t->timeout_reached)
        cout << "Downtime length: " << t->result_failover_len_ms << endl;
    
    WriteOutputStructure(t, argv[3]);
    cout << "Deallocating the resources..." << endl;
    delete r;
    delete t;
    cout << "DONE." << endl;
    return 0;
    
}

int WriteOutputStructure(TestData * t, const char * frameworkPath)
{
    Json::Value root;
    Json::Value * config = t->config;
    root["id"] = (*config)["id"];
    root["ts"] = (*config)["launch_time"];
    if(t->timeout_reached)
    {
        root["downtime"] = 0;
        root["precision"] = 0;
        root["success"] = false;
    }
    else
    {
        root["downtime"] = t->result_failover_len_ms;
        root["precision"] = t->result_failvoer_precision_ms;
    }

    root["vms"] = (*config)["vms"];;
    root["notes"] = (*config)["notes"];
    
    Json::StyledStreamWriter writer;
    string path = frameworkPath;
    path += "/results/" + (*config)["output"]["file"].asString();
    cout << "Writing the output file to " << path << endl;
    ofstream test1(path);
    writer.write(test1,root);
    
    return 1;
}
