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

class TestData
{
public:
    TestData(const char ** url);
    TestData(const char * url, int min_interval_ms);
    ~TestData();
    
    list<Response *> * responses;
    
    bool terminate;
    int timeout;
    bool timeout_reached;
    
    int min_interval_ms;
    string url;
    string terminate_command;
    const char * os_password;
    
    int result_failover_len_ms;
    int result_failvoer_precision_ms;
    
    Json::Value * config;
    
    pthread_mutex_t * responses_list_mutex;
private:
    int TriggerFailover();
    int ParseInputFile(const char * argv);
    int ParseInputJson(string argv);
};

#endif /* TestData_hpp */
