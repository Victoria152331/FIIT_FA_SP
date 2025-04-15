#include <filesystem>
#include <utility>
#include <not_implemented.h>
#include "../include/client_logger_builder.h"
#include <not_implemented.h>

using namespace nlohmann;

logger_builder& client_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity) &
{   
    std::string path = std::filesystem::weakly_canonical(stream_file_path).string();

    client_logger::refcounted_stream new_stream (path);
    if(new_stream._stream.first == "") {
        return *this;
    }

    auto it = _output_streams.find(severity);
    if (it == _output_streams.end()) {
        _output_streams.emplace(severity, std::make_pair(std::forward_list<client_logger::refcounted_stream>(), 0));
        it = _output_streams.find(severity);
    }
    auto& streams = it->second.first;
    for (const auto& stream : streams) {
        if (stream._stream.first == path) {
            return *this;
        }
    }

    streams.emplace_front(new_stream);
    return *this;
}

logger_builder& client_logger_builder::add_console_stream(
    logger::severity severity) &
{
    if (_output_streams.find(severity) == _output_streams.end()) {
        _output_streams[severity] = std::make_pair(std::forward_list<client_logger::refcounted_stream>(), true);
    } else {
        _output_streams[severity].second = true;
    }
    return *this;
}

logger_builder& client_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path) &
{
    std::ifstream config_file(configuration_file_path);
    if (!config_file.is_open()) {
        std::cerr << "fail to open config " << configuration_file_path << std::endl;
        return *this;
    }

    nlohmann::json config_json;
    config_file >> config_json;

    nlohmann::json config_section = config_json;
    if (config_json.contains(configuration_path)) {
        config_section = config_json[configuration_path];
    } else {
        std::cout << "invalid config path " << configuration_path << std::endl;
        return *this;
    }

    if (config_section.contains("format")) {
        _format = config_section["format"];
    }
    logger::severity severeties [] = {logger::severity::trace,
            logger::severity::debug,
            logger::severity::information,
            logger::severity::warning,
            logger::severity::error,
            logger::severity::critical
            };
    for (auto sev : severeties) {
        auto string_sev = logger::severity_to_string(sev);
        if (config_section.contains(string_sev)) {
            parse_severity(sev, config_section[string_sev]);
        }
    }

    return *this;
}

logger_builder& client_logger_builder::clear() &
{
    _format = "%m";
    _output_streams.clear();
    return *this;
}

logger *client_logger_builder::build() const
{
    auto log = new client_logger(_output_streams, _format);
    return log;
}

logger_builder& client_logger_builder::set_format(const std::string &format) &
{
    _format = format;
    return *this;
}

void client_logger_builder::parse_severity(logger::severity sev, nlohmann::json& j)
{
    // Чтение output_files и обработка
    if (j.contains("output_files")) {
        for (const auto& file_path : j["output_files"]) {
            add_file_stream(file_path, sev);
        }
    }

    // Чтение консольной записи
    if (j.contains("console") && j["console"] == true) {
        add_console_stream(sev);
    }
}

logger_builder& client_logger_builder::set_destination(const std::string &format) &
{
    throw not_implemented("logger_builder *client_logger_builder::set_destination(const std::string &format)", "invalid call");
}
