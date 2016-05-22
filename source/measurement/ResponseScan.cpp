//
//  ResponseScan.cpp
//  test_framework
//
//  Created by Stepan Vanecek on 28/04/16.
//  Copyright © 2016 Stepan Vanecek. All rights reserved.
//

#include <unistd.h>
#include <cmath>
#include <pthread.h>
#include "ResponseScan.hpp"


ResponseScan::ResponseScan(TestData * t)
{
    this->t = t;
    this->responses = t->responses;
    totalRequests = 0;
}

ResponseScan::~ResponseScan()
{
    for (auto & x : results_pre)
    {
        delete x.first;
    }
    for (auto & x : request_buffer)
    {
        delete x;
    }
}

int ResponseScan::Run()
{
    GatherData();

    CountArraySize();
    
    TriggerFailover();
    
    timeout_start = UrlRequester::get_act_time_ms();
    
    cout << "********triggering failover********" << timeout_start << endl;
    
    ResponsesPost();
    
    return 0;
}

int ResponseScan::TriggerFailover()
{
    system(t->terminate_command.c_str());
    return 1;
}

void ResponseScan::ResponsesPost()
{
    Response* resp_begin_ptr;
    long last_time_sent = 0;

    
    while(!t->terminate)
    {
        Response* to_delete;
        while(true) //no further request sent - the last one was already processed
        {
            pthread_mutex_lock(t->responses_list_mutex);
            auto b = responses->begin();
            auto e = responses->end();
            pthread_mutex_unlock(t->responses_list_mutex);
            
            if(b != e)
            break;
            
            usleep(t->min_interval_ms * 1024);
        }
        
        resp_begin_ptr = *responses->begin();
        
        while((resp_begin_ptr)->time_received == -1) //not received - wait until it is received
        {
            usleep(t->min_interval_ms * 1024);
        }
        
        //the response is already received - either good or bad
        request_buffer.push_back(resp_begin_ptr);

        if(inside_buffer == request_buffer_size)
        {
            to_delete = *request_buffer.begin();
            delete to_delete;
            request_buffer.pop_front();
        }
        else
            inside_buffer ++;

        responses->pop_front();

        if(!incorrect_response_detected)
        {
            //if incorrect response is deteced. Covers 5.3.2.1, 5.3.2.2, 5.3.2.3
            if(!(resp_begin_ptr)->received && (resp_begin_ptr)->time_sent >= timeout_start)
            {
                incorrect_response_detected = true;
                failover_start = resp_begin_ptr->time_sent;
                
                startdiff = failover_start - last_time_sent;
                
                cout << "Failover start: " << failover_start << " precision: " << startdiff << endl;
                correct_responses_after_failover = 0;
            }
            //5.3.2.4 initially scan everything
            else if(inside_buffer == request_buffer_size)
            {
                for (auto & x : results_pre)
                {
                    x.second.post_checked = false;
                }
                for (list<Response*>::iterator x=request_buffer.begin(); x != request_buffer.end(); ++x)
                {
                    if(distance(request_buffer.begin(), x) <= (*results_pre.find((*x)->ptr)).second.pre_failover_ratio)//TODO
                    {
                        (*results_pre.find((*x)->ptr)).second.post_checked = true;
                    }
                }
                for (auto & x : results_pre)
                {
                    bool found = false;
                    if(!x.second.post_checked)
                    {
                        x.second.is_set_in_downtime = true;
                        found = true;
                    }
                    if(found)
                    {
                        t->terminate = true;
                        cout << "zero downtime" << endl;
                        t->result_failover_len_ms = 0;
                        t->result_failvoer_precision_ms = (int)((resp_begin_ptr->time_sent - last_time_sent)*results_pre.size()/2);
                        return;
                    }
                }
            }
        }
        
        if(incorrect_response_detected)
        {
            //cout << "point1 - " << resp_begin_ptr->received << " - " << resp_begin_ptr->time_sent << endl;
            for (auto & x : results_pre)
            {
                x.second.post_checked = false;
            }
            for (list<Response*>::iterator x=request_buffer.begin(); x != request_buffer.end(); ++x)
            {
                if(results_pre.find((*x)->ptr) != results_pre.end())
                {
                    if(distance(request_buffer.begin(), x) <= (*results_pre.find((*x)->ptr)).second.pre_failover_ratio)
                    {
                        (*results_pre.find((*x)->ptr)).second.post_checked = true;
                    }
                }
            }
            
            for (auto & x : results_pre)
            {
                if(!x.second.post_checked)
                {
                    x.second.is_set_in_downtime = true;
                }
            }
            if(resp_begin_ptr->received)
            {
                correct_responses_after_failover ++;
                if(correct_responses_after_failover == 1)
                {
                    failover_finish = last_time_sent;
                }
                
                for(auto & x : results_pre)
                {
                    if(x.second.is_set_in_downtime)
                    {
                        //cout << "condition :" << abs(correct_responses_after_failover - x.second.pre_failover_ratio/POSITIVE_REP) << endl;
                        if(abs(correct_responses_after_failover - x.second.pre_failover_ratio) < 0.5 )
                        {
                            t->result_failover_len_ms = (int)(failover_finish - failover_start);
                            t->result_failvoer_precision_ms = (int)((startdiff + resp_begin_ptr->time_sent - last_time_sent)*results_pre.size()/4);
                            cout << "Failover finished: " << failover_finish << endl;
                            t->terminate = true;
                            return;
                        }
                    }
                }
            }
            else
                correct_responses_after_failover = 0;
            
            if(correct_responses_after_failover >= request_buffer_size)
            {
                t->result_failover_len_ms = (int)(failover_finish - failover_start);
                t->result_failvoer_precision_ms = (int)((startdiff + resp_begin_ptr->time_sent - last_time_sent)*results_pre.size()/4);
                cout << "Failover finish: " << failover_finish << endl;
                t->terminate = true;
                return;
            }
        }
        
        last_time_sent = resp_begin_ptr->time_sent;
        
        if(UrlRequester::get_act_time_ms() - timeout_start > t->timeout)
        {
            cout << "Timeout reached" << endl;
            t->timeout_reached = true;
            t->terminate = true;
            return;
        }
    }
    
}

void ResponseScan::CountArraySize()
{
    request_buffer_size = 0;//TODO is necessary?
    incorrect_response_detected = false;
    
    for (auto & x : results_pre)
    {
        x.second.pre_failover_ratio = (double)(POSITIVE_REP * totalRequests)/(double)(x.second.count);
        if(x.second.pre_failover_ratio > request_buffer_size)
            request_buffer_size = x.second.pre_failover_ratio;
    }
    //cout << "buffer size: " << request_buffer_size << endl;
    inside_buffer = 0;
    
    return;
}

int ResponseScan::GatherData()
{
    list<Response*>::iterator resp_it;

    while(!t->terminate)
    {
        while(true) //no further request sent - the last one was already processed
        {
            pthread_mutex_lock(t->responses_list_mutex);
            auto b = responses->begin();
            auto e = responses->end();
            pthread_mutex_unlock(t->responses_list_mutex);

            if(b != e)
                break;
            
            usleep(t->min_interval_ms * 1024);
        }
        resp_it = responses->begin();
        
        while((*resp_it)->time_received == -1) //not received - wait until it is received
        {
            usleep(t->min_interval_ms * 1024);
        }
        
        if(!(*resp_it)->received)
        {
            //TODO should not happen
        }
        else
        {
            totalRequests++;
            auto map_it = results_pre.find((*resp_it)->ptr);
            if (map_it == results_pre.end())
            {
                ResponseData d;
                d.count = 1;
                char * message = (char*)malloc(sizeof(char)*(*resp_it)->len + 1);
                memcpy(message, (*resp_it)->ptr, sizeof(const char)*(*resp_it)->len + 1);
                d.response = message;
                d.is_set_in_downtime = false;
                d.times.push_back((*resp_it)->time_sent);

                results_pre.insert(pair<const char *, ResponseData>(message, d));
            }
            else
            {
                map_it->second.times.push_back((*resp_it)->time_sent);
                map_it->second.count++;
            }
        }
        delete(*responses->begin());
        responses->pop_front();
        
        
        if(totalRequests > 1000 || ((totalRequests >= results_pre.size() * MIN_REQUESTS_PER_MACHINE) && totalRequests > 10))
        {
            break;
        }
    }

    return 0;
}

