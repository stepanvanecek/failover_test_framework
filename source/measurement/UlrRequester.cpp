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

#define MIN_TIMEOUT 10000
#define MAX_THREADS 1000


pthread_mutex_t UrlRequester::req_mutex;
pthread_mutex_t UrlRequester::responses_list_mutex;

long UrlRequester::timeout;
void * UrlRequester::t;

UrlRequester::UrlRequester(void * v)
{
    t = v;
    this->timeout = (((TestData *)t)->min_interval_ms * MAX_THREADS > MIN_TIMEOUT)? (long)(((TestData *)t)->min_interval_ms * MAX_THREADS): MIN_TIMEOUT;

    pthread_mutex_init(&req_mutex, NULL);

    pthread_mutex_init(&responses_list_mutex, NULL);

    ((TestData *)t)->responses_list_mutex = & responses_list_mutex;
    ((TestData *)t)->terminate = false;

}
UrlRequester::~UrlRequester()
{
    pthread_join(generator, NULL);
//    int error;
//    for(int i=0; i< MAX_THREADS; i++) {
//        error = pthread_join(cons_pool[i].thread, NULL);
//    }
    
    //delete [] cons_pool;
}

int UrlRequester::Run()
{
    int error;
    error = pthread_create(&generator,
                           NULL,
                           cons_func,
                           (void *)NULL);
    if(error != 0)
    {
        fprintf(stderr, "Couldn't run thread generator, errno %d\n", error);
    }
    
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

void * UrlRequester::pull_one_url(void * ptr)
{
    Request_thr * req_struct = (Request_thr *)ptr;
/*
    char * c = (char *)malloc(sizeof(char *)*10);
    Response * r = new Response();
    r->ptr = c;
    r->time_sent = get_act_time_ms();
    
    printf ("--x-%ld-x--%s\n", r->time_sent, r->ptr);
    
    pthread_mutex_lock(&responses_list_mutex);
    ((TestData *)t)->responses->push_back(r);
    pthread_mutex_unlock(&responses_list_mutex);
/*////*
 
    Response * r = new Response();
    CURL * curl;
    curl = curl_easy_init();
    r->time_sent = get_act_time_ms();

    //cout << "url being sent: " << ((TestData *)t)->url << " at time: " << r->time_sent << endl;
    curl_easy_setopt(curl, CURLOPT_URL, ((TestData *)t)->url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_func);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, r);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout);
    
    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
        r->received = false;
    }

    curl_easy_cleanup(curl);
    
    //printf ("--x-%ld-x--%s\n", r->time_sent, r->ptr);
    pthread_mutex_lock(&responses_list_mutex);
    
    ((TestData *)t)->responses->push_back(r);
    pthread_mutex_unlock(&responses_list_mutex);
 
// */
//    printf ("--x-%ld-x--\n", get_act_time_ms());

    //curl_easy_setopt(curl, CURLOPT_ACCEPTTIMEOUT_MS, 1000);
    //curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 10000);
    //    //curl_easy_setopt(CURL *handle, CURLOPT_LOW_SPEED_LIMIT, long speedlimit);
    //    Pass a long as parameter. It contains the average transfer speed in bytes per second that the transfer should be below during CURLOPT_LOW_SPEED_TIME seconds for libcurl to consider it to be too slow and abort.
    //        curl_easy_setopt(CURL *handle, CURLOPT_LOW_SPEED_TIME, long speedtime);
    //    Pass a long as parameter. It contains the time in number seconds that the transfer speed should be below the CURLOPT_LOW_SPEED_LIMIT for the library to consider it too slow and abort.

    req_struct->active = false;
    return(NULL);
}


void * UrlRequester::cons_func(void * ptr)
{
    int error;
    Request_thr thread_pool[MAX_THREADS];
    for(int i = 0; i<MAX_THREADS; i++)
    {
        thread_pool[i].active = false;
    }
    long int lastLaunchTime = 0;
    int thrIndex = 0;
    while(!((TestData *)t)->terminate)
    {
        if(!thread_pool[thrIndex].active)
        {
            pthread_mutex_lock(&req_mutex);
            long int actTime = get_act_time_ms();
            if(actTime - lastLaunchTime >= ((TestData *)t)->min_interval_ms)
            {
                thread_pool[thrIndex].active = true;
                lastLaunchTime = actTime;
            }
            pthread_mutex_unlock(&req_mutex);
            if(thread_pool[thrIndex].active)
            {
                error = pthread_create(&thread_pool[thrIndex].thread,
                                       NULL, /* default attributes */
                                       pull_one_url,
                                       (void *)&thread_pool);
                if(error != 0)
                {
                    fprintf(stderr, "Couldn't run thread number %d, errno %d\n", thrIndex, error);
                }
            }
            else
                usleep(((TestData *)t)->min_interval_ms * 100);
        }
        else
            usleep(((TestData *)t)->min_interval_ms * 100);
        
        if(thrIndex == MAX_THREADS - 1)
            thrIndex = 0;
        else
            thrIndex ++;
        
    }
    for(int i=0; i< MAX_THREADS; i++)
        error = pthread_join(thread_pool[i].thread, NULL);

    pthread_exit(NULL);
    return NULL;
}


