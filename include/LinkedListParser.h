/**
 * LinkedListParser.h
 */

#ifndef __BLACK_LIBRARY_CORE_PARSERS_LINKED_LIST_PARSER_H__
#define __BLACK_LIBRARY_CORE_PARSERS_LINKED_LIST_PARSER_H__

#include "Parser.h"

namespace black_library {

namespace core {

namespace parsers {

class LinkedListParser : public Parser
{
public:
    explicit LinkedListParser(parser_t parser_type);

protected:
    virtual void ParseLoop(ParserResult &parser_result);
    virtual std::string PreprocessTargetUrl(const std::string &job_url);
    virtual void SaveLastUrl(ParserResult &parser_result);
    virtual void SaveUpdateDate(ParserResult &parser_result);

    std::string last_url_;
    std::string next_url_;
    time_t last_update_date_;
    bool reached_end_;
};

} // namespace parsers
} // namespace core
} // namespace black_library

#endif
