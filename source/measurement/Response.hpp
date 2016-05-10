//
//  Response.hpp
//  test_framework
//
//  Created by Stepan Vanecek on 27/04/16.
//  Copyright © 2016 Stepan Vanecek. All rights reserved.
//

#ifndef Response_hpp
#define Response_hpp

#include <stdio.h>

class Response
{
public:
    Response();
    ~Response();
    char *ptr;
    size_t len;
    long int time_sent;
    long int time_received;
    bool received;
};

#endif /* Response_hpp */
