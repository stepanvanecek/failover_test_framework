//
//  TestData.cpp
//  test_framework
//
//  Created by Stepan Vanecek on 28/04/16.
//  Copyright © 2016 Stepan Vanecek. All rights reserved.
//

#include <fstream>

#include "TestData.hpp"



int TestData::ParseInputJson(string tmp_file)
{
    config = new Json::Value();
    Json::Reader reader;
    ifstream test(tmp_file, std::ifstream::binary);
    bool parsingSuccessful = reader.parse( test, * config, false );
    if ( !parsingSuccessful )
    {
        std::cout  << reader.getFormatedErrorMessages() << "\n";
        exit(1);
    }
    
    //TODO try catch
    url = (*config)["test_url"]["full_url"].asString();
    terminate_command = (*config)["failover_trigger"].asString();
    min_interval_ms = (*config)["precision_ms"].asInt();
    if(config->isMember("timeout"))
        timeout = (*config)["timeout"].asInt() * 1000;
    else
        timeout = 300000;
    
    return 1;
}
TestData::TestData(const char ** argv)
{
    terminate = false;
    timeout_reached = false;
    responses = new list<Response*>();
    os_password = argv[1];
    string pathToTmp = argv[2];
    if(!ParseInputJson(pathToTmp))
    {
        printf("Problem with reading tmp file.\n");
        exit(1);
    }
}

TestData::TestData(const char * url, int min_interval_ms)
{
    terminate = false;
    responses = new list<Response*>();
    this->min_interval_ms = min_interval_ms;
}

TestData::~TestData()
{
    for (list<Response*>::iterator it=responses->begin(); it != responses->end(); ++it)
    {
        delete(*it);
    }
    delete responses;
    delete config;
}


int TestData::ParseInputFile(const char * tmp_file)
{
    return 1;
}
