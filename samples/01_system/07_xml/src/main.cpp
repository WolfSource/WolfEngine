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
    const wchar_t* _name = L"07_xml";
    WOLF_INIT(_name);

	logger.write(L"Wolf initialized");
    
    //create root node
    w_xml_data _root;
    _root.node = L"peoples";
    
    
    //create first node
    w_xml_data _node_0;
    
    //assign attributes to first node
    w_xml_data::w_xml_data_attribute _attr;
    _attr.name = L"name";
    _attr.value = L"pooya";
    _node_0.attributes.push_back(_attr);
    
    _attr.name = L"age";
    _attr.value = L"31";
    _node_0.attributes.push_back(_attr);
    
    //add to the root node
     _node_0.node = L"people";
    _root.children.push_back(_node_0);
    
    //create second node
    w_xml_data _node_1;
    _attr.name = L"name";
    _attr.value = L"rayan";
    _node_1.attributes.push_back(_attr);
    
    _attr.name = L"age";
    _attr.value = L"0";
    _node_1.attributes.push_back(_attr);
    
     _node_1.node = L"people";
    _root.children.push_back(_node_1);
    
    bool _save_as_utf8 = false;
    
#if defined(__WIN32) || defined(__UWP)
    _save_as_utf8 = true;
    const wchar_t* _path = L"test.xml";
#else
    const char* _path = "test.xml";
#endif
    
    if(w_xml::save(_path, _save_as_utf8, _root, L"<!-- Sample XML -->") == S_OK)
    {
            //Successfully saved, now load it again
            std::ifstream _file("test.xml");
            std::stringstream _string_stream;
            _string_stream << _file.rdbuf();
            _file.close();
            std::string _content(_string_stream.str());
        
            using namespace rapidxml;
            xml_document<> _doc;
            try
            {
                _doc.parse<0>(&_content[0]);
                auto _node = _doc.first_node();
                if (_node)
                {
                    logger.write(_node->name());
                    for (auto _child = _node->first_node(); _child != nullptr; _child = _child->next_sibling())
                    {
                        logger.write(_child->name());
                    }
                }
                _doc.clear();
            }
            catch (...)
            {
                logger.error(L"Could not parse xml file");
            }

    }
    else
    {
            logger.write(L"failed to save xml");
    }
    
    
    logger.write(L"shutting down Wolf");

    logger.release();

	return EXIT_SUCCESS;
}
