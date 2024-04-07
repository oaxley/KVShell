/*
 * @file    args.h
 * @brief   Header file for Args class
 */

// ----- guards
#ifndef ARGS_H
#define ARGS_H

// ----- includes
#include <string>
#include <vector>


// ----- class
namespace Application
{

    class Args
    {
    public:
        // parse the command line
        void parse(int argc, char* argv[]);

        // return an iterator on the command line arguments
        using Iterator = std::vector<std::string>::const_iterator;
        Iterator begin() const;
        Iterator end() const;

        // number of arguments
        size_t size() const;
        int count() const;
        bool empty() const;


    private:
        std::vector<std::string> args_;         //< command line arguments
    };

}

#endif // ARGS_H