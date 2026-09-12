#include "json.h"

#include <cassert>
#include <iostream>
#include <string>

static void reject(const std::string& text) {
    json::Document value;
    std::string error;
    assert(!json::Document::parse(text, value, error));
    assert(!error.empty());
}

int main() {
    json::Document value;
    std::string error;

    const char* invalid[] = {
        "", " ", "[", "{", "true false", "+1", ".1", "1.", "1e", "--1",
        "[1,,2]", "[1 2]", "{\"a\":}", "{\"a\":1 \"b\":2}",
        "\"\\u0000", "\"\\uD800\"", "\"\\uDC00\""
    };
    for (const char* text : invalid) reject(text);

    assert(json::Document::parse("[0,-0,1.25,1e3,-2.5E-2]", value, error));
    assert(value.is_array() && value.array.size() == 5);
    assert(json::Document::parse("{\"nested\":{\"array\":[true,false,null,\"x\"]}}", value, error));
    assert(value["nested"]["array"][3].string == "x");

    assert(json::Document::parse("{\"a\":1,\"a\":2}", value, error));
    assert(value.object.size() == 2);
    assert(value.object[0].first == "a" && value.object[0].second.as_int() == 1);
    assert(value.object[1].first == "a" && value.object[1].second.as_int() == 2);

    reject(std::string("[\"\\xff\"]", 5));
    reject(std::string("[\"\\xc0\\xaf\"]", 6));
    reject(std::string("[\"\\xed\\xa0\\x80\"]", 7));
    reject(std::string("[\"\\xf4\\x90\\x80\\x80\"]", 8));

    std::string deeply_nested;
    for (int i = 0; i < 64; ++i) deeply_nested += '[';
    deeply_nested += '0';
    for (int i = 0; i < 64; ++i) deeply_nested += ']';
    assert(json::Document::parse(deeply_nested, value, error));

    deeply_nested.clear();
    for (int i = 0; i < 513; ++i) deeply_nested += '[';
    deeply_nested += '0';
    for (int i = 0; i < 513; ++i) deeply_nested += ']';
    reject(deeply_nested);

    std::cout << "Jsonic++ adversarial test passed\n";
}
