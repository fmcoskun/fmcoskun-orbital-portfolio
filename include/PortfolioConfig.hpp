#ifndef FMCOSKUN_PORTFOLIO_CONFIG_HPP
#define FMCOSKUN_PORTFOLIO_CONFIG_HPP

#include "raylib.h"
#include "MiniJson.hpp"
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

    // Projelerde repoUrl verilmemişse butonların yönlendiği varsayılan GitHub profili
    inline constexpr const char* DEFAULT_GITHUB_PROFILE = "https://github.com/fmcoskun";

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
        std::string liveUrl; // Boşsa canlı demo butonu gösterilmez
        std::string repoUrl; // Boşsa GitHub butonu profile yönlenir
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

    struct ConfigLoadResult {
        std::vector<OrbitNodeConfig> orbits;
        bool loadedFromFile{false};
        std::string githubProfile{DEFAULT_GITHUB_PROFILE};
        std::string sourcePath; // Okunmaya çalışılan dosyanın çözümlenmiş yolu
        std::string error;      // Yerleşik verilere düşülme sebebi (dosya hiç yoksa boş)
    };

    // ========================================================================
    // 3. HARİCİ JSON YAPILANDIRMA OKUYUCU (MiniJson DOM ÜZERİNDEN)
    // ========================================================================
    class PortfolioConfigLoader {
    private:
        static std::string resolveConfigPath(const std::string& fileName) {
            // Önce çalıştırılabilir dosyanın klasörüne bak; böylece exe başka bir dizinden başlatılsa da JSON bulunur
            std::string appDir = GetApplicationDirectory();
            if (!appDir.empty() && appDir.back() != '/' && appDir.back() != '\\') appDir += '/';
            std::string besideExe = appDir + fileName;
            if (FileExists(besideExe.c_str())) return besideExe;
            return fileName;
        }

        static MetricSpec parseMetric(const Json::Value& m) {
            MetricSpec spec;
            spec.label = m.getString("label", "Metric");
            spec.value = m.getString("value", "100%");
            spec.ratio = std::clamp(m.getFloat("ratio", 0.95f), 0.0f, 1.0f);
            return spec;
        }

        static OrbitNodeConfig parseOrbit(const Json::Value& o) {
            OrbitNodeConfig cfg;
            cfg.id           = o.getString("id", "ORBIT-NODE");
            cfg.label        = o.getString("label", "Service Domain");
            // Sahneyi bozacak uç değerler güvenli aralığa sıkıştırılır
            cfg.radius       = std::clamp(o.getFloat("radius", 2.5f), 1.2f, 8.0f);
            cfg.tiltX        = o.getFloat("tiltX", 0.3f);
            cfg.tiltZ        = o.getFloat("tiltZ", 0.2f);
            cfg.angularSpeed = std::clamp(o.getFloat("angularSpeed", 0.35f), -3.0f, 3.0f);
            cfg.initialPhase = o.getFloat("initialPhase", 0.0f);
            cfg.nodeSize     = std::clamp(o.getFloat("nodeSize", 0.15f), 0.05f, 0.6f);

            cfg.project.code             = o.getString("code", "LAB // SYS");
            cfg.project.title            = o.getString("title", "Untitled Project");
            cfg.project.category         = o.getString("category", cfg.label);
            cfg.project.status           = o.getString("status", "CONCEPT");
            cfg.project.summary          = o.getString("summary", "");
            cfg.project.architectureNote = o.getString("architectureNote", "");
            cfg.project.liveUrl          = o.getString("liveUrl", "");
            cfg.project.repoUrl          = o.getString("repoUrl", "");
            cfg.project.techStack        = o.getStringArray("techStack");
            cfg.project.pipeline         = o.getStringArray("pipeline");

            if (const Json::Value* metrics = o.find("metrics"); metrics && metrics->isArray()) {
                for (const auto& m : metrics->items) {
                    if (m.isObject()) cfg.project.metrics.push_back(parseMetric(m));
                }
            }
            return cfg;
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
                        "", "",
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
                        "", "",
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
                        "", "",
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

        // Dosya yoksa sessizce yerleşik verilere düşer; dosya var ama bozuksa sebebi error alanında döner
        static ConfigLoadResult loadFromJsonFile(const std::string& fileName) {
            ConfigLoadResult result;
            result.sourcePath = resolveConfigPath(fileName);

            auto fallback = [&](const std::string& reason) {
                result.orbits = getBuiltInDefaults();
                result.loadedFromFile = false;
                result.error = reason;
                return result;
            };

            if (!FileExists(result.sourcePath.c_str())) {
                return fallback("");
            }

            std::ifstream in(result.sourcePath, std::ios::binary);
            if (!in.is_open()) {
                return fallback("cannot open " + fileName);
            }

            std::ostringstream ss;
            ss << in.rdbuf();
            const std::string content = ss.str();

            Json::Value root;
            std::string parseError;
            if (!Json::Parser(content).parse(root, parseError)) {
                return fallback(parseError);
            }

            // Yalnızca https bağlantıları kabul edilir; aksi halde varsayılan profil kullanılır
            std::string profile = root.getString("githubProfile", DEFAULT_GITHUB_PROFILE);
            if (profile.rfind("https://", 0) == 0) result.githubProfile = profile;

            const Json::Value* orbits = root.find("orbits");
            if (!orbits || !orbits->isArray()) {
                return fallback("missing \"orbits\" array");
            }

            for (const auto& item : orbits->items) {
                if (item.isObject()) result.orbits.push_back(parseOrbit(item));
            }
            if (result.orbits.empty()) {
                return fallback("\"orbits\" array has no valid entries");
            }

            result.loadedFromFile = true;
            return result;
        }
    };

} // namespace FmCoskunStudio

#endif // FMCOSKUN_PORTFOLIO_CONFIG_HPP

