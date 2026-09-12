#include "json.h"

#include <cassert>
#include <iostream>
#include <string>

static void expect_invalid(const std::string& source) {
    json::Document document;
    std::string error;
    assert(!json::Document::parse(source, document, error));
    assert(!error.empty());
}

int main() {
    json::Document document;
    std::string error;

    const std::string source =
        R"({"title":"A \"quoted\" title","emoji":"\uD83D\uDE00","items":[1,true,null,{"text":"line\nnext"}]})";

    assert(json::Document::parse(source, document, error));
    assert(document.is_object());
    assert(document["title"].string == "A \"quoted\" title");
    assert(document["emoji"].string == "😀");
    assert(document["items"][3]["text"].string == "line\nnext");

    document["extra"] = "value";
    const std::string serialized = document.dump();
    json::Document reparsed;
    assert(json::Document::parse(serialized, reparsed, error));
    assert(reparsed["extra"].string == "value");

    // Structural errors. Duplicate names are valid JSON syntax and are
    // preserved in source order, even though applications should avoid them.
    assert(json::Document::parse(R"({"a":1,"a":2})", document, error));
    assert(document.object.size() == 2);
    expect_invalid(R"([1,])");
    expect_invalid(R"({"a":1,})");
    expect_invalid(R"({"a" 1})");
    expect_invalid(R"({a:1})");

    // Number grammar and finite-range handling.
    expect_invalid("01");
    expect_invalid("-01");
    expect_invalid("1.");
    expect_invalid("1e");
    expect_invalid("1e+");
    expect_invalid("1e309");
    assert(json::Document::parse("-0.25e+2", document, error));
    assert(document.is_number() && document.num == -25.0);

    // Numbers whose original JSON spelling cannot be reproduced by the
    // compact double representation retain that spelling internally while
    // remaining numbers through the public predicates and numeric field.
    assert(json::Document::parse("0.0", document, error));
    assert(document.type == json::Type::StrNumber && document.is_number());
    assert(document.num == 0.0 && document.dump(0) == "0.0");
    assert(json::Document::parse("1234567890123456789", document, error));
    assert(document.type == json::Type::StrNumber && document.is_number());
    assert(document.dump(0) == "1234567890123456789");
    assert(json::Document::parse("42", document, error));
    assert(document.type == json::Type::Number && document.dump(0) == "42");
    const char* exact_numbers[] = {
        "-1234567890123456789", "1234567890123456789", "9223372036854775807",
        "0.0", "-0.0", "5e-324", "2.225073858507201e-308",
        "2.2250738585072014e-308", "1.7976931348623157e308"
    };
    for (const char* exact : exact_numbers) {
        assert(json::Document::parse(exact, document, error));
        assert(document.is_number() && document.dump(0) == exact);
    }

    // Bounded views do not require a null terminator and must not consume
    // adjacent bytes. Extreme values exercise the precise strtod fallback.
    const std::string padded_array = "[1,2] trailing";
    assert(json::Document::parse(std::string_view(padded_array.data(), 5), document, error));
    assert(document.is_array() && document.array.size() == 2);
    const std::string padded_number = "12x";
    assert(json::Document::parse(std::string_view(padded_number.data(), 2), document, error));
    assert(document.is_number() && document.num == 12.0);
    const std::string underflow = "1e-10000x";
    assert(json::Document::parse(std::string_view(underflow.data(), 8), document, error));
    assert(document.is_number() && document.num == 0.0);
    const std::string overflow = "1e309x";
    assert(!json::Document::parse(std::string_view(overflow.data(), 5), document, error));

    // String escapes / Unicode surrogate handling.
    expect_invalid(R"("\x")");
    expect_invalid(R"("\uDE00")");
    expect_invalid(R"("\uD83D")");
    expect_invalid(R"("\uD83D\u0041")");
    expect_invalid(std::string("\"line\nraw\""));
    assert(json::Document::parse(R"("\/\b\f\n\r\t")", document, error));

    // Streaming named-array reader should reject duplicate root keys too.
    bool callback_called = false;
    assert(!json::Document::for_each_array_item(
        R"({"tracked":[],"tracked":[]})", "tracked",
        [&](json::Document&&) { callback_called = true; return true; }, error));
    assert(!callback_called);

    std::cout << "JSON smoke test passed\n";
}
