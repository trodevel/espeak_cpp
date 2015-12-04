#include <iostream>         // cout
#include <map>              // std::map

#include "espeak_cpp.h"     // ESpeakCpp

int main()
{
    std::map<std::string, std::string> texts =
    {
            { "en",     "hello world" },
            { "mb-en1", "hello world" },
            { "mb-us1", "hello world" },
            { "de",     "hallo welt" },
            { "mb-de4", "hallo welt" },
            { "mb-de5", "hallo welt" },
            { "mb-de6", "hallo welt" },
            { "mb-de7", "hallo welt" },
            { "ru",     "привет мир" },
    };

    espeak_cpp::ESpeakCpp g;

    std::string error;

    for( auto & s : texts )
    {
        bool b = g.say( s.second, "test_" + s.first + ".wav", s.first, error );

        if( b == false )
            std::cout << "ERROR: failed to generate text for lang '" << s.first
            << "', error " << error << std::endl;
        else
            std::cout << "OK: generated for '" << s.first << "'" << std::endl;
    }

    return 0;
}
