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
    this->terminate = &t->terminate;
    totalRequests = 0;
}

int ResponseScan::Run()
{
    GatherData();

    CountArraySize();
    //TODO spocitat prumerne neuspechy, aby se potom mohly tolerovat
    
    TriggerFailover();
    
    timeout_start = UrlRequester::get_act_time_ms();
    
    cout << "********triggering failover********" << endl;
    
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
    list<Response*>::iterator resp_it;
    long last_time_sent = 0;
    
    while(!*terminate)
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
        
        //the response is already received - either good or bad
        request_buffer.push_back(*resp_it);

        if(inside_buffer == request_buffer_size)
        {
            delete(*request_buffer.begin());
            request_buffer.pop_front();
        }
        else
            inside_buffer ++;

        responses->pop_front();

        if(!incorrect_response_detected)
        {
            //if incorrect response is deteced. Covers 5.3.2.1, 5.3.2.2, 5.3.2.3
            if(!(*resp_it)->received)
            {
                
                incorrect_response_detected = true;
                failover_start = (*resp_it)->time_sent;
                
                startdiff = failover_start - last_time_sent;
                
                cout << "failover start: " << failover_start << " precision: " << startdiff << endl;
                correct_responses_after_failover = 0;
            }
            //5.3.2.4 initially scan everything
            else if(inside_buffer == request_buffer_size)
            {
                for (auto & x : results_pre)
                {
                    x.second.post_checked = false;
                }
                cout << "hele353" << endl;
                for (list<Response*>::iterator x=request_buffer.begin(); x != request_buffer.end(); ++x)
                {
                    if(distance(request_buffer.begin(), x) <= (*results_pre.find((*x)->ptr)).second.pre_failover_ratio)//TODO overit, jestli nedelim 3!!!
                    {
                        (*results_pre.find((*x)->ptr)).second.post_checked = true;
                    }
                    /*old
                     if(distance(request_buffer.begin(), x) <= (*results_pre.find((*x)->ptr)).second.pre_failover_ratio)
                     (*results_pre.find((*x)->ptr)).second.post_checked = true;
                     */
                }
                for (auto & x : results_pre)
                {
                    bool found = false;
                    if(!x.second.post_checked)
                    {
                        //x.first je jmeno masiny ve failoveru
                        x.second.is_set_in_downtime = true;
                        //cout << "DOWNTIME: " << x.first << endl;
                        found = true;
                    }
                    if(found)
                    {
                        * terminate = true;
                        cout << "zero downtime" << endl;
                        t->result_failover_len_ms = 0;
                        return;
                    }
                }
            }
        }
        
        if(incorrect_response_detected)
        {
            cout << "hele34" << endl;
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
                    //x.first je jmeno masiny ve failoveru
                    x.second.is_set_in_downtime = true;
                    //cout << "DOWNTIME: " << x.first << endl;
                }
            }
            if((*resp_it)->received)
            {
                correct_responses_after_failover ++;
                if(correct_responses_after_failover == 1)
                failover_finish = (*resp_it)->time_sent * results_pre.size();
                
                for(auto & x : results_pre)
                {
                    if(x.second.is_set_in_downtime)
                    {
                        //cout << "podminka :" << abs(correct_responses_after_failover - x.second.pre_failover_ratio/POSITIVE_REP) << endl;
                        if(abs(correct_responses_after_failover - x.second.pre_failover_ratio/POSITIVE_REP) < 0.5 )
                        {
                            failover_finish = (*resp_it)->time_sent;
                            cout << "to delete failover finish:" << failover_finish << endl;
                            t->result_failover_len_ms = (int)(failover_finish - failover_start);
                        }
                    }
                }
            }
            else
            correct_responses_after_failover = 0;
            
            if(correct_responses_after_failover >= request_buffer_size)
            {
                cout << "Failover finished: " << failover_finish << endl;
                cout << "terminating" << endl;
                * terminate = true;
                return;
            }
        }
        
        last_time_sent = (*resp_it)->time_sent;
        
        if(UrlRequester::get_act_time_ms() - timeout_start > t->timeout)
            *terminate = true;
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
    cout << "buffer size: " << request_buffer_size << endl;
    inside_buffer = 0;
    
    return;
}

int ResponseScan::GatherData()//implicit false
{
    list<Response*>::iterator resp_it;

    while(!*terminate)
    {
        
        //        pthread_mutex_lock(responses_list_mutex);
        //        if(distance(responses->begin(), responses->end()) != 0)
        //            cout << "---" << distance(responses->begin(), responses->end()) << endl;
        //        for (list<Response*>::iterator it=responses->begin(); it != responses->end(); ++it)
        //            std::cout << "list data: " << (*it)->time_sent << " - " << (*it)->ptr << " - " << ((*it)->received? "ok" : "failure");
        //        pthread_mutex_unlock(responses_list_mutex);
        
        
        //        cout << "----------------" << totalRequests<< endl;
        //        for( auto & x : results_pre)
        //        {
        //            cout << x.second.times.size() << " " << x.first <<  ": ";
        //            for (auto & y : x.second.times)
        //            {
        //                cout << y << "   ";
        //            }
        //            cout << endl;
        //        }
        //        cout << "----------------" << endl;
        
        
        
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
            //TODO chyba kdyz nema byt
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
                //memcpy(s->ptr+s->len, ptr, size*nmemb);
                d.response = message;
                d.is_set_in_downtime = false;
                d.times.push_back((*resp_it)->time_sent);
                cout << "----------------------------------pushing: " << message << "--" << (*resp_it)->time_sent << endl;
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

