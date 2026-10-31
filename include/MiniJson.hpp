#ifndef FMCOSKUN_MINI_JSON_HPP
#define FMCOSKUN_MINI_JSON_HPP

#include <cctype>
#include <cstdlib>
#include <string>
#include <vector>

namespace FmCoskunStudio {
namespace Json {

    // ========================================================================
    // 1. JSON DEĞER AĞACI (DOCUMENT OBJECT MODEL)
    // ========================================================================
    class Value {
    public:
        enum class Type { Null, Bool, Number, String, Array, Object };

        Type type{Type::Null};
        bool boolean{false};
        double number{0.0};
        std::string str;
        std::vector<Value> items;            // Array elemanları veya Object değerleri
        std::vector<std::string> keys;       // Object anahtarları (items ile aynı sırada)

        bool isObject() const { return type == Type::Object; }
        bool isArray()  const { return type == Type::Array; }

        // Object içinde anahtar arar; bulunamazsa nullptr döner
        const Value* find(const std::string& key) const {
            if (type != Type::Object) return nullptr;
            for (size_t i = 0; i < keys.size(); ++i) {
                if (keys[i] == key) return &items[i];
            }
            return nullptr;
        }

        std::string getString(const std::string& key, const std::string& fallback = "") const {
            const Value* v = find(key);
            return (v && v->type == Type::String) ? v->str : fallback;
        }

        float getFloat(const std::string& key, float fallback = 0.0f) const {
            const Value* v = find(key);
            return (v && v->type == Type::Number) ? static_cast<float>(v->number) : fallback;
        }

        std::vector<std::string> getStringArray(const std::string& key) const {
            std::vector<std::string> out;
            const Value* v = find(key);
            if (!v || v->type != Type::Array) return out;
            for (const auto& item : v->items) {
                if (item.type == Type::String) out.push_back(item.str);
            }
            return out;
        }
    };

    // ========================================================================
    // 2. ÖZYİNELEMELİ İNİŞLİ AYRIŞTIRICI (RECURSIVE-DESCENT PARSER, RFC 8259)
    // ========================================================================
    class Parser {
    private:
        const std::string& src;
        size_t pos{0};
        std::string error;
        static constexpr int MAX_DEPTH = 64;

        bool fail(const std::string& msg) {
            if (error.empty()) {
                size_t line = 1, col = 1;
                for (size_t i = 0; i < pos && i < src.size(); ++i) {
                    if (src[i] == '\n') { line++; col = 1; } else { col++; }
                }
                error = "line " + std::to_string(line) + ", col " + std::to_string(col) + ": " + msg;
            }
            return false;
        }

        void skipWhitespace() {
            while (pos < src.size() && (src[pos] == ' ' || src[pos] == '\t' || src[pos] == '\n' || src[pos] == '\r')) {
                pos++;
            }
        }

        bool consume(char expected) {
            skipWhitespace();
            if (pos < src.size() && src[pos] == expected) {
                pos++;
                return true;
            }
            return false;
        }

        bool matchLiteral(const char* literal) {
            size_t len = std::char_traits<char>::length(literal);
            if (src.compare(pos, len, literal) == 0) {
                pos += len;
                return true;
            }
            return false;
        }

        static void appendUtf8(std::string& out, unsigned long cp) {
            if (cp < 0x80) {
                out += static_cast<char>(cp);
            } else if (cp < 0x800) {
                out += static_cast<char>(0xC0 | (cp >> 6));
                out += static_cast<char>(0x80 | (cp & 0x3F));
            } else if (cp < 0x10000) {
                out += static_cast<char>(0xE0 | (cp >> 12));
                out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                out += static_cast<char>(0x80 | (cp & 0x3F));
            } else {
                out += static_cast<char>(0xF0 | (cp >> 18));
                out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
                out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                out += static_cast<char>(0x80 | (cp & 0x3F));
            }
        }

        bool parseHex4(unsigned long& out) {
            if (pos + 4 > src.size()) return fail("truncated \\u escape");
            out = 0;
            for (int i = 0; i < 4; ++i) {
                char c = src[pos++];
                out <<= 4;
                if (c >= '0' && c <= '9')      out |= static_cast<unsigned long>(c - '0');
                else if (c >= 'a' && c <= 'f') out |= static_cast<unsigned long>(c - 'a' + 10);
                else if (c >= 'A' && c <= 'F') out |= static_cast<unsigned long>(c - 'A' + 10);
                else return fail("invalid hex digit in \\u escape");
            }
            return true;
        }

        bool parseString(std::string& out) {
            // Çağıran açılış tırnağını doğrulamış olmalı
            pos++;
            out.clear();
            while (pos < src.size()) {
                char c = src[pos++];
                if (c == '"') return true;
                if (static_cast<unsigned char>(c) < 0x20) return fail("control character in string");
                if (c != '\\') {
                    out += c;
                    continue;
                }
                if (pos >= src.size()) break;
                char esc = src[pos++];
                switch (esc) {
                    case '"':  out += '"';  break;
                    case '\\': out += '\\'; break;
                    case '/':  out += '/';  break;
                    case 'b':  out += '\b'; break;
                    case 'f':  out += '\f'; break;
                    case 'n':  out += '\n'; break;
                    case 'r':  out += '\r'; break;
                    case 't':  out += '\t'; break;
                    case 'u': {
                        unsigned long cp = 0;
                        if (!parseHex4(cp)) return false;
                        // UTF-16 vekil çifti (surrogate pair) birleştirme
                        if (cp >= 0xD800 && cp <= 0xDBFF) {
                            unsigned long low = 0;
                            if (!matchLiteral("\\u") || !parseHex4(low) || low < 0xDC00 || low > 0xDFFF) {
                                return fail("invalid UTF-16 surrogate pair");
                            }
                            cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
                        }
                        appendUtf8(out, cp);
                        break;
                    }
                    default:
                        pos--;
                        return fail(std::string("invalid escape '\\") + esc + "'");
                }
            }
            return fail("unterminated string");
        }

        bool parseNumber(Value& out) {
            size_t start = pos;
            if (pos < src.size() && src[pos] == '-') pos++;
            if (pos >= src.size() || !std::isdigit(static_cast<unsigned char>(src[pos]))) return fail("invalid number");
            if (src[pos] == '0') {
                pos++;
            } else {
                while (pos < src.size() && std::isdigit(static_cast<unsigned char>(src[pos]))) pos++;
            }
            if (pos < src.size() && src[pos] == '.') {
                pos++;
                if (pos >= src.size() || !std::isdigit(static_cast<unsigned char>(src[pos]))) return fail("digit expected after '.'");
                while (pos < src.size() && std::isdigit(static_cast<unsigned char>(src[pos]))) pos++;
            }
            if (pos < src.size() && (src[pos] == 'e' || src[pos] == 'E')) {
                pos++;
                if (pos < src.size() && (src[pos] == '+' || src[pos] == '-')) pos++;
                if (pos >= src.size() || !std::isdigit(static_cast<unsigned char>(src[pos]))) return fail("digit expected in exponent");
                while (pos < src.size() && std::isdigit(static_cast<unsigned char>(src[pos]))) pos++;
            }
            out.type = Value::Type::Number;
            out.number = std::strtod(src.substr(start, pos - start).c_str(), nullptr);
            return true;
        }

        bool parseValue(Value& out, int depth) {
            if (depth > MAX_DEPTH) return fail("nesting too deep");
            skipWhitespace();
            if (pos >= src.size()) return fail("unexpected end of input");

            char c = src[pos];
            if (c == '{') return parseObject(out, depth);
            if (c == '[') return parseArray(out, depth);
            if (c == '"') {
                out.type = Value::Type::String;
                return parseString(out.str);
            }
            if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parseNumber(out);
            if (matchLiteral("true"))  { out.type = Value::Type::Bool; out.boolean = true;  return true; }
            if (matchLiteral("false")) { out.type = Value::Type::Bool; out.boolean = false; return true; }
            if (matchLiteral("null"))  { out.type = Value::Type::Null; return true; }
            return fail(std::string("unexpected character '") + c + "'");
        }

        bool parseArray(Value& out, int depth) {
            pos++;
            out.type = Value::Type::Array;
            if (consume(']')) return true;
            while (true) {
                Value item;
                if (!parseValue(item, depth + 1)) return false;
                out.items.push_back(std::move(item));
                if (consume(',')) continue;
                if (consume(']')) return true;
                return fail("expected ',' or ']' in array");
            }
        }

        bool parseObject(Value& out, int depth) {
            pos++;
            out.type = Value::Type::Object;
            if (consume('}')) return true;
            while (true) {
                skipWhitespace();
                if (pos >= src.size() || src[pos] != '"') return fail("expected string key in object");
                std::string key;
                if (!parseString(key)) return false;
                if (!consume(':')) return fail("expected ':' after object key");
                Value item;
                if (!parseValue(item, depth + 1)) return false;
                out.keys.push_back(std::move(key));
                out.items.push_back(std::move(item));
                if (consume(',')) continue;
                if (consume('}')) return true;
                return fail("expected ',' or '}' in object");
            }
        }

    public:
        explicit Parser(const std::string& text) : src(text) {}

        // Başarılıysa true döner; aksi halde errorMessage satır/sütun bilgisiyle doldurulur
        bool parse(Value& out, std::string& errorMessage) {
            // UTF-8 BOM'u atla (Windows editörleri sıkça ekler)
            if (src.compare(0, 3, "\xEF\xBB\xBF") == 0) pos = 3;
            bool ok = parseValue(out, 0);
            if (ok) {
                skipWhitespace();
                if (pos != src.size()) ok = fail("unexpected trailing content");
            }
            errorMessage = error;
            return ok;
        }
    };

} // namespace Json
} // namespace FmCoskunStudio

#endif // FMCOSKUN_MINI_JSON_HPP
