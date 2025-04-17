#include <not_implemented.h>
#include "../include/server_logger_builder.h"

logger_builder& server_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity) &
{
    auto it = _output_streams.find(severity);
    if (it == _output_streams.end()) {
        _output_streams[severity] = {stream_file_path, false};
    } else {
        it->second.first = stream_file_path;
    }
    return *this;
}

logger_builder& server_logger_builder::add_console_stream(
    logger::severity severity) &
{
    auto it = _output_streams.find(severity);
    if (it == _output_streams.end()) {
        _output_streams[severity] = {"0", true};
    } else {
        it->second.second = true;
    }
    return *this;
}

logger_builder& server_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path) &
{
    throw not_implemented("logger_builder* server_logger_builder::transform_with_configuration(std::string const &configuration_file_path, std::string const &configuration_path)", "your code should be here...");
}

logger_builder& server_logger_builder::clear() &
{
    _destination = "http://127.0.0.1:9200";
    _output_streams.clear();
    return *this;
}

logger *server_logger_builder::build() const
{
    auto *new_logger = new server_logger(_destination, _output_streams);
    return new_logger;
}

logger_builder& server_logger_builder::set_destination(const std::string& dest) &
{
    _destination = dest;
    return *this;
}

logger_builder& server_logger_builder::set_format(const std::string &format) &
{
    throw not_implemented("logger_builder& server_logger_builder::set_format(const std::string &) &", "your code should be here...");
}
