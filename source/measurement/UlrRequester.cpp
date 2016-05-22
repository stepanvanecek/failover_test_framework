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
#include "jsoncpp/json/json-forwards.h"
#include "jsoncpp/json/json.h"

#define TIMEOUT 5000
#define CONNECTTIMEOUT 2000
#define MAX_THREADS 1000


pthread_mutex_t UrlRequester::req_mutex;
pthread_mutex_t UrlRequester::responses_list_mutex;

long UrlRequester::timeout;
void * UrlRequester::t;

UrlRequester::UrlRequester(void * v)
{
    t = v;

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
    string str = (char*)ptr;//TODO to string or to char?
    //cout << "--------------------------------" << (char*)ptr << " - " << size << " - " << nmemb << endl;
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
 
    Response * r = new Response();
    CURL * curl;
    curl = curl_easy_init();
    r->time_sent = get_act_time_ms();
    
    pthread_mutex_lock(&responses_list_mutex);
    ((TestData *)t)->responses->push_back(r);
    pthread_mutex_unlock(&responses_list_mutex);

    //cout << "url being sent: " << ((TestData *)t)->url << " at time: " << r->time_sent << endl;
    curl_easy_setopt(curl, CURLOPT_URL, ((TestData *)t)->url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_func);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, r);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, TIMEOUT);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, CONNECTTIMEOUT);
    curl_easy_setopt(curl, CURLOPT_MAXCONNECTS, 50);
    long http_code = 0;
    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
    //curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1);
    
    r->time_sent = get_act_time_ms();
    CURLcode res = curl_easy_perform(curl);
    r->time_received = get_act_time_ms();//TODO thread-unsafe?
    if(res != CURLE_OK || strcmp(r->ptr,"") == 0 || http_code >= 400)
    {
        r->received = false;
        //printf ("--x-%ld--%ld---%ld--x--%s   false\n", r->time_sent, r->time_received-r->time_sent, r->time_received, r->ptr);
    }
    else
    {
//        printf ("--x-%ld--%ld---%ld--x--%s\n", r->time_sent, r->time_received-r->time_sent, r->time_received, r->ptr);
//        Json::Value v = ((TestData*)(t))->config;
//        if (v.isMember("incorrect_responses"))
//        {
//            for (auto itr : v["incorrect_responses"])
//            {
//                string resp = itr.asString();
//                if(strcmp(r->ptr,resp.c_str()) == 0)
//                    r->received = false;
//            }
//        }
    }
    
    curl_easy_cleanup(curl);

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
        //cout << "url "<< !((TestData *)t)->terminate << endl;
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
                //pthread_cancel(thread_pool[thrIndex].thread);
                //pthread_join(thread_pool[thrIndex].thread,NULL);
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
            {
                usleep(((TestData *)t)->min_interval_ms);//microseconds
            }
        }
        else
        {
            if(thrIndex == MAX_THREADS -1)
            {
                usleep(((TestData *)t)->min_interval_ms * 128);
            }
        }
        
        if(thrIndex == MAX_THREADS - 1)
            thrIndex = 0;
        else
            thrIndex ++;
        
    }
    cout << "Joining threads.." << endl;
    //for(int i=0; i< MAX_THREADS; i++)
    //    error = pthread_cancel(thread_pool[i].thread);
    for(int i=0; i< MAX_THREADS; i++)
        error = pthread_join(thread_pool[i].thread, NULL);

    //pthread_exit(NULL);
    return NULL;
}


