#ifndef FMCOSKUN_PORTFOLIO_CONFIG_HPP
#define FMCOSKUN_PORTFOLIO_CONFIG_HPP

#include "raylib.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>

namespace FmCoskunStudio {

    // ========================================================================
    // 1. TASARIM TOKENLARI (DESIGN TOKENS - Single Source of Truth)
    // ========================================================================
    namespace Tokens {
        constexpr Color BG_PRIMARY      = { 11,  15,  11, 255 }; // #0B0F0B
        constexpr Color BG_HEADER       = { 14,  20,  14, 238 }; // rgba(14, 20, 14, 0.93)
        constexpr Color BG_SURFACE      = { 20,  27,  20, 255 }; // #141B14
        constexpr Color BG_ELEVATED     = { 26,  36,  26, 255 }; // #1A241A
        constexpr Color BG_MODAL_BACK   = { 5,    8,   5, 215 }; // Modal karartma katmanı
        constexpr Color ACCENT_PRIMARY  = { 52, 232,  52, 255 }; // #34E834
        constexpr Color ACCENT_SECONDARY= { 110, 245, 110, 255 }; // Hover parlak yeşil
        constexpr Color ACCENT_GLOW     = { 52, 232,  52,  90 }; // rgba(52, 232, 52, 0.35)
        constexpr Color BORDER_COLOR    = { 35,  51,  35, 255 }; // #233323
        constexpr Color TEXT_PRIMARY    = { 240, 250, 240, 255 }; // #F0FAF0
        constexpr Color TEXT_SECONDARY  = { 142, 165, 142, 255 }; // #8EA58E
        constexpr Color TEXT_ON_BRIGHT  = { 0,    0,   0, 255 }; // #000000
    }

    // ========================================================================
    // 2. VERİ MODELLERİ (PORTFOLIO, METRIC & ORBIT CONFIG STRUCTS)
    // ========================================================================
    struct MetricSpec {
        std::string label;
        std::string value;
        float ratio{0.9f};
    };

    struct ProjectSpec {
        std::string code;
        std::string title;
        std::string category;
        std::string status;
        std::string summary;
        std::string architectureNote;
        std::string liveUrl;
        std::string repoUrl;
        std::vector<std::string> techStack;
        std::vector<std::string> pipeline;
        std::vector<MetricSpec> metrics;
    };

    struct OrbitNodeConfig {
        std::string id;
        std::string label;
        float radius{2.5f};
        float tiltX{0.3f};
        float tiltZ{0.2f};
        float angularSpeed{0.35f};
        float initialPhase{0.0f};
        float nodeSize{0.15f};
        ProjectSpec project;
    };

    // ========================================================================
    // 3. HARİCİ JSON YAPILANDIRMA OKUYUCU (ZERO-DEPENDENCY JSON PARSER)
    // ========================================================================
    class PortfolioConfigLoader {
    private:
        static std::string extractString(const std::string& block, const std::string& key, const std::string& fallback = "") {
            std::string pattern = "\"" + key + "\"";
            size_t keyPos = block.find(pattern);
            if (keyPos == std::string::npos) return fallback;
            size_t colonPos = block.find(':', keyPos + pattern.size());
            if (colonPos == std::string::npos) return fallback;
            size_t firstQuote = block.find('"', colonPos + 1);
            if (firstQuote == std::string::npos) return fallback;
            size_t secondQuote = block.find('"', firstQuote + 1);
            if (secondQuote == std::string::npos) return fallback;
            return block.substr(firstQuote + 1, secondQuote - firstQuote - 1);
        }

        static float extractFloat(const std::string& block, const std::string& key, float fallback = 0.0f) {
            std::string pattern = "\"" + key + "\"";
            size_t keyPos = block.find(pattern);
            if (keyPos == std::string::npos) return fallback;
            size_t colonPos = block.find(':', keyPos + pattern.size());
            if (colonPos == std::string::npos) return fallback;
            size_t valStart = block.find_first_of("-0123456789.", colonPos + 1);
            if (valStart == std::string::npos) return fallback;
            size_t valEnd = block.find_first_not_of("-0123456789.", valStart);
            std::string numStr = block.substr(valStart, valEnd - valStart);
            char* endPtr = nullptr;
            float val = std::strtof(numStr.c_str(), &endPtr);
            return (endPtr != numStr.c_str()) ? val : fallback;
        }

        static std::vector<std::string> extractStringArray(const std::string& block, const std::string& key) {
            std::vector<std::string> result;
            std::string pattern = "\"" + key + "\"";
            size_t keyPos = block.find(pattern);
            if (keyPos == std::string::npos) return result;
            size_t openBracket = block.find('[', keyPos + pattern.size());
            if (openBracket == std::string::npos) return result;
            size_t closeBracket = block.find(']', openBracket + 1);
            if (closeBracket == std::string::npos) return result;

            std::string arrContent = block.substr(openBracket + 1, closeBracket - openBracket - 1);
            size_t pos = 0;
            while (true) {
                size_t q1 = arrContent.find('"', pos);
                if (q1 == std::string::npos) break;
                size_t q2 = arrContent.find('"', q1 + 1);
                if (q2 == std::string::npos) break;
                result.push_back(arrContent.substr(q1 + 1, q2 - q1 - 1));
                pos = q2 + 1;
            }
            return result;
        }

        static std::vector<MetricSpec> extractMetrics(const std::string& block) {
            std::vector<MetricSpec> metrics;
            size_t keyPos = block.find("\"metrics\"");
            if (keyPos == std::string::npos) return metrics;
            size_t openBracket = block.find('[', keyPos);
            size_t closeBracket = block.find(']', openBracket);
            if (openBracket == std::string::npos || closeBracket == std::string::npos) return metrics;

            std::string arrContent = block.substr(openBracket + 1, closeBracket - openBracket - 1);
            size_t pos = 0;
            while (true) {
                size_t objStart = arrContent.find('{', pos);
                if (objStart == std::string::npos) break;
                size_t objEnd = arrContent.find('}', objStart);
                if (objEnd == std::string::npos) break;
                std::string mBlock = arrContent.substr(objStart, objEnd - objStart + 1);
                MetricSpec m;
                m.label = extractString(mBlock, "label", "Metric");
                m.value = extractString(mBlock, "value", "100%");
                m.ratio = extractFloat(mBlock, "ratio", 0.95f);
                metrics.push_back(m);
                pos = objEnd + 1;
            }
            return metrics;
        }

    public:
        static std::vector<OrbitNodeConfig> getBuiltInDefaults() {
            return {
                OrbitNodeConfig{
                    "ORBIT-ALPHA", "Digital Experiences",
                    2.35f, 0.42f, -0.25f, 0.45f, 0.40f, 0.16f,
                    ProjectSpec{
                        "LAB-01 // SYS-01",
                        "Aether Orbital Telemetry Interface",
                        "Digital Experiences",
                        "PRODUCTION READY",
                        "Low-latency 3D control deck for visualizing satellite constellation trajectories and real-time signal handoffs.",
                        "Single-pass procedural geometry pipeline + zero-state pointer parallax loop with 60 FPS deterministic frame pacing.",
                        "https://fmcoskun.dev/projects/aether-orbital",
                        "https://github.com/fmcoskun/aether-orbital-telemetry",
                        {"C++17", "Raylib / OpenGL", "React Three Fiber", "TypeScript"},
                        {"Sensor Ingest", "Quaternion Solver", "3D Scene Graph", "60FPS HUD Render"},
                        {
                            {"Frame Pacing Stability", "99.4%", 0.994f},
                            {"GPU Draw Call Efficiency", "94.0%", 0.940f},
                            {"Pointer Parallax Latency", "< 2.1ms", 0.960f}
                        }
                    }
                },
                OrbitNodeConfig{
                    "ORBIT-BETA", "Intelligent Automation",
                    3.15f, -0.55f, 0.38f, -0.32f, 2.10f, 0.17f,
                    ProjectSpec{
                        "LAB-02 // SYS-02",
                        "Synapse Autonomous Triage Engine",
                        "Intelligent Automation",
                        "ARCHITECTURE SPEC",
                        "Deterministic and AI-assisted workflow router that validates incoming client briefs against strict domain schemas.",
                        "Chain-of-Responsibility task pipeline with zero-trust schema guards and human-in-the-loop verification checkpoints.",
                        "https://fmcoskun.dev/projects/synapse-triage",
                        "https://github.com/fmcoskun/synapse-autonomous-triage",
                        {"Workflow Automation", "Schema Guards", "C++ Templates", "Event Pipelines"},
                        {"Brief Webhook", "Schema Validator", "AI Classifier", "Dispatch Queue"},
                        {
                            {"Schema Validation Accuracy", "99.8%", 0.998f},
                            {"Automated Routing Rate", "88.5%", 0.885f},
                            {"Throughput Capacity", "4.2k req/s", 0.920f}
                        }
                    }
                },
                OrbitNodeConfig{
                    "ORBIT-GAMMA", "Systems & Integration",
                    3.95f, 0.22f, 0.62f, 0.22f, 4.20f, 0.15f,
                    ProjectSpec{
                        "LAB-03 // SYS-03",
                        "Nexus Multi-Source Data Mesh",
                        "Systems & Integration",
                        "INTERNAL LAB",
                        "Resilient API orchestration layer synchronizing distributed state and telemetry webhooks into one unified core.",
                        "Isolated adapter boundaries with exponential backoff retry queues and circuit-breaker fallback telemetry.",
                        "https://fmcoskun.dev/projects/nexus-data-mesh",
                        "https://github.com/fmcoskun/nexus-data-mesh",
                        {"Systems Architecture", "API Gateways", "State Sync", "Telemetry HUD"},
                        {"Edge Gateways", "Circuit Breaker", "State Normalizer", "Telemetry Core"},
                        {
                            {"Mesh Uptime SLA", "99.95%", 0.999f},
                            {"Circuit Recovery Speed", "91.2%", 0.912f},
                            {"State Sync Consistency", "97.6%", 0.976f}
                        }
                    }
                }
            };
        }

        static std::vector<OrbitNodeConfig> loadFromJsonFile(const std::string& filePath, bool& outLoadedFromFile) {
            outLoadedFromFile = false;
            std::ifstream in(filePath);
            if (!in.is_open()) {
                return getBuiltInDefaults();
            }

            std::ostringstream ss;
            ss << in.rdbuf();
            std::string content = ss.str();

            size_t orbitsPos = content.find("\"orbits\"");
            if (orbitsPos == std::string::npos) {
                return getBuiltInDefaults();
            }

            size_t arrStart = content.find('[', orbitsPos);
            if (arrStart == std::string::npos) {
                return getBuiltInDefaults();
            }

            std::vector<OrbitNodeConfig> parsedConfigs;
            size_t pos = arrStart + 1;
            while (pos < content.size()) {
                size_t objStart = content.find('{', pos);
                if (objStart == std::string::npos) break;

                // Eşleşen kapanış süslü parantezini bul (iç içe metrics dizisini dikkate alarak)
                int depth = 0;
                size_t objEnd = std::string::npos;
                for (size_t i = objStart; i < content.size(); ++i) {
                    if (content[i] == '{') depth++;
                    else if (content[i] == '}') {
                        depth--;
                        if (depth == 0) {
                            objEnd = i;
                            break;
                        }
                    }
                }
                if (objEnd == std::string::npos) break;

                std::string block = content.substr(objStart, objEnd - objStart + 1);
                OrbitNodeConfig cfg;
                cfg.id           = extractString(block, "id", "ORBIT-NODE");
                cfg.label        = extractString(block, "label", "Service Domain");
                cfg.radius       = extractFloat(block, "radius", 2.5f);
                cfg.tiltX        = extractFloat(block, "tiltX", 0.3f);
                cfg.tiltZ        = extractFloat(block, "tiltZ", 0.2f);
                cfg.angularSpeed = extractFloat(block, "angularSpeed", 0.35f);
                cfg.initialPhase = extractFloat(block, "initialPhase", 0.0f);
                cfg.nodeSize     = extractFloat(block, "nodeSize", 0.15f);

                cfg.project.code             = extractString(block, "code", "LAB // SYS");
                cfg.project.title            = extractString(block, "title", "Untitled Project");
                cfg.project.category         = extractString(block, "category", cfg.label);
                cfg.project.status           = extractString(block, "status", "CONCEPT");
                cfg.project.summary          = extractString(block, "summary", "");
                cfg.project.architectureNote = extractString(block, "architectureNote", "");
                cfg.project.liveUrl          = extractString(block, "liveUrl", "https://fmcoskun.dev");
                cfg.project.repoUrl          = extractString(block, "repoUrl", "https://github.com/fmcoskun");
                cfg.project.techStack        = extractStringArray(block, "techStack");
                cfg.project.pipeline         = extractStringArray(block, "pipeline");
                cfg.project.metrics          = extractMetrics(block);

                parsedConfigs.push_back(std::move(cfg));
                pos = objEnd + 1;
            }

            if (parsedConfigs.empty()) {
                return getBuiltInDefaults();
            }

            outLoadedFromFile = true;
            return parsedConfigs;
        }
    };

} // namespace FmCoskunStudio

#endif // FMCOSKUN_PORTFOLIO_CONFIG_HPP

