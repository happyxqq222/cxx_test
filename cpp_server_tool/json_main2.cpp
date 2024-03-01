//
// Created by Administrator on 2024-02-26.
//
#include <iostream>
#include <JSONObject.hpp>
#include "print.h"
using namespace std;

static constexpr std::string_view legal_escaped = " \r\n\t\v\f\0";

int main()
{
    std::string json_text = R"(
                {
                    " name": "southernbrid" ,
                    "age": 14,
                    "gender": true,
                    "height": 1.65,
                    "grade": null,
                    "skills": [
                        "JavaScript"   ,
                        "Java",
                        "Python",
                        "Lisp"
                    ],
                    "obj": {
                        "kk": {
                            "abcd": "def",
                            "ll": "jjk"
                        },
                        "array1": ["t1"  , "t2", "t3"]
                    }
                }
    )";

    pair<JSONObject, size_t> p  =  JSONObject::parse(json_text);
    print(p.first);


    static constexpr std::string_view escaped2 = " \r\n\t\v\f\0";
    cout <<(escaped2.find('a') == escaped2.npos );

    return 0;
}
