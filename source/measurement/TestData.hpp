//
//  TestData.hpp
//  test_framework
//
//  Created by Stepan Vanecek on 28/04/16.
//  Copyright © 2016 Stepan Vanecek. All rights reserved.
//

#ifndef TestData_hpp
#define TestData_hpp

#include <stdio.h>
#include <list>

#include "jsoncpp/json/json-forwards.h"
#include "jsoncpp/json/json.h"

#include "UlrRequester.hpp"
#include "Response.hpp"

using namespace std;

//class ConfigData
//{
//public:
//    string os_password;
//    
//};
class ConfigData
{
public:
    string terminate_command;
    string os_username;
    string os_tenant_name;
    string out_file;
    string id;
    string notes;
    string ts;
    
    
    const char * os_password;
};

class TestData
{
public:
    TestData(const char ** url);
    TestData(const char * url, int min_interval_ms);
    //TestData(Json::Value root, int min_interval_ms);
    ~TestData();
    bool terminate;
    list<Response *> * responses;
    
    pthread_mutex_t * responses_list_mutex;
    
    int result_failover_len_ms;
    int result_failvoer_precision_ms;
    
    //config file data
    ConfigData c;
    string url;
    
    int min_interval_ms;
    

private:
    int TriggerFailover();
    int ParseInputFile(const char * argv);
};

#endif /* TestData_hpp */
