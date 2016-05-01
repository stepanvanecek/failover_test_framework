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


#include "Response.hpp"

using namespace std;

class UrlRequester
{
public:
    UrlRequester(void * t);
    ~UrlRequester();
    int Run();
private:
    static size_t write_callback_func(void *ptr, size_t size, size_t nmemb, Response *s);
    static long int get_act_time_ms();
    static void * pull_one_url(void * url);
    //static void * prod_func(void * i);
    
    static void * cons_func(void * i);
    
    static bool * terminate;
    static list<Response*> * responses;
    
    const char * url;
    //int RunProd();
    int RunCons();
    
    pthread_t * cons_pool;
    //pthread_t prod_thread;
    static int consThreads;
    static pthread_mutex_t req_mutex;
    static pthread_mutex_t responses_list_mutex;
    static long int req;
    static int min_interval_ms;
    static long timeout;
    
};

#endif /* UlrRequester_hpp */
