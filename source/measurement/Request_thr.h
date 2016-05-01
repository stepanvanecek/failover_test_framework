//
//  Request_thr.h
//  test_framework
//
//  Created by Stepan Vanecek on 27/04/16.
//  Copyright © 2016 Stepan Vanecek. All rights reserved.
//

#ifndef Request_thr_h
#define Request_thr_h

class Request_thr
{
public:
    pthread_t thread;
    bool active;
    const char * url_ptr;
};


#endif /* Request_thr_h */
