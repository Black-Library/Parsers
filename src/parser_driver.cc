/*extern "C" {
    #include <curl/curl.h>
}*/

#include <Parser.h>
//#include "ParsersConfig.h"

#include <ParserAO3.h>

int main(int argc, char* argv[])
{
    // check to see if there are any extra arguments
    if (argc > 1)
    {
        std::cout << "extra arguments ";
        for (int i = 0; i < argc; ++i)
        {
            std::cout << std::string(argv[i]);
        }
        std::cout << std::endl;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);

    librarycore::AO3::ParserAO3 parser;
    parser.SetUrl("https://archiveofourown.org/works/505809");
    parser.Parse();

    librarycore::Parser parser2;
    parser2.SetUrl("https://archiveofourown.org/works/505809");
    parser2.Parse();

    curl_global_cleanup();

    std::cout << "Tester exit successful" << std::endl;

    return 0;
}