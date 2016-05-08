
#include<unistd.h>
#include<list>
#include <sys/time.h>

#include "jsoncpp/json/json-forwards.h"
#include "jsoncpp/json/json.h"
#include <fstream>

#include "UlrRequester.hpp"
#include "ResponseScan.hpp"

using namespace std;

int WriteOutputStructure(TestData * t);

int main(int argc, const char * argv[])
{
    TestData * t;
    if(argc >= 2)
        t  = new TestData(argv);
    else
    {
        exit(1);
    }
    
    UrlRequester *r = new UrlRequester((void*)t);
    r->Run();

    //usleep(200000);

    ResponseScan c(t);
    c.Run();
    //usleep(30000000);
    cout << "failover len: " << t->result_failover_len_ms << endl;
    
    WriteOutputStructure(t);
    cout << "Deallocating the resources..." << endl;
    delete r;
    delete t;
    cout << "DONE." << endl;
    return 0;
    
}

int WriteOutputStructure(TestData * t)
{
    Json::Value root;
    Json::Value * config = t->config;
    root["id"] = (*config)["id"];
    root["ts"] = (*config)["launch_time"];
    root["downtime"] = t->result_failover_len_ms;
    root["precision"] = t->result_failvoer_precision_ms;
    root["vms"] = (*config)["vms"];;
    root["notes"] = (*config)["notes"];
    
    Json::StyledStreamWriter writer;
    cout << "Writing to output file..." << endl;
    ofstream test1((*config)["output"]["file"].asString());
    writer.write(test1,root);
    
    return 1;
}


//
//
//#include <iostream>
//#include <pthread.h>
//#include <unistd.h>
//#include <unistd.h>
//#include <curl/curl.h>
//#include <pthread.h>
//
//#include "Request_thr.h"
//
//#define MAX_THREADS 2000
//#define MIN_INTERVAL_MS 5
//pthread_mutex_t req_mutex;
//
//class response
//{
//public:
//    char *ptr;
//    size_t len;
//    
//};
//
//size_t write_callback_func(void *ptr, size_t size, size_t nmemb, response *s)
//{
//    size_t new_len = s->len + size*nmemb;
//    s->ptr = (char*)realloc(s->ptr, new_len+1);
//    if (s->ptr == NULL) {
//        fprintf(stderr, "realloc() failed\n");
//        exit(EXIT_FAILURE);
//    }
//    memcpy(s->ptr+s->len, ptr, size*nmemb);
//    s->ptr[new_len] = '\0';
//    s->len = new_len;
//    
//    return size*nmemb;
//}
//
//void init_response(response *s) {
//    s->len = 0;
//    s->ptr = (char*)malloc(s->len+1);
//    if (s->ptr == NULL) {
//        fprintf(stderr, "malloc() failed\n");
//        exit(EXIT_FAILURE);
//    }
//    s->ptr[0] = '\0';
//}
//
//long int get_act_time_ms()
//{
//    struct timeval tp;
//    gettimeofday(&tp, NULL);
//    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
//}
//
//static void * pull_one_url(void * url)
//{
//    CURL * curl;
//    response r;
//    init_response(&r);
//    
//    curl = curl_easy_init();
//    long int ms = get_act_time_ms();
//    
//    curl_easy_setopt(curl, CURLOPT_URL, (const char *)url);
//    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_func);
//    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r);
//    CURLcode res = curl_easy_perform(curl);
//    if(res != CURLE_OK)
//        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
//    
//    printf ("--x-%ld-x--%s\n", ms, r.ptr);
//    curl_easy_cleanup(curl);
//    
//    
//    //pthread_mutex_lock(&mutex_req_threads);
//    //((Request_thr *)req_class)->active = false;
//    //usleep(1000000);
//    //pthread_exit(NULL);
//    //printf("ending\n");
//    return(NULL);
//    //pthread_exit(NULL);
//    //pthread_mutex_unlock(&mutex_req_threads);
//}
//
//int req;
//
//static void * cons_func(void * url)
//{
//    bool call_url_req = false;
//    while(true)
//    {
//        pthread_mutex_lock(&req_mutex);
//        if(req != 0)
//        {
//            //printf ("taken\n");
//            call_url_req = true;
//            req = 0;
//        }
//        pthread_mutex_unlock(&req_mutex);
//        
//        if(call_url_req)
//        {
//            pull_one_url(url);
//            call_url_req = false;
//        }
//        else
//        {
//            usleep(1000000);
//        }
//        
//    }
//}
//
//static void * prod_func(void * i)
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
//        usleep(MIN_INTERVAL_MS * 1000 - 100);
//    }
//    
//    
//    return NULL;
//}
//
//
//
//int main(int argc, const char * argv[])
//{
//    
//    req = 0;
//    int index;
//    int error;
//
//    
//    
//    //Request_thr thread_pool[MAX_THREADS];
//    
//    pthread_t prod_thread;
//    error = pthread_create(&prod_thread,
//                   NULL,
//                   prod_func,
//                   (void *)&argv[1]);
//    if(error != 0)
//    {
//        fprintf(stderr, "Couldn't run producer thread, errno %d\n", error);
//    }
//
//
//    pthread_t thread_pool[MAX_THREADS];
//    for(index=0; index<MAX_THREADS; index++)
//    {
//       // printf("uuuuu %s\n", argv[1]);
//        error = pthread_create(&thread_pool[index],
//                       NULL,
//                       cons_func,
//                       (void *)argv[1]);
//        if(error != 0)
//        {
//            fprintf(stderr, "Couldn't run thread number %d, errno %d\n", index, error);
//        }
//    }
//    
//    for(int i=0; i< MAX_THREADS; i++) {
//        error = pthread_join(thread_pool[i], NULL);
//        //fprintf(stderr, "Thread %d terminated\n", i);
//    }
//    
//    return 0;
//}


//
//    //have to scan the args
//    int print = 0;
//    int error;
//    
//    bool running = true;
//    
//    /* Must initialize libcurl before any threads are started */
//    curl_global_init(CURL_GLOBAL_ALL);
//    int thrIndex = 0;
//    //Initialize the threads and also values
//    Request_thr thread_pool[MAX_THREADS];
//    for(int i = 0; i<MAX_THREADS; i++)
//    {
//        thread_pool[i].active = false;
//        thread_pool[i].url_ptr = argv[1];
//    }
//    long int lastLaunchTime = 0;
//    //for(int k = 0; k<100; k++)
//    int testRuns = 0;
//    while (running)
//    {
//        long int actTime = get_act_time_ms();
//        if( actTime - lastLaunchTime > MIN_INTERVAL_MS )
//        {
//            pthread_mutex_lock(&mutex_req_threads);
//            bool isActive = thread_pool[thrIndex].active;
//            pthread_mutex_unlock(&mutex_req_threads);
//            
//            if(!isActive)
//            {
//                printf("%d\n", print++);
//                thread_pool[thrIndex].active = true;
//                lastLaunchTime = get_act_time_ms();
//                error = pthread_create(&thread_pool[thrIndex].thread,
//                                       NULL, /* default attributes */
//                                       pull_one_url,
//                                       (void *)&thread_pool[thrIndex]);
//                if(error != 0)
//                {
//                    fprintf(stderr, "Couldn't run thread number %d, errno %d\n", thrIndex, error);
//                }
//                else
//                {
//                    //fprintf(stderr, "Thread %d, gets %s\n", i, "abcd");
//                }
//                if(testRuns++ == 50)/////////////////////////////////////////////////////////////
//                    running = false;
//            }
//            
//            if(thrIndex == MAX_THREADS - 1)
//                thrIndex = 0;
//            else
//                thrIndex ++;
//        }
//        else
//            usleep(MIN_INTERVAL_MS * 1000 /2);
//    }
//    printf("koneec %ld\n", get_act_time_ms());
//    for(int i=0; i< MAX_THREADS; i++) {
//        error = pthread_join(thread_pool[i].thread, NULL);
//        //fprintf(stderr, "Thread %d terminated\n", i);
//    }
//    printf("koneec %ld\n", get_act_time_ms());
//    
//    return 0;
//}
//
//


//
//
//#include <iostream>
////#include <stdlib.h>
//#include <pthread.h>
//#include <curl/curl.h>
////#include <sys/time.h>
////#include <string.h>
//#include <unistd.h>
//
//#include "Request_thr.h"
//
//#define MAX_THREADS 2000
//#define MIN_INTERVAL_MS 10
//pthread_mutex_t mutex_req_threads;
//
//class response
//{
//public:
//    char *ptr;
//    size_t len;
//    
//};
//
//size_t write_callback_func(void *ptr, size_t size, size_t nmemb, response *s)
//{
//    size_t new_len = s->len + size*nmemb;
//    s->ptr = (char*)realloc(s->ptr, new_len+1);
//    if (s->ptr == NULL) {
//        fprintf(stderr, "realloc() failed\n");
//        exit(EXIT_FAILURE);
//    }
//    memcpy(s->ptr+s->len, ptr, size*nmemb);
//    s->ptr[new_len] = '\0';
//    s->len = new_len;
//    
//    return size*nmemb;
//}
//
//void init_response(response *s) {
//    s->len = 0;
//    s->ptr = (char*)malloc(s->len+1);
//    if (s->ptr == NULL) {
//        fprintf(stderr, "malloc() failed\n");
//        exit(EXIT_FAILURE);
//    }
//    s->ptr[0] = '\0';
//}
//
//long int get_act_time_ms()
//{
//    struct timeval tp;
//    gettimeofday(&tp, NULL);
//    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
//}
//
//static void * pull_one_url(void * req_class)
//{
//    CURL * curl;
//    response r;
//    init_response(&r);
//    
//    curl = curl_easy_init();
//    long int ms = get_act_time_ms();
//    
//    curl_easy_setopt(curl, CURLOPT_URL, ((Request_thr *)req_class)->url_ptr);
//    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_func);
//    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r);
//    CURLcode res = curl_easy_perform(curl); /* ignores error */
//    if(res != CURLE_OK)
//        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
//    printf ("--x-%ld-x--%s\n", ms, r.ptr);
//    curl_easy_cleanup(curl);
//    
//    
//    pthread_mutex_lock(&mutex_req_threads);
//    ((Request_thr *)req_class)->active = false;
//    usleep(1000000);
//    //pthread_exit(NULL);
//    printf("ending\n");
//    pthread_exit(NULL);
//    pthread_mutex_unlock(&mutex_req_threads);
//}
//
//int main(int argc, const char * argv[])
//{
//    //have to scan the args
//    int print = 0;
//    int error;
//    
//    bool running = true;
//    
//    /* Must initialize libcurl before any threads are started */
//    curl_global_init(CURL_GLOBAL_ALL);
//    int thrIndex = 0;
//    //Initialize the threads and also values
//    Request_thr thread_pool[MAX_THREADS];
//    for(int i = 0; i<MAX_THREADS; i++)
//    {
//        thread_pool[i].active = false;
//        thread_pool[i].url_ptr = argv[1];
//    }
//    long int lastLaunchTime = 0;
//    //for(int k = 0; k<100; k++)
//    int testRuns = 0;
//    while (running)
//    {
//        long int actTime = get_act_time_ms();
//        if( actTime - lastLaunchTime > MIN_INTERVAL_MS )
//        {
//            pthread_mutex_lock(&mutex_req_threads);
//            bool isActive = thread_pool[thrIndex].active;
//            pthread_mutex_unlock(&mutex_req_threads);
//            
//            if(!isActive)
//            {
//                printf("%d\n", print++);
//                thread_pool[thrIndex].active = true;
//                lastLaunchTime = get_act_time_ms();
//                error = pthread_create(&thread_pool[thrIndex].thread,
//                                       NULL, /* default attributes */
//                                       pull_one_url,
//                                       (void *)&thread_pool[thrIndex]);
//                if(error != 0)
//                {
//                    fprintf(stderr, "Couldn't run thread number %d, errno %d\n", thrIndex, error);
//                }
//                else
//                {
//                    //fprintf(stderr, "Thread %d, gets %s\n", i, "abcd");
//                }
//                //if(testRuns++ == 50)/////////////////////////////////////////////////////////////
//                //    running = false;
//            }
//            
//            if(thrIndex == MAX_THREADS - 1)
//                thrIndex = 0;
//            else
//                thrIndex ++;
//        }
//        else
//            usleep(MIN_INTERVAL_MS * 1000 /2);
//    }
//    printf("koneec %ld\n", get_act_time_ms());
//    for(int i=0; i< MAX_THREADS; i++) {
//        error = pthread_join(thread_pool[i].thread, NULL);
//        //fprintf(stderr, "Thread %d terminated\n", i);
//    }
//    printf("koneec %ld\n", get_act_time_ms());
//    
//    return 0;
//}
//
