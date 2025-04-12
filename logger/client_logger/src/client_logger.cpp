#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
#include "../include/client_logger.h"
#include <not_implemented.h>
#include <iomanip>

std::unordered_map<std::string, std::pair<size_t, std::ofstream>> client_logger::refcounted_stream::_global_streams;


logger& client_logger::log(
    const std::string &text,
    logger::severity severity) &
{
    std::string output_string = make_format(text, severity);
    auto it = _output_streams.find(severity);
    if (it == _output_streams.end()) {
        return *this;
    }
    if (it->second.second == true) {
        std::cout << output_string << std::endl;
    }
    for (client_logger::refcounted_stream& s : it->second.first) {
        *(s._stream.second) << output_string << std::endl;
    }
    return *this; 
}

std::string client_logger::make_format(const std::string &message, severity sev) const
{
    std::stringstream res;
    size_t pos = 0,  prev_pos = 0;
    while (1) {
        while ((_format[pos] != 0) && (_format[pos] != '%')) {
            pos++;
        }
        res << _format.substr(prev_pos, pos - prev_pos);
        prev_pos = pos;
        if (_format[pos] == 0) {
            break;
        }
        pos++;
        switch (char_to_flag(_format[pos]))
        {
        case client_logger::flag::DATE:
            res << current_date_to_string();
            break;
        case client_logger::flag::TIME:
            res << current_time_to_string();
            break;
        case client_logger::flag::SEVERITY:
            res << severity_to_string(sev);
            break;
        case client_logger::flag::MESSAGE:
            res << message;
            break;
        default:
            res << _format.substr(pos - 1, 2);
            break;
        }
        pos++;
        prev_pos = pos;
    }
    return res.str();
}

client_logger::client_logger(
        const std::unordered_map<logger::severity, std::pair<std::forward_list<refcounted_stream>, bool>> &streams,
        std::string format)
{
    
    _output_streams = streams;
    _format = format;
}

client_logger::flag client_logger::char_to_flag(char c) noexcept
{
    switch (c)
    {
    case 'd': return client_logger::flag::DATE;
    case 't': return client_logger::flag::TIME;
    case 's': return client_logger::flag::SEVERITY;
    case 'm': return client_logger::flag::MESSAGE;
    default:
        return client_logger::flag::NO_FLAG;
    }
}

client_logger::client_logger(const client_logger &other) = default;

client_logger &client_logger::operator=(const client_logger &other) = default;

client_logger::client_logger(client_logger &&other) noexcept = default;

client_logger &client_logger::operator=(client_logger &&other) noexcept = default;

client_logger::~client_logger() noexcept = default;

client_logger::refcounted_stream::refcounted_stream(const std::string &path)
{
    _stream.first = path;
    if (_global_streams.find(path) == _global_streams.end()) {
        std::ofstream s(path, std::ios::app);
        if (!s.is_open()) {
            _stream.first = "";
            std::cout << "fail to open " << path << std::endl;
            return;
        }
        _global_streams.emplace(path, std::make_pair(static_cast<size_t>(1), std::move(s)));

        _stream.second = &(_global_streams[path].second);
    } else {
        _global_streams[path].first ++;
        _stream.second = &(_global_streams[path].second);
    }
}

client_logger::refcounted_stream::refcounted_stream(const client_logger::refcounted_stream &oth)
{
    
    _stream.first = oth._stream.first;
    _global_streams[_stream.first].first ++;
    _stream.second = oth._stream.second;
}

client_logger::refcounted_stream &
client_logger::refcounted_stream::operator=(const client_logger::refcounted_stream &oth)
{
    _global_streams[_stream.first].first --;
    if (_global_streams[_stream.first].first == 0) {
        _global_streams[_stream.first].second.close();
        _global_streams.erase(_stream.first);
    }
    _stream.first = oth._stream.first;
    _global_streams[_stream.first].first ++;
    _stream.second = oth._stream.second;
    return *this;
}

client_logger::refcounted_stream::refcounted_stream(client_logger::refcounted_stream &&oth) noexcept
{
    _stream.first = oth._stream.first;
    _stream.second = oth._stream.second;
    oth._stream.first = "";
    oth._stream.second = NULL;
}

client_logger::refcounted_stream &client_logger::refcounted_stream::operator=(client_logger::refcounted_stream &&oth) noexcept
{
    _global_streams[_stream.first].first --;
    if (_global_streams[_stream.first].first == 0) {
        _global_streams[_stream.first].second.close();
        _global_streams.erase(_stream.first);
    }
    _stream.first = oth._stream.first;
    _stream.second = oth._stream.second;
    oth._stream.first = "";
    oth._stream.second = NULL;
    return *this;
}

client_logger::refcounted_stream::~refcounted_stream()
{
    if (_stream.first == "") {
        return;
    }
    _global_streams[_stream.first].first --;
    if (_global_streams[_stream.first].first == 0) {
        _global_streams[_stream.first].second.close();
        _global_streams.erase(_stream.first);
    }
}
