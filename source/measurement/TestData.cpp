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

TestData::~TestData()
{
    for (list<Response*>::iterator it=responses->begin(); it != responses->end(); ++it)
    {
        delete(*it);
    }
}