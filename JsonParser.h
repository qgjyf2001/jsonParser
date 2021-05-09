#ifndef JSONPARSER_H
#define JSONPARSER_H
#include <iostream>
#include <map>
#include <vector>
#include <functional>
class JsonParser
{
public:
    enum Type{INT,STRING,OBJECT,ARRAY};
    JsonParser(std::string* message,Type type=OBJECT)
    {
        this->type=type;
        if (type==INT)
        {
            this->messsageInt=std::atoi(message->c_str());
            return;
        }
        if (type==STRING)
        {
            this->messageStr=message;
            return;
        }
        auto begin=message->begin();
        auto end=message->end()-1;
        moveForwardPtr<' ','\n'>(begin,message->end());
        moveBackwardPtr<' ','\n'>(end,message->begin());
        if ((*begin=='{'&&*end=='}')||(*begin=='['&&*end==']'))
        {
            this->type=*begin=='['?ARRAY:OBJECT;
            begin+=1;
            std::string::iterator tail,head;
            while (true)
            {

                if (this->type!=ARRAY)
                {
                moveForwardPtr<' ','\n'>(begin,end);
                if (begin==end)
                    break;
                if (*begin!='"')
                    throw std::runtime_error("illegal syntax");
                head=++begin;
                moveForwardPtr<'"'>(begin,end,false);
                if (*begin!='"')
                    throw std::runtime_error("illegal syntax");
                tail=begin++;
                moveForwardPtr<' ','\n'>(begin,end);
                if (begin==end||*begin!=':')
                    throw std::runtime_error("illegal syntax");
                begin++;
                moveForwardPtr<' ','\n'>(begin,end);
                }
                moveForwardPtr<'{','"','0','1','2','3','4','5','6','7','8','9','['>(begin,end,false);
                if (begin==end)
                {
                    if (this->type==OBJECT)
                        throw std::runtime_error("illegal syntax");
                    else
                        break;
                }
                if (*begin=='"')
                {
                    auto contentHead=++begin;
                    while (true)
                    {
                        moveForwardPtr<'"','\\'>(begin,end,false);
                        if (begin==end)
                            throw  std::runtime_error("illegal syntax");
                        if (*begin=='"')
                            break;
                        begin+=2;
                    }
                    auto contentTail=begin;
                    if (this->type==ARRAY)
                        jsonArray.push_back(new JsonParser(new std::string(contentHead,contentTail),STRING));
                    else
                        json[std::string(head,tail)]=new JsonParser(new std::string(contentHead,contentTail),STRING);
                    begin++;

                }
                else if (*begin=='{')
                {
                    auto contentHead=begin;
                    JsonParser* newJson;
                    while (true)
                    {
                        moveForwardPtr<'}'>(begin,end,false);
                        if (begin==end)
                            throw  std::runtime_error("illegal syntax");
                        begin++;

                        std::string *tempStr;
                        try {
                            tempStr=new std::string(contentHead,begin);
                            newJson=new JsonParser(tempStr);
                        } catch (...) {
                            delete tempStr;
                            continue;
                        }
                        break;
                    }
                    if (this->type==ARRAY)
                        jsonArray.push_back(newJson);
                    else
                        json[std::string(head,tail)]=newJson;
                }
                else if (*begin=='[')
                {
                    auto contentHead=begin;
                    JsonParser* newJson;
                    while (true)
                    {
                        moveForwardPtr<']'>(begin,end,false);
                        if (begin==end)
                            throw  std::runtime_error("illegal syntax");
                        begin++;
                        std::string *tempStr;
                        try {
                            tempStr=new std::string(contentHead,begin);
                            newJson=new JsonParser(tempStr);
                        } catch (...) {
                            delete tempStr;
                            continue;
                        }
                        break;
                    }
                    if (this->type==ARRAY)
                        jsonArray.push_back(newJson);
                    else
                        json[std::string(head,tail)]=newJson;
                }
                else
                {
                    auto contentHead=begin;
                    moveForwardPtr<'0','1','2','3','4','5','6','7','8','9'>(begin,end);
                    auto contentTail=begin;
                    if (this->type==ARRAY)
                        jsonArray.push_back(new JsonParser(new std::string(contentHead,contentTail),INT));
                    else
                        json[std::string(head,tail)]=new JsonParser(new std::string(contentHead,contentTail),INT);
                }

                moveForwardPtr<' ','\n'>(begin,end);
                if (*begin==',')
                    begin++;
                else if (!((*begin=='}'&&this->type==OBJECT)||(*begin==']'&&this->type==ARRAY)))
                    throw  std::runtime_error("illegal syntax");
                if (*begin==',')
                    begin++;
            }
        }
        else
        {
            throw std::runtime_error("illegal syntax");
        }
    }
    JsonParser &operator [](std::string index)
    {
        return *json[index];
    }
    std::string toString()
    {
        if (this->type!=STRING)
            throw std::runtime_error("illegal operation");
        return *messageStr;
    }
    int toInt()
    {
        if (this->type!=INT)
            throw std::runtime_error("illegal operation");
        return messsageInt;
    }
    void foreach(std::function<void(JsonParser)> function)
    {
        if (this->type!=ARRAY)
            throw std::runtime_error("illegal operation");
        for (auto obj:jsonArray)
            function(*obj);
    }
    Type type;
private:
    template<char... ch>
    static inline void moveForwardPtr(std::string::iterator& pos,std::string::iterator end,bool equal=true)
    {
        while (pos!=end&&(((*pos==ch))||...)^(!equal))
            pos++;
    }
    template<char... ch>
    static inline void moveBackwardPtr(std::string::iterator& pos,std::string::iterator begin,bool equal=true)
    {
        while (pos!=begin&&(((*pos==ch))||...)^(!equal))
            pos--;
    }
    std::map<std::string,JsonParser*> json;
    std::vector<JsonParser*> jsonArray;
    std::string* messageStr=nullptr;
    int messsageInt;
};
#endif // JSONPARSER_H
