/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to interact lua as script engine of Wolf Engine
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"

//namespaces
using namespace wolf;
using namespace wolf::system;

//Entry point of program 
WOLF_MAIN()
{
    const wchar_t* _name = L"08_json";
    logger.initialize(_name, wolf::system::io::get_current_directoryW());

    //use rapid json to make json
    using namespace rapidjson;
    StringBuffer _string_buffer;
    Writer<StringBuffer> _writer(_string_buffer);
    
    _writer.StartObject();
    {
    _writer.Key("name");
    _writer.String("pooya");
    _writer.Key("male");
    _writer.Bool(true);
    _writer.Key("age");
    _writer.Uint(31);
    _writer.Key("friends");
    _writer.StartArray();
    _writer.String("ray");
    _writer.String("rayan");
    _writer.String("sib");
    _writer.String("barf");
    _writer.EndArray();
    _writer.Key("NULL");
    _writer.Null();
    }
    _writer.EndObject();
    
    //output json
    auto _size = _string_buffer.GetSize();
    auto _str = _string_buffer.GetString();
    logger.write(_str);
    
    //read it again
    Document _doc;

    auto _buffer = new char[_size];
    memcpy(&_buffer[0], &_str[0], _size);

	auto _parse = &_doc.Parse<rapidjson::kParseStopWhenDoneFlag>(_buffer);
    if (_parse->HasParseError())
    {
        logger.error("error on parsing json. error code: " + std::to_string(_parse->GetParseError()));
    }
    else
    {
        if(_doc.HasMember("name"))
        {
            if (_doc["name"].IsString())
            {
                logger.write(_doc["name"].GetString());
            }
        }
        
    }
    
    logger.release();

	return EXIT_SUCCESS;
}
