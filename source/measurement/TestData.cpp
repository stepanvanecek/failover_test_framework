//
//  TestData.cpp
//  test_framework
//
//  Created by Stepan Vanecek on 28/04/16.
//  Copyright © 2016 Stepan Vanecek. All rights reserved.
//

#include <fstream>

#include "TestData.hpp"

int TestData::ParseInputFile(const char * tmp_file)
{
    ifstream file(tmp_file);
    if (!file) {
        cerr << "File not found." << endl;
        return 0;
    }
    
    if(!getline(file, url))
        return 0;
    
    if(!getline(file, c.terminate_command))//TODO multiline scan
        return 0;

    if(!getline(file, c.os_username))
        return 0;

    if(!getline(file, c.os_tenant_name))
        return 0;
    
    if(!getline(file, c.out_file))
        return 0;
    
    if(!getline(file, c.id))
        return 0;
    
    if(!getline(file, c.ts))
        return 0;
    
    if(!getline(file, c.notes))//TODO multiline scan
        return 0;

    string value;
    if(!getline(file, value))
        return 0;
    min_interval_ms = atoi(value.c_str());
    
    cout << url << c.terminate_command << c.os_username << c.os_tenant_name << c.os_password << endl;
    
    return 1;
}

TestData::TestData(const char ** argv)
{
    terminate = false;
    responses = new list<Response*>();
    
    c.os_password = argv[1];
    if(!ParseInputFile(argv[2]))
    {
        printf("Problem with reading tmp file.\n");
        exit(1);
    }
//    this->url = url;
//    this->min_interval_ms = min_interval_ms;
}

TestData::TestData(const char * url, int min_interval_ms)
{
    terminate = false;
    responses = new list<Response*>();
    //this->url = url;
    this->min_interval_ms = min_interval_ms;
}

TestData::~TestData()
{
    for (list<Response*>::iterator it=responses->begin(); it != responses->end(); ++it)
    {
        delete(*it);
    }
    delete responses;
}

//
//TestData::TestData(Json::Value root, int min_interval_ms)
//{
//    terminate = false;
//    responses = new list<Response*>();
//    this->min_interval_ms = min_interval_ms;
//
//
//    if (root["test_url"].isMember("full_url"))
//    {
//        this->url = root["test_url"]["full_url"].asString().c_str();
//        printf("url: %s\n", url);
//    }
//    else
//    {
//        cout << "error with the structure" << endl;
//        exit(1);
//    }
////
////    terminate_command = (*root)["failover_trigger"].asString().c_str();
////    //printf("terminate_command: %s\n", terminate_command);
////    os_username = (*root)["creds"]["os_username"].asString().c_str();
////    //printf("os_username: %s\n", os_username);
////    os_tenant_name = (*root)["creds"]["os_tenant_name"].asString().c_str();
////    //printf("os_tenant_name: %s\n", os_tenant_name);
////    os_password = (*root)["creds"]["os_password"].asString().c_str();
////    printf("os_password: %s\n", os_password);
//}