/**
 * parser_worker_driver.cc
 */

#include <getopt.h>

#include <iostream>
#include <memory>
#include <signal.h>

#include <Parser.h>
#include <ParserCommon.h>
#include <ParserFactory.h>
#include <ParserWorker.h>

namespace BlackLibraryParsers = black_library::core::parsers;

struct options
{
    BlackLibraryParsers::parser_t source;
};

BlackLibraryParsers::ParserWorker *parser_worker;

static void Usage(const char *prog)
{
    const char *p = strchr(prog, '/');
    printf("usage: %s --(s)ource_target [-h]\n", p ? (p + 1) : prog);
}

static int ParseOptions(int argc, char **argv, struct options *opts)
{
    static const char *const optstr = "hs:";
    static const struct option long_opts[] = {
        { "help", no_argument, 0, 'h' },
        { "source_target", required_argument, 0, 's' },
        { 0, 0, 0, 0 }
    };

    if (!argv || !opts)
        return -1;

    int opt = 0;
    while ((opt = getopt_long(argc, argv, optstr, long_opts, 0)) >= 0)
    {
        switch (opt)
        {
            case 'h':
                Usage(argv[0]);
                exit(0);
                break;
            case 's':
                if (std::string(optarg) == "ao3")
                {
                    opts->source = BlackLibraryParsers::parser_t::AO3_PARSER;
                }
                else if (std::string(optarg) == "rr")
                {
                    opts->source = BlackLibraryParsers::parser_t::RR_PARSER;
                }
                else if (std::string(optarg) == "sbf")
                {
                    opts->source = BlackLibraryParsers::parser_t::SBF_PARSER;
                }
                else
                {
                    std::cout << "Could not match source" << std::endl;
                    Usage(argv[0]);
                    exit(1);
                }
                break;
            default:
                exit(1);
                break;
        }
    }

    if (optind < argc)
    {
        fprintf(stderr, "trailing options..\n");
        exit(1);
    }

    return 0;
}

void SigHandler(int sig)
{
    std::cout << "parser_worker_driver stop" << std::endl;

    if (sig == SIGTERM || sig == SIGINT)
        parser_worker->Stop();
}

int main(int argc, char* argv[])
{
    struct options opts;

    if (argc < 2)
    {
        Usage(argv[0]);
        exit(1);
    }

    if (ParseOptions(argc, argv, &opts))
    {
        Usage(argv[0]);
        exit(1);
    }

    signal(SIGINT, SigHandler);
    signal(SIGTERM, SigHandler);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    using dummy_worker = BlackLibraryParsers::ParserWorker;

    auto factory = std::make_shared<BlackLibraryParsers::ParserFactory>();

    dummy_worker dummy_worker_0(factory, "/mnt/black-library/store", BlackLibraryParsers::parser_t::ERROR_PARSER, 3);

    if (opts.source == BlackLibraryParsers::parser_t::ERROR_PARSER)
    {
        std::cout << "could not match parser source" << std::endl;
    }

    dummy_worker dummy_worker_1(factory, "/mnt/black-library/store", opts.source, 2);

    parser_worker = &dummy_worker_1;

    BlackLibraryParsers::ParserJob job_0;
    BlackLibraryParsers::ParserJob job_1;
    BlackLibraryParsers::ParserJob job_2;

    job_0.uuid = "some-uuid-0";
    job_1.uuid = "some-uuid-1";
    job_2.uuid = "some-uuid-2";

    if (opts.source == BlackLibraryParsers::parser_t::AO3_PARSER)
    {
        job_0.url = "";
        job_1.url = "";
        job_2.url = "";
    }
    else if (opts.source == BlackLibraryParsers::parser_t::RR_PARSER)
    {
        job_0.url = "https://www.royalroad.com/fiction/21220/mother-of-learning";
        job_1.url = "https://www.royalroad.com/fiction/17731/i-never-wanted-you-dead";
        job_2.url = "https://www.royalroad.com/fiction/16946/azarinth-healer";
    }
    else if (opts.source == BlackLibraryParsers::parser_t::SBF_PARSER)
    {
        job_0.url = "https://forums.spacebattles.com/threads/be-thou-my-good.867883/";
        job_1.url = "https://forums.spacebattles.com/threads/new-operational-parameters.815612/";
        job_2.url = "https://forums.spacebattles.com/threads/commander-pa-multicross-si.309838/";
    }
    else if (opts.source == BlackLibraryParsers::parser_t::SVF_PARSER)
    {
        job_0.url = "https://forums.sufficientvelocity.com/threads/scientia-weaponizes-the-future.82203/";
        job_1.url = "https://forums.sufficientvelocity.com/threads/zero-summing-foz-skyrim.19118/";
        job_2.url = "https://forums.sufficientvelocity.com/threads/in-nuclear-fire.67755/";
    }
    else
    {
        std::cout << "could not match parser source" << std::endl;
        Usage(argv[0]);
        exit(1);
    }

    parser_worker->RegisterProgressNumberCallback(
        [](const std::string &uuid, size_t progress_num, bool error)
        {
            std::cout << "ProgressNumberCallback: " << uuid << " - " << progress_num << " - " << error << std::endl;
        }
    );
    parser_worker->RegisterJobStatusCallback(
        [](const BlackLibraryParsers::ParserJob &parser_job, BlackLibraryParsers::job_status_t job_status)
        {
            std::cout << "JobStatusCallback: " << parser_job << " - " << BlackLibraryParsers::GetStatusName(job_status) << std::endl;
        }
    );
    parser_worker->RegisterManagerNotifyCallback(
        [](BlackLibraryParsers::ParserJobResult result)
        {
            std::cout << "ManagerNotifyCallback: " << result.metadata.uuid << " - " << result.metadata.url << std::endl;
        }
    );

    parser_worker->AddJob(job_0);
    parser_worker->AddJob(job_1);
    parser_worker->AddJob(job_2);

    parser_worker->Run();

    curl_global_cleanup();

    std::cout << "Tester exit successful" << std::endl;

    return 0;
}
