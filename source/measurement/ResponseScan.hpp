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
    int Run();
private:
    TestData * t;
    //pre-failover variables
    list<Response*>::iterator ptr_first;
    list<Response*>::iterator ptr_last;
    list<Response*>::iterator last_pre_failover;
    
    int GatherData(bool last = false);
    bool pre_failover;
    
    int totalRequests;
   // int primary_vms; jako results_pre.size()
    list<Response*> * responses;
    //pthread_mutex_t * responses_list_mutex;
    
    map<const char *, ResponseData, cmp_str> results_pre;
    
    
    int TriggerFailover();
    
    //post-failover variables
    long timeout_start;
    
    bool incorrect_response_detected;
    int correct_responses_after_failover;
    long failover_start;
    long failover_finish;
//    int * failover_len_ms;
    void CountArraySize();
    void ResponsesPost();
    int request_buffer_size;
    list <Response *> request_buffer;
    int inside_buffer;
    
    bool * terminate;
};

#endif /* ResponseScan_hpp */
