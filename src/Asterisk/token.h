//
// C++ Interface: socket
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKTOKEN_H
#define ASTERISKTOKEN_H

#include <map>
#include <string>

namespace ASTERISK { 

/**
@author 
*/
class Token {
public:
    Token();
    virtual ~Token();

public:
    void setKey(std::string key, std::string value);
    std::string& getKey(std::string key);
    bool hasKey(std::string key);

    void removeAll();
   
    virtual std::string serialize() const;
    virtual int unserialize(std::string str);

private:
    std::map<std::string, std::string> tokmap;
};

};

#endif
