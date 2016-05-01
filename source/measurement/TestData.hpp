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

#include "UlrRequester.hpp"
#include "Response.hpp"

using namespace std;

class TestData
{
public:
    TestData(const char * url, int max_threads, int min_interval_ms);
    ~TestData();
    bool terminate;
    list<Response *> * responses;
    const char * url;
    int max_threads;
    int min_interval_ms;
    
    pthread_mutex_t * responses_list_mutex;
    
    int failover_len_ms;
    int failvoer_precision_ms;
private:
};

#endif /* TestData_hpp */
