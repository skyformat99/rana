#pragma once

#include <fstream>
#include <sstream>

#include "rana.hpp"

namespace rana {
    
class printer
{
public:
    printer();
    
    printer &encoding(rana::encoding e) { encoding_ = e; return *this; }
    rana::encoding get_encoding() const { return encoding_; }
    printer &ascii(bool enable) { encoding_ = enable ? encoding::ascii : encoding::utf8; return *this; }
    printer &latin1(bool enable) { encoding_ = enable ? encoding::latin1 : encoding::utf8; return *this; }
    printer &utf8(bool enable) { encoding_ = enable ? encoding::utf8 : encoding::ascii; return *this; }
    printer &pretty(bool enable) { indent(enable); space_after(enable); space_before(enable); return *this; }
    printer &indent(bool enable) { indent_ = enable; return *this; }
    printer &indent_length(std::size_t length) { indent_length_ = length; return *this; }
    printer &space_before(bool enable) { space_before_ = enable; return *this; }
    printer &space_after(bool enable) { space_after_ = enable; return *this; }
    
    void to_file(const value &v, const std::string &filename) { std::ofstream stream(filename); to_stream(v, stream); }
    std::string to_string(const value &v) { std::stringstream ss; to_stream(v, ss); return ss.str(); }
    void to_stream(const value &v, std::ostream &stream) { write(v, stream, 0); }
    
private:
    void write(const value &v, std::ostream &stream, std::size_t depth, const std::string &key = "", bool has_key = false)
    {
        std::string full_indent_string;
        
        if(indent_)
        {
            if(indent_string_.size() != indent_length_)
            {
                indent_string_ = std::string(indent_length_, ' ');
            }
        
            for(std::size_t i = 0; i < depth; i++)
            {
                full_indent_string.append(indent_string_);
            }
        
            stream << full_indent_string;
        }
        
        if(has_key)
        {
            stream << "\"" << key << "\"";
            
            if(space_before_)
            {
                stream << " ";
            }
            
            stream << ":";
            
            if(space_after_)
            {
                stream << " ";
            }
        }
        
        if(!v.is_container())
        {
            stream << v;
            return;
        }

        std::size_t count = 0;
        
        if(v.is_array())
        {
            stream << "[";
            
            if(indent_)
            {
                stream << std::endl;
            }
            
            for(const auto &element : v.iter_array())
            {
                write(element, stream, depth + 1);
                
                if(count++ < v.size() - 1)
                {
                    stream << ",";
                    
                    if(space_after_)
                    {
                        stream << " ";
                    }
                }
                
                if(indent_)
                {
                    stream << std::endl;
                }
            }
            
            if(indent_)
            {
                stream << full_indent_string;
            }
            
            stream << "]";
        }
        else
        {
            stream << "{";
            
            if(indent_)
            {
                stream << std::endl;
            }
            
            for(const auto &key_value_pair : v.iter_object())
            {
                write(key_value_pair.second, stream, depth + 1, key_value_pair.first, true);
                
                if(count++ < v.size() - 1)
                {
                    stream << ",";
                    
                    if(space_after_)
                    {
                        stream << " ";
                    }
                }
                
                if(indent_)
                {
                    stream << std::endl;
                }
            }
            
            if(indent_)
            {
                stream << full_indent_string;
            }
            
            stream << "}";
        }
    }
    
    rana::encoding encoding_;
    bool indent_;
    std::size_t indent_length_;
    bool space_before_;
    bool space_after_;
    
    std::string indent_string_;
};

inline void to_file(const value &v, const std::string &filename)
{
    printer().to_file(v, filename);
}

} // namespace rana