//
//  UlrRequester.cpp
//  test_framework
//
//  Created by Stepan Vanecek on 27/04/16.
//  Copyright © 2016 Stepan Vanecek. All rights reserved.
//

#include "UlrRequester.hpp"

#include <unistd.h>
#include <curl/curl.h>
#include <pthread.h>

#include "TestData.hpp"

#define MIN_TIMEOUT 5000

pthread_mutex_t UrlRequester::req_mutex;
pthread_mutex_t UrlRequester::responses_list_mutex;

long int UrlRequester::req;
int UrlRequester::min_interval_ms;
int UrlRequester::consThreads;
list<Response*> * UrlRequester::responses;
bool * UrlRequester::terminate;
long UrlRequester::timeout;

UrlRequester::UrlRequester(void * v)
{
    TestData * t = (TestData*)v;
    this->url = t->url;
    this->min_interval_ms = t->min_interval_ms;
    this->consThreads = t->max_threads;
    
    this->timeout = (t->min_interval_ms * t->max_threads > MIN_TIMEOUT)? (long)(t->min_interval_ms * t->max_threads): MIN_TIMEOUT;
    pthread_mutex_init(&req_mutex, NULL);
    pthread_mutex_init(&responses_list_mutex, NULL);
    t->responses_list_mutex = & responses_list_mutex;
    cons_pool = new pthread_t[t->max_threads];
    
    this->responses = t->responses;
    this->terminate = &(t->terminate);
    *terminate = false;
    
    req = 0;
}
UrlRequester::~UrlRequester()
{
    int error;
    for(int i=0; i< consThreads; i++) {
        error = pthread_join(cons_pool[i], NULL);
//        //fprintf(stderr, "Thread %d terminated\n", i);
    }
    // error = pthread_join(prod_thread, NULL);
    
    delete [] cons_pool;
}

int UrlRequester::Run()
{
    this->RunCons();
    
    return 0;
}



long int UrlRequester::get_act_time_ms()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

size_t UrlRequester::write_callback_func(void *ptr, size_t size, size_t nmemb, Response *s)
{
    size_t new_len = s->len + size*nmemb;
    s->ptr = (char*)realloc(s->ptr, new_len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    
    return size*nmemb;
}

void * UrlRequester::pull_one_url(void * urll)
{
/*
    char * c = (char *)malloc(sizeof(char *)*10);
    Response * r = new Response();
    r->ptr = c;
    r->time_sent = get_act_time_ms();
    
    pthread_mutex_lock(&responses_list_mutex);
    responses->push_back(r);
    pthread_mutex_unlock(&responses_list_mutex);
/*////*
 
    Response * r = new Response();
    CURL * curl;
    curl = curl_easy_init();
    r->time_sent = get_act_time_ms();
    
    curl_easy_setopt(curl, CURLOPT_URL, (const char *)urll);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_func);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, r);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout);
    
    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
        //cout << "not received" << endl;
        r->received = false;
        //fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
    
    //printf ("--x-%ld-x--%s\n", r->time_sent, r->ptr);
    pthread_mutex_lock(&responses_list_mutex);
    responses->push_back(r);
    pthread_mutex_unlock(&responses_list_mutex);
 
// */
//    printf ("--x-%ld-x--\n", get_act_time_ms());

    //curl_easy_setopt(curl, CURLOPT_ACCEPTTIMEOUT_MS, 1000);
    //curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 10000);
    //    //curl_easy_setopt(CURL *handle, CURLOPT_LOW_SPEED_LIMIT, long speedlimit);
    //    Pass a long as parameter. It contains the average transfer speed in bytes per second that the transfer should be below during CURLOPT_LOW_SPEED_TIME seconds for libcurl to consider it to be too slow and abort.
    //        curl_easy_setopt(CURL *handle, CURLOPT_LOW_SPEED_TIME, long speedtime);
    //    Pass a long as parameter. It contains the time in number seconds that the transfer speed should be below the CURLOPT_LOW_SPEED_LIMIT for the library to consider it too slow and abort.

    
    return(NULL);
}


void * UrlRequester::cons_func(void * url)
{
    bool call_url_req = false;
    while(!*terminate)
    {
        //cout << "terminate: " << (*terminate? "true" : "false") << endl;
        pthread_mutex_lock(&req_mutex);
        long int actTime = get_act_time_ms();
        if(actTime - req >= min_interval_ms)
        {
            call_url_req = true;
            req = actTime;
        }
        pthread_mutex_unlock(&req_mutex);
        
        if(call_url_req)
        {
            pull_one_url(url);
            call_url_req = false;
        }
        else
        {
            usleep(min_interval_ms * 1000 * consThreads/2);
        }
        
    }
    //TODO if list is too long
    //printf("terminating\n");
    pthread_exit(NULL);
    return NULL;
}

int UrlRequester::RunCons()
{
    int error;
    for(int index=0; index<consThreads; index++)
    {
        error = pthread_create(&(cons_pool[index]),
                               NULL,
                               cons_func,
                               (void *)url);
        if(error != 0)
        {
            fprintf(stderr, "Couldn't run thread number %d, errno %d\n", index, error);
        }
        //usleep(min_interval_ms * 1000 * consThreads/2);
    }
    return 0;
}


//void * UrlRequester::prod_func(void * i)
//{
//
//    //long interval = (long)&i;
//    while(true)
//    {
//        pthread_mutex_lock(&req_mutex);
//
//        req = 1;
//        //printf ("produced\n");
//        pthread_mutex_unlock(&req_mutex);
//
//        usleep(min_interval_ms * 1000 - 100);
//    }
//
//
//    return NULL;
//}


//int UrlRequester::RunProd()
//{
//    //printf("here\n");
//    int error = pthread_create(&prod_thread,
//                           NULL,
//                           prod_func,
//                           (void *)url);
//    if(error != 0)
//    {
//        fprintf(stderr, "Couldn't run producer thread, errno %d\n", error);
//    }
//    return 0;
//}
