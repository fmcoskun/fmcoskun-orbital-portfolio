#include "PortfolioConfig.hpp"
#include <cstdio>

using namespace FmCoskunStudio;

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { std::printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

static bool parse(const std::string& text, Json::Value& v, std::string& err) {
    return Json::Parser(text).parse(v, err);
}

int main(int argc, char** argv) {
    Json::Value v;
    std::string err;

    // Kaçış karakterleri, unicode, string içindeki parantezler
    CHECK(parse(R"({"a":"say \"hi\" {x} [y]","t":"çğ","e":"🚀","n":-1.5e2,"b":true,"z":null})", v, err));
    CHECK(v.getString("a") == "say \"hi\" {x} [y]");
    CHECK(v.getString("t") == "\xC3\xA7\xC4\x9F");          // çğ
    CHECK(v.getString("e") == "\xF0\x9F\x9A\x80");          // roket emojisi (surrogate pair)
    CHECK(v.getFloat("n") == -150.0f);
    CHECK(v.find("b") && v.find("b")->boolean);
    CHECK(v.find("z") && v.find("z")->type == Json::Value::Type::Null);

    // Tip uyuşmazlığında fallback (eski parser burada bir sonraki sayıyı yakalardı)
    CHECK(parse(R"({"radius":"abc","next":9})", v, err));
    CHECK(v.getFloat("radius", 2.5f) == 2.5f);

    // Anahtar adı başka bir string değerinin içinde geçse bile karışmaz
    CHECK(parse(R"({"summary":"the \"title\" is here","title":"Real"})", v, err));
    CHECK(v.getString("title") == "Real");

    // Hatalı girdiler ve satır/sütun raporu
    CHECK(!parse("{\n  \"a\": 1,\n  \"b\" 2\n}", v, err));
    std::printf("error sample 1: %s\n", err.c_str());
    CHECK(err.find("line 3") != std::string::npos);
    CHECK(!parse(R"({"a":[1,2,})", v, err));
    std::printf("error sample 2: %s\n", err.c_str());
    CHECK(!parse(R"({"a":"unterminated)", v, err));
    CHECK(!parse(R"({"a":01})", v, err));
    CHECK(!parse(R"({"a":1} extra)", v, err));
    CHECK(!parse(std::string(100, '['), v, err));             // derinlik sınırı

    // BOM toleransı
    CHECK(parse("\xEF\xBB\xBF{\"k\":\"v\"}", v, err));

    // Gerçek projects.json
    if (argc > 1) {
        ChangeDirectory(argv[1]);
        ConfigLoadResult r = PortfolioConfigLoader::loadFromJsonFile("projects.json");
        std::printf("projects.json: fromFile=%d orbits=%zu error='%s' path='%s'\n",
                    r.loadedFromFile, r.orbits.size(), r.error.c_str(), r.sourcePath.c_str());
        CHECK(r.loadedFromFile && r.orbits.size() == 3 && r.error.empty());
        auto defaults = PortfolioConfigLoader::getBuiltInDefaults();
        for (size_t i = 0; i < r.orbits.size() && i < defaults.size(); ++i) {
            CHECK(r.orbits[i].id == defaults[i].id);
            CHECK(r.orbits[i].project.title == defaults[i].project.title);
            CHECK(r.orbits[i].project.techStack == defaults[i].project.techStack);
            CHECK(r.orbits[i].project.metrics.size() == defaults[i].project.metrics.size());
            CHECK(r.orbits[i].radius == defaults[i].radius);
        }
    }

    std::printf(failures == 0 ? "ALL TESTS PASSED\n" : "%d FAILURE(S)\n", failures);
    return failures == 0 ? 0 : 1;
}
