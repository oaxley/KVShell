/*
 * @file    cmdline.h
 * @brief   Header file for the CmdLine class
 */

// ----- guards
#ifndef APPLICATION_CMDLINE_H
#define APPLICATION_CMDLINE_H

// ----- includes
#include <string>
#include <vector>

// ----- typedef
using CmdLine_t = std::vector<std::string>;



// ----- class
namespace Application
{
    // Interface class to handle Args & Options on the command line
    class CmdLine
    {
    public:
        CmdLine() = default;
        ~CmdLine() = default;

        // no copy semantics
        CmdLine(const CmdLine&) = delete;
        CmdLine& operator=(const CmdLine&) = delete;

        // no move semantics
        CmdLine(CmdLine&&) = delete;
        CmdLine& operator=(CmdLine&&) = delete;

        void parse(int argc, char* argv[]);

        // type aliases for better code reading
        using Args_t = std::vector<std::string_view>;
        using Options_t = std::vector<std::string_view>;

        // Options & Arguments from the Command Line
        Options_t& options();
        Args_t& args();


    private:
        CmdLine_t cmdline_;
        Args_t args_;
        Options_t options_;
    };


} //< end namespace

#endif // APPLICATION_CMDLINE_H
