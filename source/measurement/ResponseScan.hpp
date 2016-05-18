//
//  ResponseScan.hpp
//  test_framework
//
//  Created by Stepan Vanecek on 28/04/16.
//  Copyright © 2016 Stepan Vanecek. All rights reserved.
//

#ifndef ResponseScan_hpp
#define ResponseScan_hpp

#include <stdio.h>
#include <map>
#include <vector>
#include <cstring>

#include "UlrRequester.hpp"
#include "TestData.hpp"

#define MIN_REQUESTS_PER_MACHINE 40
#define POSITIVE_REP 3

struct cmp_str
{
    bool operator()(char const *a, char const *b) const
    {
        return std::strcmp(a, b) < 0;
    }
};

class ResponseData
{
public:
    int count;
    vector<long int> times;
    const char * response;//neni potreba
    double pre_failover_ratio;
    
    //post
    bool post_checked;
    bool is_set_in_downtime;
};

class ResponseScan{
public:
    ResponseScan(TestData * t);
    ~ResponseScan();
    int Run();
private:
    TestData * t;
    //pre-failover variables
    list<Response*>::iterator ptr_first;
    list<Response*>::iterator ptr_last;
    
    int GatherData();
    
    int totalRequests;

    list<Response *> * responses;
    map<const char *, ResponseData, cmp_str> results_pre;
    
    long startdiff;
    
    
    int TriggerFailover();
    
    //post-failover variables
    list <Response *> request_buffer;
    long timeout_start;
    
    bool incorrect_response_detected;
    int correct_responses_after_failover;
    long failover_start;
    long failover_finish;
    void CountArraySize();
    void ResponsesPost();
    int request_buffer_size;
    
    int inside_buffer;
    
    //bool * terminate;
};

#endif /* ResponseScan_hpp */
