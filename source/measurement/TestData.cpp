//
//  TestData.cpp
//  test_framework
//
//  Created by Stepan Vanecek on 28/04/16.
//  Copyright © 2016 Stepan Vanecek. All rights reserved.
//

#include "TestData.hpp"


TestData::TestData(const char * url, int max_threads, int min_interval_ms)
{
    terminate = false;
    responses = new list<Response*>();
    this->url = url;
    this->max_threads = max_threads;
    this->min_interval_ms = min_interval_ms;
}

TestData::TestData(Json::Value * root, int max_threads, int min_interval_ms)
{
    terminate = false;
    responses = new list<Response*>();
    this->max_threads = max_threads;
    this->min_interval_ms = min_interval_ms;

    
    if ((*root)["test_url"].isMember("full_url"))
        this->url = (*root)["test_url"]["full_url"].asString().c_str();
    else
    {
        cout << "error with the structure" << endl;
        exit(1);
    }
    
    this->terminate_command = (*root)["failover_trigger"].asString().c_str();
}

TestData::~TestData()
{
    for (list<Response*>::iterator it=responses->begin(); it != responses->end(); ++it)
    {
        delete(*it);
    }
    delete responses;
}