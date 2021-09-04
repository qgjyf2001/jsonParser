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
    JsonParser(std::string* message,Type type=OBJECT,bool checkEnd=true,std::string::iterator *beginPtr=nullptr,std::string::iterator *endPtr=nullptr)
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
        auto begin=!checkEnd?*beginPtr:message->begin();
        auto end=message->end()-1;
        moveForwardPtr<' ','\n'>(begin,message->end());
        if (checkEnd)
            moveBackwardPtr<' ','\n'>(end,message->begin());
        if ((*begin=='{'&&(!checkEnd||*end=='}'))||(*begin=='['&&(!checkEnd||*end==']')))
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
                    if (((*begin==']'&&this->type==ARRAY)||(*begin=='}'&&this->type==OBJECT))&&!checkEnd)
                    {
                        *endPtr=begin+1;
                        return;
                    }
                }
                else if (*begin=='{')
                {
                    JsonParser* newJson=new JsonParser(message,OBJECT,false,&begin,&begin);
                    if (this->type==ARRAY)
                        jsonArray.push_back(newJson);
                    else
                        json[std::string(head,tail)]=newJson;
                }
                else if (*begin=='[')
                {
                    JsonParser* newJson=new JsonParser(message,OBJECT,false,&begin,&begin);
                    if (this->type==ARRAY)
                        jsonArray.push_back(newJson);
                    else
                        json[std::string(head,tail)]=newJson;
                }
                else{
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
                else if ((!((*begin=='}'&&this->type==OBJECT)||(*begin==']'&&this->type==ARRAY)))&&checkEnd)
                    throw  std::runtime_error("illegal syntax");
                else if (((*begin=='}'&&this->type==OBJECT)||(*begin==']'&&this->type==ARRAY))&&!checkEnd)
                {
                    *endPtr=begin+1;
                    return;
                }
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
    void foreach(std::function<void(JsonParser&)> function)
    {
        if (this->type!=ARRAY)
            throw std::runtime_error("illegal operation");
        for (auto obj:jsonArray)
            function(*obj);
    }
    Type type;
    operator std::string()
    {
        if (this->type==STRING)
            return "\""+this->toString()+"\"";
        else if (this->type==INT)
            return std::to_string(this->toInt());
        else if (this->type==ARRAY){
            std::string result="[";
            this->foreach([&](JsonParser& json){
                 if (result=="[")
                          result+=json;
                 else
                          result+=std::string(",")+(std::string)json;
        });
            result+="]";
            return result;
        }
        else{
            std::string result="{";
            for (auto &&it:json)
            {
                if (result=="{")
                    result+="\""+it.first+"\":"+(std::string)*(it.second);
                else
                    result+=std::string(",")+"\""+it.first+"\":"+(std::string)*(it.second);
            }
            result+="}";
            return result;
        }
    }
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