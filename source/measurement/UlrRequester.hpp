//
//  UlrRequester.hpp
//  test_framework
//
//  Created by Stepan Vanecek on 27/04/16.
//  Copyright © 2016 Stepan Vanecek. All rights reserved.
//

#ifndef UlrRequester_hpp
#define UlrRequester_hpp

#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <list>

#include "Request_thr.h"
//#include "TestData.hpp"

#include "Response.hpp"


using namespace std;

class UrlRequester
{
public:
    UrlRequester(void * t);
    ~UrlRequester();
    int Run();
    static long int get_act_time_ms();
private:
    static void * t;
    
    static size_t write_callback_func(void *ptr, size_t size, size_t nmemb, Response *s);
    static void * pull_one_url(void * url);
    static void * cons_func(void * i);
    
    pthread_t generator;
    
    static pthread_mutex_t req_mutex;
    static pthread_mutex_t responses_list_mutex;
    static long timeout;
    
};

#endif /* UlrRequester_hpp */
