//
//  Response.cpp
//  test_framework
//
//  Created by Stepan Vanecek on 27/04/16.
//  Copyright © 2016 Stepan Vanecek. All rights reserved.
//

#include "Response.hpp"
#include <iostream>

Response::Response()
{
    this->len = 0;
    this->ptr = (char*)malloc(this->len+1);
    if (this->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        //exit(EXIT_FAILURE);
    }
    this->ptr[0] = '\0';
    this->received = true;
    this->time_received = -1;
}

Response::~Response()
{
    delete ptr;
}