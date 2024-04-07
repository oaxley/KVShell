/*
 * @file    args.cpp
 * @brief   Source file for Args class
 */

// ----- includes
#include "args.h"


// ----- methods definition

// parse the command line arguments
void Application::Args::parse(int argc, char* argv[])
{
    std::vector<std::string> tmp(argv + 1, argv + argc);
    args_ = tmp;
}

// return the begin iterator
Application::Args::Iterator Application::Args::begin() const
{
    return args_.begin();
}

// return the end iterator
Application::Args::Iterator Application::Args::end() const
{
    return args_.end();
}

// return the size of the underlying container
size_t Application::Args::size() const
{
    return args_.size();
}

// return the number of element inside the container
int Application::Args::count() const
{
    return static_cast<int>(size());
}

// true if the container is empty
bool Application::Args::empty() const
{
    return args_.empty();
}