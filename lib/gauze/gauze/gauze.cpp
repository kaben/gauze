#include <cstdlib> // std::getenv

#include <iostream> // std::cerr

#include <boost/predef.h> // BOOST_OS_IOS

// clang-format off
#if (BOOST_OS_IOS)
#  include <regex>    // std::regex
#  include <unistd.h> // chdir
#endif
// clang-format on

// Should be defined by user
int gauze_main(int argc, char** argv);

int main(int argc, char** argv)
{
    try
    {
#if (BOOST_OS_IOS)
        const char* home = std::getenv("HOME");
        if (home == nullptr)
        {
            std::cerr << "HOME not found" << std::endl;
            return EXIT_FAILURE;
        }

        std::string working_dir(home);
        working_dir += "/tmp";

        if (chdir(working_dir.c_str()) != 0)
        {
            std::cerr << "Can't change working directory" << std::endl;
            return EXIT_FAILURE;
        }

        std::vector<char*> new_argv(argc);
        const std::regex r("\\$<GAUZE_RESOURCE_FILE:(.*)>$");

        std::string fmt(home);
        fmt += "/\\1";

        std::vector<std::string> extra_args;

        for (int i = 0; i < argc; ++i)
        {
            const std::string original(argv[i]);
            const std::string x = std::regex_replace(original, r, fmt, std::regex_constants::format_sed);
            if (original == x)
            {
                new_argv[i] = argv[i];
            }
            else
            {
                extra_args.push_back(x);
                new_argv[i] = const_cast<char*>(extra_args.back().c_str());
            }
        }

        return gauze_main(argc, new_argv.data());
#else
        // If platform is not iOS - leave as is
        return gauze_main(argc, argv);
#endif
    }
    catch (std::exception& exc)
    {
        std::cerr << "Exception caught: " << exc.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "Unknown exception caught" << std::endl;
        return EXIT_FAILURE;
    }
}
