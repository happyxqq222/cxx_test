//
// Created by Administrator on 2024-02-26.
//

#ifndef CPP_SERVER_TOOL_JSONOBJECT_HPP
#define CPP_SERVER_TOOL_JSONOBJECT_HPP
#include <variant>
#include <vector>
#include <unordered_map>
#include <string>
#include <charconv>
#include <regex>
#include <string_view>
#include "print.h"


class JSONObject;
using JSONDict = std::unordered_map<std::string,JSONObject>;
using JSONArray = std::vector<JSONObject>;


class JSONObject{

private:
    using VariantType =std::variant<std::nullptr_t,bool,int,double,std::string,JSONArray,JSONDict>;
    using Key_Type = std::tuple<std::string,size_t>;
private:
    VariantType inner;
    enum class Phase{
        Raw,
        Escaped
    };
    static constexpr std::string_view legal_escaped = " \r\n\t\v\f\0";
public:

    JSONObject(){
    }

    template<typename T, typename = std::enable_if_t<std::is_convertible_v<T,VariantType>>>
    JSONObject(T&& t) : inner(std::forward<T>(t)){
    }

    JSONObject(JSONObject&& other) : inner(std::move(other.inner)){
    }

    void do_print() const{
        print(inner);
    };
    template<class T>
    bool is() const{
        return std::holds_alternative<T>(inner);
    };

    template<class T>
    T const& get() const{
        return std::get<T>(inner);
    }

    template<class T>
    T& get(){
        return std::get<T>(inner);
    }
    /**
     * json parse method
     * @param json   json str
     * @return
     */
    static std::pair<JSONObject,size_t> parse(std::string_view json);

private:
    template<typename T>
    static std::optional<T> try_parse_num(std::string_view str);


    static size_t illegalCharaceter(std::string_view json);

    static char unescaped_char(char c);

};


template<typename T>
std::optional<T> JSONObject::try_parse_num(std::string_view str) {
    T value;
    auto [ptr,ec] = std::from_chars(str.data(),str.data()+str.length(), value);
    if(ec == std::errc() && ptr == str.data() + str.length()){
        return value;
    }
    return std::nullopt;
}

std::pair<JSONObject, size_t> JSONObject::parse(std::string_view json) {
    if(json.empty()) {
        return {JSONObject{nullptr}, 0};
    }else if(size_t off = json.find_first_not_of(" \r\n\t\v\f\0"); off != 0 && off != json.npos){    // remove unused character
        auto[jsonObj,eaten] = parse(json.substr(off));
        return {std::move(jsonObj),eaten+off};
    }else if(json.find_first_of("true") == 0 || json.find_first_of("false") == 0 ) {   //handle bool type
        if(json.find_first_of("true") == 0){
            return {JSONObject(true),4};
        }else{
            return {JSONObject(false),5};
        }
    }else if(json.find_first_of("null") == 0){    //handle null type
        return {JSONObject(nullptr),4};
    }else if(json[0] >= '0' && json[0] <= '9' || json[0] == '+' || json[0] == '-') {    //parse int type or float type
        std::regex num_re{"[+-]?[0-9]+(\\.[0-9]*)?([eE][+-]?[0-9]+)?"};
        std::cmatch match;
        if (std::regex_search(json.data(), json.data() + json.size(), match, num_re)) {
            std::string str = match.str();
            if (auto num = try_parse_num<int>(str)) {
                return {JSONObject{*num}, str.size()};
            }
            if (auto num = try_parse_num<double>(str)) {
                return {JSONObject{*num}, str.size()};
            }
        }
    }else if(json[0] == '{'){ //parse objct
        JSONDict  dict;
        size_t i;
        for(i = 1; i < json.size();){
            std::string_view subJson = json.substr(i);

            //parse dict's key
            std::string_view  keySv = json.substr(i);
            auto[keyObj,keyEaten] =  parse(json.substr(i));
            if(keyEaten == 0){
                i = 0;
                break;
            }
            if(!std::holds_alternative<std::string>(keyObj.inner)){
                i = 0;
                break;
            }
            std::string key = std::get<std::string>(keyObj.inner);
            i = i +keyEaten;


            if(json[i] == ':'){
                i = i +1;
            }

            //parse dict's value
            std::string_view  tmpSv = json.substr(i);
            auto[valueObj,valueEaten] = parse(tmpSv);
            if(valueEaten == 0){
                i = 0;
                break;
            }
            i = i + valueEaten;

            dict.try_emplace(std::move(key),std::move(valueObj));

            size_t index = json.substr(i).find_first_not_of(legal_escaped);
            if(json[i+index] == ','){
                i = i+index +1 ;
            }else if(json[i+index] == '}'){
                i = i+index+1;
                break;
            }else{
                std::string_view sv = json.substr(i);
                std::cout << sv;
                i = 0;
                break;
            }
        }
        if(i == 0 || dict.empty() ){
            return {JSONObject{nullptr},0};
        }else{
            return {JSONObject{std::move(dict)},i};
        }
    }else if(json[0] == '"'){
        std::string str;
        size_t i;
        Phase phase = Phase::Raw;
        for(i = 1; i < json.length();i++){
            char ch = json[i];
            if(phase == Phase::Raw){
                if(ch == '\\'){
                    phase = Phase::Escaped;
                    continue;
                } else if (ch == '"'){
                    //over;
                    i = i +1;
                    break;
                }else{
                    str += ch;
                }
            }else if(phase == Phase::Escaped){
                str += unescaped_char(ch);
            }
        }
        return {JSONObject{std::move(str)},i};
    }else if(json[0] == '['){   //handle array
        JSONArray  array;
        size_t i;
        for(i = 1; i < json.length();){
            if (json[i] == ']') {
                i += 1;
                break;
            }
            auto [arrayItemObject,eaten] = parse(json.substr(i));
            if(eaten == 0){
                i = 0;
                break;
            }
            array.push_back(std::move(arrayItemObject));
            i += eaten;

            size_t index = json.substr(i).find_first_not_of(legal_escaped);
            if(json[i+index] == ','){
                i = i+index +1 ;
            }else if(json[i+index] == ']'){
                i = i+index+1;
                break;
            }else{
                i = 0;
                break;
            }
        }
        return {JSONObject{std::move(array)}, i};

    }
    return {JSONObject{nullptr},0};
}

char JSONObject::unescaped_char(char c){
    switch(c){
        case 'n': return '\n';
        case 'r': return '\r';
        case '0': return '\0';
        case 't': return '\t';
        case 'v': return '\v';
        case 'f': return '\f';
        case 'b': return '\b';
        case 'a': return '\a';
        default: return c;
    }
}

template<class ...Fs>
struct overloaded : Fs...{
    using Fs::operator()...;
};

template<class ...Fs>
overloaded(Fs...) -> overloaded<Fs...>;



#endif //CPP_SERVER_TOOL_JSONOBJECT_HPP
