#include <iostream>
#include <JsonParser.h>
using namespace std;

int main()
{
    auto json=JsonParser(new std::string(R"(
                                         {
                                            "name":"jyf{[\"wycfather\"]}",
                                            "child":"wyc",
                                            "age":12,
                                            "json":{"address":"qd","data":["num1","num2","num3",{"num4":["1"]}]},
                                            "childsSoulMate":"zxy"
                                         }
                                         )"));

    std::cout<<json["childsSoulMate"].toString()<<std::endl;
    json["json"]["data"].foreach([](JsonParser e){
    if (e.type==JsonParser::STRING)
        std::cout<<e.toString()<<std::endl;});
    return 0;
}
