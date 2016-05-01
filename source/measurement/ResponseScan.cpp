//
//  ResponseScan.cpp
//  test_framework
//
//  Created by Stepan Vanecek on 28/04/16.
//  Copyright © 2016 Stepan Vanecek. All rights reserved.
//

#include <unistd.h>
#include <cmath>

#include "ResponseScan.hpp"


ResponseScan::ResponseScan(TestData * t)
{
    this->responses = t->responses;
    this->terminate = &t->terminate;
    totalRequests = 0;
    pre_failover = true;
    this->failover_len_ms = &t->failover_len_ms;
    this->responses_list_mutex = t->responses_list_mutex;
    
}

int ResponseScan::Run()
{
    while(!*terminate)
    {
//        for (list<Response*>::iterator it=responses->begin(); it != responses->end(); ++it)
//            std::cout << "list data: " << (*it)->time_sent << " - " << (*it)->ptr << " - " << ((*it)->received? "ok" : "failure");

        GatherData();
        
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
        
        if(pre_failover && (totalRequests >= results_pre.size() * MIN_REQUESTS_PER_MACHINE) && totalRequests > 10)
        {
            last_pre_failover = responses->end();
            GatherData(true);
            CountArraySize();
            //TODO trigger the failover
            //TODO spocitat prumerne neuspechy, aby se potom mohly tolerovat
            break;
        }
        usleep(10);
        
    }
    cout << "********triggering failover********" << endl;
    while(!*terminate)
    {
        ResponsesPost();
        usleep(100000);
    }
    return 0;
}

void ResponseScan::ResponsesPost()
{
    ptr_first = next(responses->begin(), inside_buffer);
    ptr_last = prev(responses->end());
    
    for (list<Response*>::iterator resp_it=ptr_first; resp_it != next(ptr_last); ++resp_it)
    {
        if(inside_buffer == request_buffer_size)
        {
            delete(*responses->begin());
            responses->pop_front();
            request_buffer.pop_front();
        }
        else
            inside_buffer ++;

        request_buffer.push_back(*resp_it);
        
        if(!incorrect_response_detected)
        {
            //if incorrect response is deteced. Covers 5.3.2.1, 5.3.2.2, 5.3.2.3
            if(!(*resp_it)->received)
            {
                incorrect_response_detected = true;
                failover_start = (*resp_it)->time_sent;
                cout << "failover start: " << failover_start << endl;
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
                    if(distance(request_buffer.begin(), x) <= (*results_pre.find((*x)->ptr)).second.pre_failover_ratio)
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
                        cout << "zero downtime" << endl;
                        * terminate = true;
                        return;
                    }
                }
            }
        }
        
        if(incorrect_response_detected)
        {
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
                            cout << "failover finish:" << failover_finish << endl;
                            *failover_len_ms = (int)(failover_finish - failover_start);
                        }
                    }
                }
            }
            else
                correct_responses_after_failover = 0;
            
            if(correct_responses_after_failover >= request_buffer_size)
            {
                * terminate = true;
                return;
            }
        }
        
    }
    
}

void ResponseScan::CountArraySize()
{
    request_buffer_size = 0;
    incorrect_response_detected = false;
    
    for (auto & x : results_pre)
    {
        x.second.pre_failover_ratio = (double)(POSITIVE_REP * totalRequests)/(double)(x.second.count);
        if(x.second.pre_failover_ratio > request_buffer_size)
            request_buffer_size = x.second.pre_failover_ratio;
    }
    inside_buffer = 0;
    
    return;
}

int ResponseScan::GatherData(bool last)//implicit false
{
    ptr_first = responses->begin();

    pthread_mutex_lock(responses_list_mutex);
    ptr_last = prev(responses->end());
    list<Response*>::iterator end = next(ptr_last);
    pthread_mutex_unlock(responses_list_mutex);
    
    //cout << "The distance is: " << std::distance(ptr_first,next(ptr_last)) << '\n';
    for (list<Response*>::iterator resp_it=ptr_first; resp_it != end; ++resp_it)
    {
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
    }

    return 0;
}

