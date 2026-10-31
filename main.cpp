/**
 * ============================================================================
 * PROJECT: FMCOSKUN STUDIO — 3D INTERACTIVE DESKTOP PORTFOLIO (C++17 / OOP)
 * CORE CONCEPT: "Streaming Your Success To New Orbits"
 * GRAPHICS ENGINE: Raylib (OpenGL Hardware-Accelerated 3D + 2D HUD)
 * ============================================================================
 *
 * DERLEME KOMUTLARI (BUILD INSTRUCTIONS):
 * ----------------------------------------------------------------------------
 * 1) Makefile (Önerilen / Recommended):
 *    mingw32-make
 *    mingw32-make run
 *
 * 2) Windows (MinGW / g++):
 *    g++ -std=c++17 -O2 main.cpp -o fmcoskun_studio.exe -lraylib -lopengl32 -lgdi32 -lwinmm
 *
 * 3) Linux (GCC):
 *    g++ -std=c++17 -O2 main.cpp -o fmcoskun_studio -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
 *
 * 4) macOS (Clang):
 *    clang++ -std=c++17 -O2 main.cpp -o fmcoskun_studio -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
 *
 * KONTROLLER (INTERACTIVE CONTROLS):
 * ----------------------------------------------------------------------------
 * - Fare Hareketi     : 3B Kamera Paralaks Etkisi (Pointer-Ref Parallax)
 * - 3B Uyduya Tıklama : 3B Raycasting ile Doğrudan Yörünge / Proje Seçimi
 * - Sağ Tık + Sürükle : Kamerayı Sahne Etrafında Döndür (Orbit Camera)
 * - Fare Tekerleği    : 3B Sahneye Yakınlaş / Uzaklaş (Smooth FOV Zoom)
 * - [1]..[9] / [TAB]  : Yörünge Düğümlerini Seç (SHIFT+TAB: Önceki)
 * - [ENTER]           : Seçili Projenin Derin Mimari & Metrik Modalını Aç
 * - [F] Tuşu          : Kamerayı Seçili 3B Uyduya Kilitle / Serbest Bırak
 * - [C] Tuşu          : Kamera Açısını ve Zoom Seviyesini Sıfırla
 * - [L] Tuşu          : projects.json Yapılandırmasını Yeniden Yükle (Kaydedince otomatik de yüklenir)
 * - [S] Tuşu          : Sinyal Akışlarını (Signal Streams) Aç / Kapat
 * - [R] Tuşu          : Reduced Motion (Statik / Hareketli Mod) Geçişi
 * - [Yukarı / Aşağı]  : Yörünge Hız Çarpanını (Velocity Multiplier) Ayarla
 * - [SPACE]           : FMCOSKUN Logosu 360° Karakter Dönüşünü Tetikle
 * - [F11] / [F12]     : Tam Ekran Geçişi / Zaman Damgalı Ekran Görüntüsü Kaydet (.png)
 * - [H] Tuşu          : Kontrol Rehberi Panelini Aç / Kapat
 * ============================================================================
 */

#include "include/PortfolioConfig.hpp"
#include "include/OrbitalEntities.hpp"
#include <ctime>
#include <iostream>
#include <memory>

namespace FmCoskunStudio {

    // ========================================================================
    // 1. YÜKSEK ÇÖZÜNÜRLÜKLÜ TİPOGRAFİ YÖNETİCİSİ (HiDPI TTF FONT MANAGER)
    // ========================================================================
    class TypographyManager {
    private:
        Font uiFont{};
        Font monoFont{};
        bool hasCustomUiFont{false};
        bool hasCustomMonoFont{false};

    public:
        void initialize() {
            const char* uiFontPaths[] = {
                "C:/Windows/Fonts/segoeuib.ttf",
                "C:/Windows/Fonts/segoeui.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/System/Library/Fonts/SFNS.ttf"
            };
            const char* monoFontPaths[] = {
                "C:/Windows/Fonts/consolab.ttf",
                "C:/Windows/Fonts/consola.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
                "/System/Library/Fonts/Menlo.ttc"
            };

            for (const char* path : uiFontPaths) {
                if (FileExists(path)) {
                    uiFont = LoadFontEx(path, 48, nullptr, 0);
                    if (uiFont.texture.id != 0) {
                        SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR);
                        hasCustomUiFont = true;
                        break;
                    }
                }
            }
            if (!hasCustomUiFont) uiFont = GetFontDefault();

            for (const char* path : monoFontPaths) {
                if (FileExists(path)) {
                    monoFont = LoadFontEx(path, 36, nullptr, 0);
                    if (monoFont.texture.id != 0) {
                        SetTextureFilter(monoFont.texture, TEXTURE_FILTER_BILINEAR);
                        hasCustomMonoFont = true;
                        break;
                    }
                }
            }
            if (!hasCustomMonoFont) monoFont = GetFontDefault();
        }

        void shutdown() {
            if (hasCustomUiFont) UnloadFont(uiFont);
            if (hasCustomMonoFont) UnloadFont(monoFont);
        }

        void drawUi(const std::string& text, float x, float y, float fontSize, Color color, float spacing = 0.8f) const {
            DrawTextEx(uiFont, text.c_str(), {std::round(x), std::round(y)}, fontSize, spacing, color);
        }

        void drawMono(const std::string& text, float x, float y, float fontSize, Color color, float spacing = 0.8f) const {
            DrawTextEx(monoFont, text.c_str(), {std::round(x), std::round(y)}, fontSize, spacing, color);
        }

        float measureUi(const std::string& text, float fontSize, float spacing = 0.8f) const {
            return MeasureTextEx(uiFont, text.c_str(), fontSize, spacing).x;
        }

        float measureMono(const std::string& text, float fontSize, float spacing = 0.8f) const {
            return MeasureTextEx(monoFont, text.c_str(), fontSize, spacing).x;
        }

        std::vector<std::string> wrapUi(const std::string& text, float maxWidth, float fontSize) const {
            std::istringstream iss(text);
            std::string word;
            std::string currentLine;
            std::vector<std::string> lines;

            while (iss >> word) {
                std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
                if (measureUi(testLine, fontSize) > maxWidth && !currentLine.empty()) {
                    lines.push_back(currentLine);
                    currentLine = word;
                } else {
                    currentLine = testLine;
                }
            }
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
            }
            return lines;
        }

        // Çizilen metin bloğunun toplam yüksekliğini döndürür (akışkan layout için)
        float drawWrappedUi(const std::string& text, float x, float y, float maxWidth, float fontSize, float lineGap, Color color) const {
            auto lines = wrapUi(text, maxWidth, fontSize);
            for (size_t i = 0; i < lines.size(); ++i) {
                drawUi(lines[i], x, y + static_cast<float>(i) * (fontSize + lineGap), fontSize, color);
            }
            return blockHeight(lines.size(), fontSize, lineGap);
        }

        float measureWrappedUi(const std::string& text, float maxWidth, float fontSize, float lineGap) const {
            return blockHeight(wrapUi(text, maxWidth, fontSize).size(), fontSize, lineGap);
        }

        // Genişliğe sığmayan tek satırlık metni "..." ile kısaltır
        std::string fitUi(const std::string& text, float fontSize, float maxWidth) const {
            return fitText(text, maxWidth, [&](const std::string& t) { return measureUi(t, fontSize); });
        }

        std::string fitMono(const std::string& text, float fontSize, float maxWidth) const {
            return fitText(text, maxWidth, [&](const std::string& t) { return measureMono(t, fontSize); });
        }

    private:
        static float blockHeight(size_t lineCount, float fontSize, float lineGap) {
            if (lineCount == 0) return 0.0f;
            return static_cast<float>(lineCount) * (fontSize + lineGap) - lineGap;
        }

        template <typename MeasureFn>
        static std::string fitText(const std::string& text, float maxWidth, MeasureFn measure) {
            if (measure(text) <= maxWidth) return text;
            std::string cut = text;
            while (!cut.empty() && measure(cut + "...") > maxWidth) {
                cut.pop_back();
                // UTF-8 çok baytlı karakterlerin ortasından kesme
                while (!cut.empty() && (static_cast<unsigned char>(cut.back()) & 0xC0) == 0x80) cut.pop_back();
                if (!cut.empty() && (static_cast<unsigned char>(cut.back()) & 0xC0) == 0xC0) cut.pop_back();
            }
            return cut + "...";
        }
    };

    // ========================================================================
    // 2. ANİMASYONLU FMCOSKUN LOGOSU SINIFI (STAGGERED CHARACTER FLIP)
    // ========================================================================
    class AnimatedBrandLogo {
    private:
        const std::string brandText{"FMCOSKUN"};
        float animTimer{0.0f};
        bool isAnimating{false};

    public:
        void triggerFlip() {
            isAnimating = true;
            animTimer = 0.0f;
        }

        bool update(float dt, Vector2 mousePos, Rectangle logoBounds, bool reducedMotion) {
            bool hovered = CheckCollisionPointRec(mousePos, logoBounds);
            if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                triggerFlip();
            }
            if (isAnimating) {
                if (reducedMotion) {
                    isAnimating = false;
                    return hovered;
                }
                animTimer += dt * 2.2f;
                if (animTimer >= 1.8f) {
                    isAnimating = false;
                    animTimer = 0.0f;
                }
            }
            return hovered;
        }

        void draw(int x, int y, const TypographyManager& typo, bool isJsonLoaded) const {
            const int boxWidth = 148;
            const int boxHeight = 34;

            DrawRectangle(x - 2, y - 2, boxWidth + 4, boxHeight + 4, Tokens::ACCENT_GLOW);
            DrawRectangle(x, y, boxWidth, boxHeight, Tokens::ACCENT_PRIMARY);
            DrawRectangleLinesEx({static_cast<float>(x), static_cast<float>(y),
                                  static_cast<float>(boxWidth), static_cast<float>(boxHeight)},
                                 1.5f, Tokens::BG_PRIMARY);

            for (size_t i = 0; i < brandText.size(); ++i) {
                float delay = static_cast<float>(i) * 0.08f;
                float localT = Clamp(animTimer - delay, 0.0f, 1.0f);
                float scaleX = isAnimating ? std::abs(std::cos(localT * 2.0f * PI)) : 1.0f;

                float charX = static_cast<float>(x + 12 + static_cast<int>(i) * 16);
                float charY = static_cast<float>(y + 8);

                std::string singleChar(1, brandText[i]);
                if (scaleX > 0.25f) {
                    typo.drawMono(singleChar, charX, charY, 18.0f, Tokens::TEXT_ON_BRIGHT);
                } else {
                    DrawRectangle(static_cast<int>(charX) + 4, static_cast<int>(charY), 2, 18, Tokens::TEXT_ON_BRIGHT);
                }
            }

            typo.drawMono("STUDIO // ORBITAL SYS v2.0", static_cast<float>(x + boxWidth + 14), static_cast<float>(y + 3), 12.0f, Tokens::TEXT_PRIMARY);
            typo.drawMono(isJsonLoaded ? "DATA: projects.json [LIVE]" : "DATA: BUILT-IN FALLBACK",
                          static_cast<float>(x + boxWidth + 14), static_cast<float>(y + 18), 11.0f, Tokens::TEXT_SECONDARY);
        }
    };

    // ========================================================================
    // 3. DUYARLI YERLEŞİM ÖLÇÜLERİ (RESPONSIVE LAYOUT METRICS)
    // ========================================================================
    struct LayoutMetrics {
        float headerH{64.0f};
        float footerH{42.0f};
        float margin{28.0f};
        float panelX{28.0f};
        float panelY{82.0f};
        float panelW{520.0f};
        float panelBottom{804.0f};
        float sceneLeft{560.0f}; // Bu x değerinin solu sol panele aittir; 3B etkileşim sağında başlar

        void compute(int screenWidth, int screenHeight) {
            panelW      = Clamp(static_cast<float>(screenWidth) * 0.37f, 400.0f, 520.0f);
            panelBottom = static_cast<float>(screenHeight) - footerH - 14.0f;
            sceneLeft   = panelX + panelW + 12.0f;
        }
    };

    // ========================================================================
    // 4. ANA UYGULAMA KONTROLCÜSÜ (MAIN PORTFOLIO APPLICATION CONTROLLER)
    // ========================================================================
    class StudioPortfolioApp {
    private:
        int screenWidth{1400};
        int screenHeight{860};

        Camera3D camera{};
        Vector3 cameraTargetSmooth{-1.35f, 0.0f, 0.0f};
        float targetFov{44.0f};

        TypographyManager typo;
        LayoutMetrics layout;
        std::unique_ptr<CentralCore3D> centralCore;
        std::vector<std::unique_ptr<OrbitingNode3D>> orbitNodes;
        std::vector<SignalStream3D> signalStreams;
        std::unique_ptr<ParticleField3D> particleField;
        AnimatedBrandLogo brandLogo;

        // Durum Değişkenleri (Telemetry & Interactive State)
        int selectedOrbitIndex{0};
        float orbitSpeedMultiplier{1.0f};
        bool showSignalStreams{true};
        bool reducedMotion{false};
        bool cameraFollowSelected{false};
        bool showDeepDiveModal{false};
        bool modalOpenAtFrameStart{false}; // Modalı açan tıklamanın aynı karede "dışarı tıklama" sayılmasını önler
        bool showHelpOverlay{false};
        bool helpOpenAtFrameStart{false};
        bool jsonLoadedFromDisk{false};
        bool anyUiHovered{false};
        Vector2 smoothPointer{0.0f, 0.0f};

        // Sağ tık sürükleme ile serbest kamera yörüngesi (yaw/pitch radyan)
        float camYaw{0.0f};
        float camPitch{0.0f};
        float targetYaw{0.0f};
        float targetPitch{0.0f};
        bool isDraggingCamera{false};

        // projects.json dosya izleyicisi (kaydedildiğinde otomatik yeniden yükleme)
        std::string configPath{};
        long configModTime{0};
        float configWatchTimer{0.0f};

        // Bildirim (Toast Banner) Durumu
        std::string toastMessage{};
        float toastTimer{0.0f};

        void showToast(const std::string& msg, float duration = 3.0f) {
            toastMessage = msg;
            toastTimer = duration;
        }

        // Yapılandırmayı yükler ve sahneyi yeniden kurar; bir sorun varsa hata metnini döndürür
        std::string rebuildOrbitsFromConfig() {
            ConfigLoadResult result = PortfolioConfigLoader::loadFromJsonFile("projects.json");
            jsonLoadedFromDisk = result.loadedFromFile;
            configPath = result.sourcePath;
            configModTime = FileExists(configPath.c_str()) ? GetFileModTime(configPath.c_str()) : 0;
            const auto& configs = result.orbits;
            orbitNodes.clear();
            signalStreams.clear();

            const float arcHeights[] = { 0.45f, -0.45f, 0.55f, -0.35f };
            for (size_t i = 0; i < configs.size(); ++i) {
                orbitNodes.push_back(std::make_unique<OrbitingNode3D>(configs[i]));
            }
            for (size_t i = 0; i < orbitNodes.size(); ++i) {
                float arc = arcHeights[i % 4];
                signalStreams.emplace_back(*centralCore, *orbitNodes[i], arc);
            }

            if (selectedOrbitIndex >= static_cast<int>(orbitNodes.size())) {
                selectedOrbitIndex = 0;
            }
            selectOrbitNode(selectedOrbitIndex);
            return result.error;
        }

        void reloadConfigWithToast(bool automatic = false) {
            std::string error = rebuildOrbitsFromConfig();
            if (!error.empty()) {
                showToast("projects.json ERROR (" + error + ") // USING BUILT-IN FALLBACK", 5.0f);
            } else if (jsonLoadedFromDisk) {
                showToast(automatic ? "FILE CHANGE DETECTED // projects.json AUTO-RELOADED"
                                    : "HOT-RELOADED: projects.json SUCCESSFULLY APPLIED", 2.6f);
            } else {
                showToast("RELOADED BUILT-IN FALLBACK CONFIG", 2.6f);
            }
        }

        // Dosyanın değişim zamanını yarım saniyede bir kontrol eder (her karede disk erişimi yapmamak için)
        void watchConfigFile(float dt) {
            configWatchTimer += dt;
            if (configWatchTimer < 0.5f) return;
            configWatchTimer = 0.0f;

            if (configPath.empty() || !FileExists(configPath.c_str())) return;
            long modTime = GetFileModTime(configPath.c_str());
            if (modTime != configModTime) {
                reloadConfigWithToast(true);
            }
        }

        void resetCameraView() {
            targetYaw = 0.0f;
            targetPitch = 0.0f;
            cameraFollowSelected = false;
            targetFov = 44.0f;
        }

        bool isPointInScene(Vector2 p) const {
            return p.x >= layout.sceneLeft && p.y >= layout.headerH &&
                   p.y <= static_cast<float>(screenHeight) - layout.footerH;
        }

        static std::string timestampedScreenshotName() {
            std::time_t now = std::time(nullptr);
            char buf[64];
            std::strftime(buf, sizeof(buf), "fmcoskun_capture_%Y%m%d_%H%M%S.png", std::localtime(&now));
            return buf;
        }

        void initializeSceneData() {
            camera.position   = { 0.0f, 1.65f, 8.2f };
            camera.target     = { -1.35f, 0.0f, 0.0f };
            camera.up         = { 0.0f, 1.0f, 0.0f };
            camera.fovy       = 44.0f;
            camera.projection = CAMERA_PERSPECTIVE;

            centralCore = std::make_unique<CentralCore3D>("FMCOSKUN-01", "Central Studio Core");
            particleField = std::make_unique<ParticleField3D>(240);

            std::string error = rebuildOrbitsFromConfig();
            if (!error.empty()) {
                showToast("projects.json ERROR (" + error + ") // USING BUILT-IN FALLBACK", 6.0f);
            }
        }

        void selectOrbitNode(int index) {
            if (index < 0 || index >= static_cast<int>(orbitNodes.size())) return;
            selectedOrbitIndex = index;
            for (size_t i = 0; i < orbitNodes.size(); ++i) {
                orbitNodes[i]->setHighlighted(static_cast<int>(i) == selectedOrbitIndex);
            }
        }

        // Arka plan katmanları (header, sol panel) modal açıkken fare girdisi almaz
        bool isBackgroundInteractive() const {
            return !showDeepDiveModal && !showHelpOverlay;
        }

        void clearSceneHover() {
            centralCore->setHovered(false);
            for (auto& node : orbitNodes) node->setHovered(false);
        }

        bool drawInteractiveButton(Rectangle rect, const std::string& label, bool primaryStyle, bool enabled = true) {
            Vector2 mouse = GetMousePosition();
            bool hovered = enabled && CheckCollisionPointRec(mouse, rect);
            if (hovered) anyUiHovered = true;

            Color bg = primaryStyle
                ? (hovered ? Tokens::ACCENT_SECONDARY : Tokens::ACCENT_PRIMARY)
                : (hovered ? Tokens::BG_ELEVATED : Tokens::BG_PRIMARY);
            Color border = primaryStyle
                ? Tokens::ACCENT_PRIMARY
                : (hovered ? Tokens::ACCENT_PRIMARY : Tokens::BORDER_COLOR);
            Color textCol = primaryStyle ? Tokens::TEXT_ON_BRIGHT : Tokens::ACCENT_PRIMARY;

            DrawRectangleRounded(rect, 0.18f, 6, bg);
            DrawRectangleLinesEx(rect, hovered ? 1.5f : 1.0f, border);

            float textW = typo.measureMono(label, 11.0f);
            float textX = rect.x + (rect.width - textW) * 0.5f;
            float textY = rect.y + (rect.height - 11.0f) * 0.5f;
            typo.drawMono(label, textX, textY, 11.0f, textCol);

            return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        }

        void perform3DRaycasting(Vector2 mouse) {
            // Sol panel veya modal açıksa 3B sahne tıklamasını engelle
            if (!isBackgroundInteractive() || isDraggingCamera || !isPointInScene(mouse)) {
                clearSceneHover();
                return;
            }

            Ray ray = GetMouseRay(mouse, camera);
            int hoveredIdx = -1;
            float closestDist = 999999.0f;

            for (size_t i = 0; i < orbitNodes.size(); ++i) {
                RayCollision col = GetRayCollisionSphere(ray, orbitNodes[i]->getPosition(), orbitNodes[i]->getCollisionRadius());
                if (col.hit && col.distance < closestDist) {
                    closestDist = col.distance;
                    hoveredIdx = static_cast<int>(i);
                }
            }

            for (size_t i = 0; i < orbitNodes.size(); ++i) {
                bool isHov = (static_cast<int>(i) == hoveredIdx);
                orbitNodes[i]->setHovered(isHov);
                if (isHov) {
                    anyUiHovered = true;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectOrbitNode(static_cast<int>(i));
                        showToast("LOCKED ON " + orbitNodes[i]->getId() + " // " + orbitNodes[i]->getLabel(), 2.2f);
                    }
                }
            }

            // Merkez çekirdek raycast kontrolü
            if (hoveredIdx == -1) {
                RayCollision coreCol = GetRayCollisionSphere(ray, centralCore->getPosition(), centralCore->getCollisionRadius());
                centralCore->setHovered(coreCol.hit);
                if (coreCol.hit) {
                    anyUiHovered = true;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        centralCore->triggerPulseWave();
                        resetCameraView();
                        showToast("CENTRAL CORE PULSE TRIGGERED // CAMERA RESET", 2.2f);
                    }
                }
            } else {
                centralCore->setHovered(false);
            }
        }

        void handleInput(float dt) {
            anyUiHovered = false;
            Vector2 mouse = GetMousePosition();

            // Yardım paneli açıkken yalnızca kapatma tuşları dinlenir
            if (showHelpOverlay) {
                clearSceneHover();
                if (IsKeyPressed(KEY_H) || IsKeyPressed(KEY_ESCAPE)) {
                    showHelpOverlay = false;
                }
                return;
            }

            // Modal açıkken ESC ile kapatma
            if (showDeepDiveModal) {
                clearSceneHover();
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                    showDeepDiveModal = false;
                }
                return;
            }

            // [1]..[9] (üst sıra veya numpad) doğrudan seçim, TAB / SHIFT+TAB sırayla gezinme
            for (int k = 0; k < 9; ++k) {
                if (IsKeyPressed(KEY_ONE + k) || IsKeyPressed(KEY_KP_1 + k)) selectOrbitNode(k);
            }
            if (IsKeyPressed(KEY_TAB) && !orbitNodes.empty()) {
                int count = static_cast<int>(orbitNodes.size());
                bool backwards = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
                selectOrbitNode((selectedOrbitIndex + (backwards ? count - 1 : 1)) % count);
            }
            if (IsKeyPressed(KEY_H)) {
                showHelpOverlay = true;
            }
            if (IsKeyPressed(KEY_C)) {
                resetCameraView();
                showToast("CAMERA VIEW RESET", 1.8f);
            }

            if (IsKeyPressed(KEY_ENTER)) {
                showDeepDiveModal = true;
            }
            if (IsKeyPressed(KEY_S)) {
                showSignalStreams = !showSignalStreams;
                showToast(std::string("SIGNAL STREAMS: ") + (showSignalStreams ? "ENABLED" : "DISABLED"), 1.8f);
            }
            if (IsKeyPressed(KEY_R)) {
                reducedMotion = !reducedMotion;
                showToast(std::string("MOTION MODE: ") + (reducedMotion ? "STATIC (REDUCED MOTION)" : "ORBITAL DYNAMIC"), 2.0f);
            }
            if (IsKeyPressed(KEY_F)) {
                cameraFollowSelected = !cameraFollowSelected;
                showToast(cameraFollowSelected
                    ? ("CAMERA LOCKED TO " + orbitNodes[selectedOrbitIndex]->getId())
                    : "CAMERA OVERVIEW MODE", 2.2f);
            }
            if (IsKeyPressed(KEY_L)) {
                reloadConfigWithToast();
            }
            if (IsKeyPressed(KEY_SPACE)) {
                brandLogo.triggerFlip();
            }
            if (IsKeyPressed(KEY_F11)) {
                if (!IsWindowFullscreen()) {
                    int mon = GetCurrentMonitor();
                    SetWindowSize(GetMonitorWidth(mon), GetMonitorHeight(mon));
                    ToggleFullscreen();
                } else {
                    ToggleFullscreen();
                    SetWindowSize(1400, 860);
                }
                showToast("DISPLAY MODE: FULLSCREEN TOGGLED", 2.0f);
            }
            if (IsKeyPressed(KEY_F12)) {
                std::string shotName = timestampedScreenshotName();
                TakeScreenshot(shotName.c_str());
                showToast("SCREENSHOT SAVED: " + shotName, 3.0f);
            }

            if (IsKeyDown(KEY_UP)) {
                orbitSpeedMultiplier = Clamp(orbitSpeedMultiplier + dt * 0.8f, 0.2f, 2.6f);
            }
            if (IsKeyDown(KEY_DOWN)) {
                orbitSpeedMultiplier = Clamp(orbitSpeedMultiplier - dt * 0.8f, 0.2f, 2.6f);
            }

            // Fare Tekerleği ile Smooth FOV Zoom
            float wheel = GetMouseWheelMove();
            if (std::abs(wheel) > 0.01f) {
                targetFov = Clamp(targetFov - wheel * 3.2f, 24.0f, 58.0f);
            }
            camera.fovy += (targetFov - camera.fovy) * Clamp(dt * 10.0f, 0.0f, 1.0f);

            // Fare Paralaks Hesaplaması
            float normX = (mouse.x / static_cast<float>(screenWidth)) * 2.0f - 1.0f;
            float normY = (mouse.y / static_cast<float>(screenHeight)) * 2.0f - 1.0f;

            smoothPointer.x += (normX - smoothPointer.x) * 0.05f;
            smoothPointer.y += (normY - smoothPointer.y) * 0.05f;

            // Sağ Tık Sürükleme ile Kamera Yörüngesi (sadece 3B sahne alanında başlar)
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && isPointInScene(mouse)) {
                isDraggingCamera = true;
            }
            if (!IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                isDraggingCamera = false;
            }
            if (isDraggingCamera) {
                Vector2 delta = GetMouseDelta();
                targetYaw   -= delta.x * 0.006f;
                targetPitch  = Clamp(targetPitch + delta.y * 0.004f, -0.45f, 0.60f);
            }
            float orbitLerp = Clamp(dt * 8.0f, 0.0f, 1.0f);
            camYaw   += (targetYaw - camYaw) * orbitLerp;
            camPitch += (targetPitch - camPitch) * orbitLerp;
            Matrix orbitRotation = MatrixMultiply(MatrixRotateX(-camPitch), MatrixRotateY(camYaw));

            // Paralaks kameranın yerel uzayında uygulanır, ardından yörünge dönüşü eklenir
            Vector3 localPos = reducedMotion
                ? Vector3{ 0.0f, 1.65f, 8.2f }
                : Vector3{ smoothPointer.x * 0.85f, 1.65f - smoothPointer.y * 0.55f, 8.2f };
            camera.position = Vector3Transform(localPos, orbitRotation);

            // Kamera Hedef Kilidi (Overview veya Seçili 3B Uydu Takibi)
            Vector3 desiredTarget = Vector3Transform({ -1.35f, 0.0f, 0.0f }, orbitRotation);
            if (cameraFollowSelected && !orbitNodes.empty()) {
                Vector3 nodePos = orbitNodes[selectedOrbitIndex]->getPosition();
                desiredTarget = Vector3Add({ nodePos.x, nodePos.y * 0.65f, nodePos.z * 0.65f },
                                           Vector3Transform({ -0.85f, 0.0f, 0.0f }, orbitRotation));
            }
            float lerpFactor = Clamp(dt * 5.5f, 0.0f, 1.0f);
            cameraTargetSmooth = Vector3Lerp(cameraTargetSmooth, desiredTarget, lerpFactor);
            camera.target = cameraTargetSmooth;

            if (brandLogo.update(dt, mouse, {28.0f, 15.0f, 148.0f, 34.0f}, reducedMotion)) {
                anyUiHovered = true;
            }

            perform3DRaycasting(mouse);
        }

        void updateSimulation(float dt) {
            if (toastTimer > 0.0f) {
                toastTimer = std::max(0.0f, toastTimer - dt);
            }
            centralCore->update(dt, orbitSpeedMultiplier, reducedMotion);
            for (auto& node : orbitNodes) {
                node->update(dt, orbitSpeedMultiplier, reducedMotion);
            }
            if (showSignalStreams) {
                for (auto& stream : signalStreams) {
                    stream.update(dt, reducedMotion);
                }
            }
            particleField->update(dt, reducedMotion);
            watchConfigFile(dt);
        }

        // 3B Uzaydaki Uyduların Üzerinde Yüzen 2B Telemetri Etiketleri (World-to-Screen)
        void draw3DWorldLabelsHUD() {
            for (size_t i = 0; i < orbitNodes.size(); ++i) {
                const auto& node = orbitNodes[i];
                Vector2 screenPos = GetWorldToScreen(node->getPosition(), camera);

                // Sol panelin arkasına geçen etiketleri gizle
                if (screenPos.x < layout.sceneLeft + 5.0f || screenPos.x > static_cast<float>(screenWidth - 20) ||
                    screenPos.y < layout.headerH + 11.0f  || screenPos.y > static_cast<float>(screenHeight) - layout.footerH - 13.0f) {
                    continue;
                }

                bool active = node->isHighlighted();
                bool hov    = node->isHovered();
                Color col   = (active || hov) ? Tokens::ACCENT_PRIMARY : Fade(Tokens::TEXT_SECONDARY, 0.72f);

                // Nişangah (Reticle) Köşe Çizgileri
                int rx = static_cast<int>(screenPos.x);
                int ry = static_cast<int>(screenPos.y);
                int rSize = active ? 16 : (hov ? 14 : 10);
                DrawRectangleLines(rx - rSize, ry - rSize, rSize * 2, rSize * 2,
                                   Fade(col, (active || hov) ? 0.85f : 0.35f));

                // Çapraz Lider Çizgisi ve Bilgi Rozeti
                DrawLine(rx + rSize, ry - rSize, rx + rSize + 14, ry - rSize - 10, col);
                float tagX = static_cast<float>(rx + rSize + 16);
                float tagY = static_cast<float>(ry - rSize - 20);

                std::string tagText = "[" + std::to_string(i + 1) + "] " + node->getId();
                float tagW = typo.measureMono(tagText, 11.0f) + 14.0f;

                DrawRectangle(static_cast<int>(tagX), static_cast<int>(tagY),
                              static_cast<int>(tagW), 20, Fade(Tokens::BG_SURFACE, 0.88f));
                DrawRectangleLines(static_cast<int>(tagX), static_cast<int>(tagY),
                                   static_cast<int>(tagW), 20,
                                   (active || hov) ? Tokens::ACCENT_PRIMARY : Tokens::BORDER_COLOR);
                typo.drawMono(tagText, tagX + 7.0f, tagY + 4.0f, 11.0f, col);
            }
        }

        void drawHeaderHUD() {
            DrawRectangle(0, 0, screenWidth, static_cast<int>(layout.headerH), Tokens::BG_HEADER);
            DrawLine(0, static_cast<int>(layout.headerH), screenWidth, static_cast<int>(layout.headerH), Tokens::BORDER_COLOR);

            brandLogo.draw(28, 15, typo, jsonLoadedFromDisk);

            // Butonlar sağ kenara hizalanır; genişlikleri sabit, konumları pencere genişliğinden türetilir
            const float camW = 165.0f, motW = 180.0f, specW = 202.0f, gap = 10.0f;
            float rightX = static_cast<float>(screenWidth) - layout.margin - (camW + motW + specW + gap * 2.0f);
            const bool enabled = isBackgroundInteractive();

            // [F] Kamera Takip Rozeti
            Rectangle camRect = {rightX, 16.0f, camW, 32.0f};
            if (drawInteractiveButton(camRect, cameraFollowSelected ? "[F] CAM: LOCKED" : "[F] CAM: OVERVIEW", cameraFollowSelected, enabled)) {
                cameraFollowSelected = !cameraFollowSelected;
            }

            // [R] Hareket Modu Rozeti
            Rectangle motRect = {rightX + camW + gap, 16.0f, motW, 32.0f};
            if (drawInteractiveButton(motRect, reducedMotion ? "[R] MOTION: STATIC" : "[R] MOTION: ORBITAL", false, enabled)) {
                reducedMotion = !reducedMotion;
            }

            // [ENTER] Mimari İnceleme Butonu
            Rectangle specRect = {rightX + camW + motW + gap * 2.0f, 16.0f, specW, 32.0f};
            if (drawInteractiveButton(specRect, "[ENTER] DEEP-DIVE SPEC", true, enabled)) {
                showDeepDiveModal = true;
            }
        }

        // Inspector kartının içerik yüksekliğini çizmeden hesaplar (kart arka planı içerikten önce çizildiği için)
        float measureInspectorHeight(const ProjectSpec& proj, float innerW) const {
            float h = 90.0f;
            h += typo.measureWrappedUi(proj.summary, innerW, 14.0f, 4.0f) + 12.0f;
            h += 16.0f;
            h += typo.measureWrappedUi(proj.architectureNote, innerW, 14.0f, 4.0f) + 14.0f;
            h += measurePillRows(proj.techStack, innerW) * 32.0f;
            h += 10.0f + 38.0f + 16.0f;
            return h;
        }

        int measurePillRows(const std::vector<std::string>& tags, float innerW) const {
            if (tags.empty()) return 0;
            int rows = 1;
            float x = 0.0f;
            for (const auto& tag : tags) {
                float w = std::min(typo.measureMono(tag, 11.0f) + 18.0f, innerW);
                if (x > 0.0f && x + w > innerW) {
                    rows++;
                    x = 0.0f;
                }
                x += w + 8.0f;
            }
            return rows;
        }

        void drawLeftPortfolioDeck() {
            if (orbitNodes.empty()) return;

            const float panelX = layout.panelX;
            const float panelY = layout.panelY;
            const float panelW = layout.panelW;
            const float innerW = panelW - 32.0f;
            const bool enabled = isBackgroundInteractive();

            // Panel içeriği alt telemetri çubuğuna taşmasın
            BeginScissorMode(0, static_cast<int>(layout.headerH), static_cast<int>(layout.sceneLeft),
                             static_cast<int>(layout.panelBottom - layout.headerH));

            // 1. Üst Telemetri Etiketi
            DrawRectangle(static_cast<int>(panelX), static_cast<int>(panelY), 315, 24, Tokens::BG_SURFACE);
            DrawRectangleLines(static_cast<int>(panelX), static_cast<int>(panelY), 315, 24, Tokens::BORDER_COLOR);
            DrawCircle(static_cast<int>(panelX) + 12, static_cast<int>(panelY) + 12, 4.0f, Tokens::ACCENT_PRIMARY);
            typo.drawMono("FMCOSKUN STUDIO // ORBITAL PORTFOLIO", panelX + 24.0f, panelY + 6.0f, 11.0f, Tokens::TEXT_SECONDARY);

            // 2. Ana Başlık (Hero Editorial Heading)
            typo.drawUi("Streaming Your Success", panelX, panelY + 34.0f, 34.0f, Tokens::TEXT_PRIMARY);
            typo.drawUi("To New Orbits", panelX, panelY + 70.0f, 34.0f, Tokens::ACCENT_PRIMARY);

            float y = panelY + 114.0f;
            y += typo.drawWrappedUi("We transform ambitious ideas into intelligent digital experiences that move businesses forward.",
                                    panelX, y, panelW, 15.0f, 4.0f, Tokens::TEXT_SECONDARY);
            y += 14.0f;

            // 3. Yörünge Düğümü Seçici Kartlar (Interactive Node Switcher)
            std::string selectorHint = "SELECT ORBITAL TELEMETRY LOCK [KEYS 1-" + std::to_string(std::min<size_t>(orbitNodes.size(), 9)) +
                                       " OR CLICK 3D NODE]:";
            typo.drawMono(typo.fitMono(selectorHint, 11.0f, panelW), panelX, y, 11.0f, Tokens::ACCENT_PRIMARY);
            y += 20.0f;

            // Seçili projenin inspector yüksekliğine göre kartlar gerekirse kompakt moda geçer
            const ProjectSpec& proj = orbitNodes[selectedOrbitIndex]->getProjectSpec();
            const float inspectorH = measureInspectorHeight(proj, innerW);
            const float fullCardsH = static_cast<float>(orbitNodes.size()) * 52.0f;
            const bool compactCards = (y + fullCardsH + inspectorH > layout.panelBottom);
            const float cardH = compactCards ? 30.0f : 44.0f;
            const float cardStep = cardH + 8.0f;

            Vector2 mouse = GetMousePosition();
            for (size_t i = 0; i < orbitNodes.size(); ++i) {
                float cardY = y + static_cast<float>(i) * cardStep;
                Rectangle cardRect = { panelX, cardY, panelW, cardH };

                bool hovered = enabled && CheckCollisionPointRec(mouse, cardRect);
                if (hovered) {
                    anyUiHovered = true;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectOrbitNode(static_cast<int>(i));
                    }
                }

                bool active = (static_cast<int>(i) == selectedOrbitIndex);
                DrawRectangleRec(cardRect, active ? Tokens::BG_ELEVATED : Fade(Tokens::BG_SURFACE, 0.65f));
                DrawRectangleLinesEx(cardRect, active ? 1.5f : 1.0f,
                                     active ? Tokens::ACCENT_PRIMARY : (hovered ? Fade(Tokens::ACCENT_PRIMARY, 0.5f) : Tokens::BORDER_COLOR));

                std::ostringstream radStream;
                radStream << "R: " << std::fixed << std::setprecision(2) << orbitNodes[i]->getRadius() << " AU";
                float radW = typo.measureMono(radStream.str(), 11.0f);
                typo.drawMono(radStream.str(), panelX + panelW - 14.0f - radW, cardY + (cardH - 11.0f) * 0.5f, 11.0f, Tokens::TEXT_SECONDARY);

                std::string keyTag = "[" + std::to_string(i + 1) + "] " + orbitNodes[i]->getId();
                if (compactCards) {
                    // Tek satır: [1] ORBIT-ALPHA  Digital Experiences
                    float tagW = typo.measureMono(keyTag, 11.0f);
                    typo.drawMono(keyTag, panelX + 14.0f, cardY + 9.5f, 11.0f, Tokens::ACCENT_PRIMARY);
                    float labelX = panelX + 14.0f + tagW + 12.0f;
                    typo.drawUi(typo.fitUi(orbitNodes[i]->getLabel(), 14.0f, panelX + panelW - radW - 28.0f - labelX),
                                labelX, cardY + 7.0f, 14.0f, Tokens::TEXT_PRIMARY);
                } else {
                    typo.drawMono(keyTag, panelX + 14.0f, cardY + 7.0f, 11.0f, Tokens::ACCENT_PRIMARY);
                    typo.drawUi(typo.fitUi(orbitNodes[i]->getLabel(), 15.0f, panelW - radW - 42.0f),
                                panelX + 14.0f, cardY + 22.0f, 15.0f, Tokens::TEXT_PRIMARY);
                }
            }

            // 4. Seçili Yörüngenin Detaylı Konsept Proje Kartı (Project Inspector)
            const float inspectorY = y + static_cast<float>(orbitNodes.size()) * cardStep + 6.0f;
            Rectangle inspRect = { panelX, inspectorY, panelW, inspectorH };

            DrawRectangleRec(inspRect, Tokens::BG_SURFACE);
            DrawRectangleLinesEx(inspRect, 1.5f, Tokens::BORDER_COLOR);
            DrawLine(static_cast<int>(panelX), static_cast<int>(inspectorY) + 32,
                     static_cast<int>(panelX + panelW), static_cast<int>(inspectorY) + 32, Tokens::BORDER_COLOR);

            // Üst Bar: Proje Kodu & Statüsü
            const float textX = panelX + 16.0f;
            typo.drawMono(proj.code, textX, inspectorY + 10.0f, 11.0f, Tokens::ACCENT_PRIMARY);
            float statusW = typo.measureMono(proj.status, 11.0f);
            typo.drawMono(proj.status, panelX + panelW - 16.0f - statusW, inspectorY + 10.0f, 11.0f, Tokens::TEXT_SECONDARY);

            // Proje Başlığı ve Açıklaması
            typo.drawUi(typo.fitUi(proj.title, 22.0f, innerW), textX, inspectorY + 42.0f, 22.0f, Tokens::TEXT_PRIMARY);
            typo.drawMono(typo.fitMono(proj.category, 12.0f, innerW), textX, inspectorY + 68.0f, 12.0f, Tokens::ACCENT_PRIMARY);

            float iy = inspectorY + 90.0f;
            iy += typo.drawWrappedUi(proj.summary, textX, iy, innerW, 14.0f, 4.0f, Tokens::TEXT_SECONDARY) + 12.0f;

            typo.drawMono("ARCHITECTURAL SOLUTION:", textX, iy, 11.0f, Tokens::ACCENT_PRIMARY);
            iy += 16.0f;
            iy += typo.drawWrappedUi(proj.architectureNote, textX, iy, innerW, 14.0f, 4.0f, Tokens::TEXT_PRIMARY) + 14.0f;

            // Teknoloji Etiketleri (Tech Stack Pills) — sığmayan etiket alt satıra geçer
            float tagX = textX;
            for (const auto& tag : proj.techStack) {
                std::string shown = typo.fitMono(tag, 11.0f, innerW - 18.0f);
                float tagW = typo.measureMono(shown, 11.0f) + 18.0f;
                if (tagX > textX && tagX + tagW > textX + innerW) {
                    tagX = textX;
                    iy += 32.0f;
                }
                DrawRectangle(static_cast<int>(tagX), static_cast<int>(iy), static_cast<int>(tagW), 24, Tokens::BG_PRIMARY);
                DrawRectangleLines(static_cast<int>(tagX), static_cast<int>(iy), static_cast<int>(tagW), 24, Tokens::BORDER_COLOR);
                typo.drawMono(shown, tagX + 9.0f, iy + 6.0f, 11.0f, Tokens::ACCENT_PRIMARY);
                tagX += tagW + 8.0f;
            }
            if (!proj.techStack.empty()) iy += 32.0f;

            // 5. Canlı Etkileşim Aksiyon Butonları (OpenURL & Deep-Dive Modal)
            float btnY = iy + 10.0f;
            float btnW = (panelW - 48.0f) / 3.0f;

            if (drawInteractiveButton({textX, btnY, btnW, 38.0f}, "ARCH & METRICS", true, enabled)) {
                showDeepDiveModal = true;
            }
            if (drawInteractiveButton({textX + 8.0f + btnW, btnY, btnW, 38.0f}, "OPEN LIVE DEMO", false, enabled)) {
                OpenURL(proj.liveUrl.c_str());
                showToast("OPENING BROWSER: " + proj.liveUrl, 2.5f);
            }
            if (drawInteractiveButton({textX + 16.0f + btnW * 2.0f, btnY, btnW, 38.0f}, "GITHUB SOURCE", false, enabled)) {
                OpenURL(proj.repoUrl.c_str());
                showToast("OPENING REPOSITORY: " + proj.repoUrl, 2.5f);
            }

            EndScissorMode();
        }

        // Seçili Projenin Derin Mimari Diyagramı ve Telemetri Metrikleri Modalı
        void drawDeepDiveModal() {
            if (!showDeepDiveModal || orbitNodes.empty()) return;

            DrawRectangle(0, 0, screenWidth, screenHeight, Tokens::BG_MODAL_BACK);

            const auto& node = orbitNodes[selectedOrbitIndex];
            const ProjectSpec& proj = node->getProjectSpec();

            // Modal boyutu içeriğe göre hesaplanır, pencereye sığacak şekilde sınırlanır
            const float modalW = std::min(780.0f, static_cast<float>(screenWidth) - 80.0f);
            const float innerW = modalW - 48.0f;
            const std::string bodyText = proj.summary + " " + proj.architectureNote;
            const float bodyH = typo.measureWrappedUi(bodyText, innerW, 15.0f, 5.0f);
            const size_t stepCount = proj.pipeline.size();
            const float pipelineH = stepCount > 0 ? 24.0f + 56.0f + 22.0f : 0.0f;
            const float metricsH = proj.metrics.empty() ? 0.0f : 24.0f + static_cast<float>(proj.metrics.size()) * 42.0f;
            const float contentH = 64.0f + 32.0f + 28.0f + bodyH + 24.0f + pipelineH + metricsH + 16.0f + 34.0f + 16.0f;
            const float modalH = std::min(contentH, static_cast<float>(screenHeight) - 60.0f);
            const float modalX = (static_cast<float>(screenWidth) - modalW) * 0.5f;
            const float modalY = (static_cast<float>(screenHeight) - modalH) * 0.5f;
            Rectangle modalRect = { modalX, modalY, modalW, modalH };

            DrawRectangleRec(modalRect, Tokens::BG_SURFACE);
            DrawRectangleLinesEx(modalRect, 2.0f, Tokens::ACCENT_PRIMARY);
            DrawLine(static_cast<int>(modalX), static_cast<int>(modalY + 48),
                     static_cast<int>(modalX + modalW), static_cast<int>(modalY + 48), Tokens::BORDER_COLOR);

            typo.drawMono(typo.fitMono("DEEP-DIVE ARCHITECTURE & TELEMETRY BENCHMARK // " + proj.code, 12.0f, innerW - 110.0f),
                          modalX + 24.0f, modalY + 17.0f, 12.0f, Tokens::ACCENT_PRIMARY);

            if (drawInteractiveButton({modalX + modalW - 118.0f, modalY + 10.0f, 96.0f, 28.0f}, "[ESC] CLOSE", false)) {
                showDeepDiveModal = false;
            }

            // Modal dışındaki karartılmış alana tıklamak da modalı kapatır
            if (modalOpenAtFrameStart && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                !CheckCollisionPointRec(GetMousePosition(), modalRect)) {
                showDeepDiveModal = false;
            }

            // Alt butonlar her zaman görünür kalsın diye içerik, buton şeridinin üstünde kırpılır
            const float buttonsY = modalY + modalH - 50.0f;
            BeginScissorMode(static_cast<int>(modalX), static_cast<int>(modalY + 50.0f),
                             static_cast<int>(modalW), static_cast<int>(buttonsY - modalY - 58.0f));

            const float x = modalX + 24.0f;
            float y = modalY + 64.0f;
            typo.drawUi(typo.fitUi(proj.title, 26.0f, innerW), x, y, 26.0f, Tokens::TEXT_PRIMARY);
            y += 32.0f;
            typo.drawMono(typo.fitMono("ORBIT NODE: " + node->getId() + "   |   DOMAIN: " + proj.category + "   |   STATUS: " + proj.status,
                                       12.0f, innerW),
                          x, y, 12.0f, Tokens::TEXT_SECONDARY);
            y += 28.0f;

            y += typo.drawWrappedUi(bodyText, x, y, innerW, 15.0f, 5.0f, Tokens::TEXT_PRIMARY) + 24.0f;

            // 1. Mimari Akış Diyagramı (N Aşamalı Sistem Boru Hattı)
            if (stepCount > 0) {
                typo.drawMono("SYSTEM EXECUTION PIPELINE (END-TO-END TOPOLOGY):", x, y, 11.0f, Tokens::ACCENT_PRIMARY);
                y += 24.0f;

                float boxW = (innerW - static_cast<float>(stepCount - 1) * 28.0f) / static_cast<float>(stepCount);
                for (size_t i = 0; i < stepCount; ++i) {
                    float bx = x + static_cast<float>(i) * (boxW + 28.0f);
                    Rectangle stepRect = { bx, y, boxW, 56.0f };
                    DrawRectangleRec(stepRect, Tokens::BG_PRIMARY);
                    DrawRectangleLinesEx(stepRect, 1.5f, Tokens::ACCENT_PRIMARY);

                    std::ostringstream stepIndex;
                    stepIndex << "STAGE " << std::setw(2) << std::setfill('0') << (i + 1);
                    typo.drawMono(stepIndex.str(), bx + 12.0f, y + 10.0f, 10.0f, Tokens::TEXT_SECONDARY);
                    typo.drawUi(typo.fitUi(proj.pipeline[i], 14.0f, boxW - 20.0f), bx + 12.0f, y + 26.0f, 14.0f, Tokens::TEXT_PRIMARY);

                    if (i + 1 < stepCount) {
                        int arrowX = static_cast<int>(bx + boxW + 4.0f);
                        int arrowY = static_cast<int>(y + 28.0f);
                        DrawLine(arrowX, arrowY, arrowX + 20, arrowY, Tokens::ACCENT_PRIMARY);
                        DrawCircle(arrowX + 16, arrowY, 3.0f, Tokens::ACCENT_PRIMARY);
                    }
                }
                y += 56.0f + 22.0f;
            }

            // 2. Canlı Telemetri & SLA Performans Barları
            if (!proj.metrics.empty()) {
                typo.drawMono("VERIFIED TELEMETRY & BENCHMARK METRICS:", x, y, 11.0f, Tokens::ACCENT_PRIMARY);
                y += 24.0f;

                for (const auto& m : proj.metrics) {
                    float valueW = typo.measureMono(m.value, 13.0f);
                    typo.drawUi(typo.fitUi(m.label, 14.0f, innerW - valueW - 16.0f), x, y, 14.0f, Tokens::TEXT_PRIMARY);
                    typo.drawMono(m.value, x + innerW - valueW, y + 2.0f, 13.0f, Tokens::ACCENT_PRIMARY);

                    Rectangle barBg = { x, y + 20.0f, innerW, 10.0f };
                    Rectangle barFg = { x, y + 20.0f, innerW * Clamp(m.ratio, 0.0f, 1.0f), 10.0f };
                    DrawRectangleRec(barBg, Tokens::BG_PRIMARY);
                    DrawRectangleRec(barFg, Tokens::ACCENT_PRIMARY);
                    DrawRectangleLinesEx(barBg, 1.0f, Tokens::BORDER_COLOR);

                    y += 42.0f;
                }
            }

            EndScissorMode();

            // Alt Butonlar
            if (drawInteractiveButton({x, buttonsY, 220.0f, 34.0f}, "LAUNCH LIVE DEPLOYMENT", true)) {
                OpenURL(proj.liveUrl.c_str());
            }
            if (drawInteractiveButton({x + 234.0f, buttonsY, 220.0f, 34.0f}, "INSPECT SOURCE REPOSITORY", false)) {
                OpenURL(proj.repoUrl.c_str());
            }
        }

        // Klavye & Fare Kontrol Rehberi (H tuşu)
        void drawHelpOverlay() {
            if (!showHelpOverlay) return;

            struct ControlRow { const char* keys; const char* action; };
            static const ControlRow rows[] = {
                { "LEFT CLICK",       "Select a 3D satellite / pulse the central core" },
                { "RIGHT DRAG",       "Orbit the camera around the scene" },
                { "MOUSE WHEEL",      "Smooth FOV zoom" },
                { "1 - 9  /  TAB",    "Select orbit node (SHIFT+TAB: previous)" },
                { "ENTER",            "Open deep-dive architecture & metrics" },
                { "F",                "Lock camera to the selected satellite" },
                { "C",                "Reset camera angle and zoom" },
                { "S",                "Toggle signal streams" },
                { "R",                "Toggle reduced motion" },
                { "UP / DOWN",        "Orbit velocity multiplier" },
                { "L",                "Reload projects.json (also auto-reloads on save)" },
                { "SPACE",            "Replay the FMCOSKUN logo flip" },
                { "F11  /  F12",      "Fullscreen / save timestamped screenshot" },
                { "H  /  ESC",        "Close this panel" },
            };
            const int rowCount = static_cast<int>(sizeof(rows) / sizeof(rows[0]));

            DrawRectangle(0, 0, screenWidth, screenHeight, Tokens::BG_MODAL_BACK);

            const float panelW = 620.0f;
            const float rowH = 28.0f;
            const float panelH = 64.0f + rowH * static_cast<float>(rowCount) + 20.0f;
            const float panelX = (static_cast<float>(screenWidth) - panelW) * 0.5f;
            const float panelY = std::max(20.0f, (static_cast<float>(screenHeight) - panelH) * 0.5f);
            Rectangle panelRect = { panelX, panelY, panelW, panelH };

            DrawRectangleRec(panelRect, Tokens::BG_SURFACE);
            DrawRectangleLinesEx(panelRect, 2.0f, Tokens::ACCENT_PRIMARY);
            DrawLine(static_cast<int>(panelX), static_cast<int>(panelY + 48),
                     static_cast<int>(panelX + panelW), static_cast<int>(panelY + 48), Tokens::BORDER_COLOR);
            typo.drawMono("CONTROL REFERENCE // KEYBOARD & MOUSE", panelX + 24.0f, panelY + 17.0f, 12.0f, Tokens::ACCENT_PRIMARY);

            if (drawInteractiveButton({panelX + panelW - 118.0f, panelY + 10.0f, 96.0f, 28.0f}, "[H] CLOSE", false)) {
                showHelpOverlay = false;
            }
            if (helpOpenAtFrameStart && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                !CheckCollisionPointRec(GetMousePosition(), panelRect)) {
                showHelpOverlay = false;
            }

            float y = panelY + 64.0f;
            for (int i = 0; i < rowCount; ++i) {
                if (i % 2 == 0) {
                    DrawRectangle(static_cast<int>(panelX + 12.0f), static_cast<int>(y - 5.0f),
                                  static_cast<int>(panelW - 24.0f), static_cast<int>(rowH), Fade(Tokens::BG_PRIMARY, 0.6f));
                }
                typo.drawMono(rows[i].keys, panelX + 24.0f, y + 2.0f, 12.0f, Tokens::ACCENT_PRIMARY);
                typo.drawUi(rows[i].action, panelX + 190.0f, y, 15.0f, Tokens::TEXT_PRIMARY);
                y += rowH;
            }
        }

        void drawToastNotification() {
            if (toastTimer <= 0.0f || toastMessage.empty()) return;

            float alpha = Clamp(toastTimer * 1.5f, 0.0f, 1.0f);
            std::string shown = typo.fitMono(toastMessage, 12.0f, static_cast<float>(screenWidth) - 120.0f);
            float textW = typo.measureMono(shown, 12.0f);
            float boxW  = textW + 42.0f;
            float boxX  = (screenWidth - boxW) * 0.5f;
            float boxY  = layout.headerH + 12.0f;

            DrawRectangleRounded({boxX, boxY, boxW, 34.0f}, 0.25f, 6, Fade(Tokens::BG_ELEVATED, 0.95f * alpha));
            DrawRectangleLinesEx({boxX, boxY, boxW, 34.0f}, 1.5f, Fade(Tokens::ACCENT_PRIMARY, alpha));
            DrawCircle(static_cast<int>(boxX + 16.0f), static_cast<int>(boxY + 17.0f), 4.5f, Fade(Tokens::ACCENT_PRIMARY, alpha));
            typo.drawMono(shown, boxX + 28.0f, boxY + 11.0f, 12.0f, Fade(Tokens::TEXT_PRIMARY, alpha));
        }

        void drawBottomTelemetryBar() {
            if (orbitNodes.empty()) return;

            const float barY = static_cast<float>(screenHeight) - layout.footerH;
            DrawRectangle(0, static_cast<int>(barY), screenWidth, static_cast<int>(layout.footerH), Tokens::BG_SURFACE);
            DrawLine(0, static_cast<int>(barY), screenWidth, static_cast<int>(barY), Tokens::BORDER_COLOR);

            std::string coreTele = centralCore->getTelemetrySummary();
            std::string nodeTele = orbitNodes[selectedOrbitIndex]->getTelemetrySummary();

            const float textY = barY + 15.0f;
            float coreW = typo.measureMono(coreTele, 11.0f);
            float nodeX = layout.margin + coreW + 24.0f;
            float nodeEnd = nodeX + typo.measureMono(nodeTele, 11.0f);
            typo.drawMono(coreTele, layout.margin, textY, 11.0f, Tokens::ACCENT_PRIMARY);
            typo.drawMono(nodeTele, nodeX, textY, 11.0f, Tokens::TEXT_PRIMARY);

            std::ostringstream fov, vel;
            fov << "FOV: " << std::fixed << std::setprecision(0) << camera.fovy << " deg";
            vel << "VEL: " << std::fixed << std::setprecision(1) << (reducedMotion ? 0.0f : orbitSpeedMultiplier) << "x";
            const std::string fps = "FPS: " + std::to_string(GetFPS());

            const std::string fullHint = std::string("[S] STREAMS: ") + (showSignalStreams ? "ON" : "OFF")
                                       + "  |  [WHEEL] " + fov.str() + "  |  [UP/DN] " + vel.str()
                                       + "  |  [L] RELOAD JSON  |  [F11/F12] SCREEN  |  [H] HELP  |  " + fps;
            const std::string shortHint = fov.str() + "  |  " + vel.str() + "  |  [H] HELP  |  " + fps;

            // Dar pencerede uzun kısayol listesi yerine kısa özet gösterilir
            const float rightLimit = static_cast<float>(screenWidth) - layout.margin;
            std::string rightText = fullHint;
            if (rightLimit - typo.measureMono(rightText, 11.0f) < nodeEnd + 24.0f) {
                rightText = shortHint;
            }
            float rightTextW = typo.measureMono(rightText, 11.0f);
            if (rightLimit - rightTextW >= nodeEnd + 24.0f) {
                typo.drawMono(rightText, rightLimit - rightTextW, textY, 11.0f, Tokens::TEXT_SECONDARY);
            }
        }

    public:
        StudioPortfolioApp() {
            SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
            InitWindow(screenWidth, screenHeight, "FMCOSKUN STUDIO // Interactive 3D Desktop Portfolio v2.0 (C++17 OOP)");
            SetWindowMinSize(1100, 780);
            SetExitKey(KEY_NULL); // ESC tuşunun uygulamayı kapatmak yerine sadece modalı kapatmasını sağla
            SetTargetFPS(60);
            typo.initialize();
            layout.compute(screenWidth, screenHeight);
            initializeSceneData();
            if (toastTimer <= 0.0f) {
                showToast("ORBITAL PORTFOLIO INITIALIZED // CLICK ANY 3D SATELLITE OR PRESS [H] FOR CONTROLS", 3.5f);
            }
        }

        ~StudioPortfolioApp() {
            typo.shutdown();
            CloseWindow();
        }

        void run() {
            while (!WindowShouldClose()) {
                screenWidth  = GetScreenWidth();
                screenHeight = GetScreenHeight();
                layout.compute(screenWidth, screenHeight);
                float dt = GetFrameTime();
                modalOpenAtFrameStart = showDeepDiveModal;
                helpOpenAtFrameStart = showHelpOverlay;

                handleInput(dt);
                updateSimulation(dt);

                BeginDrawing();
                ClearBackground(Tokens::BG_PRIMARY);

                // 1. 3B YÖRÜNGE SAHNESİNİ ÇİZ (Three.js Sahnesinin C++ OpenGL Karşılığı)
                BeginMode3D(camera);
                particleField->draw3D();
                centralCore->draw3D();
                for (const auto& node : orbitNodes) {
                    node->draw3D();
                }
                if (showSignalStreams) {
                    for (const auto& stream : signalStreams) {
                        stream.draw3D();
                    }
                }
                EndMode3D();

                // 2. 3B UYDULARIN ÜZERİNDE YÜZEN 2B HEDEF ETİKETLERİ (World-to-Screen HUD)
                draw3DWorldLabelsHUD();

                // 3. 2B KULLANICI ARAYÜZÜ VE PORTFOLYO KARTLARINI ÇİZ (HUD Layer)
                drawHeaderHUD();
                drawLeftPortfolioDeck();
                drawBottomTelemetryBar();
                drawDeepDiveModal();
                drawHelpOverlay();
                drawToastNotification();

                // Etkileşimli elemanların üzerinde fare imlecini el ikonuna dönüştür
                SetMouseCursor(isDraggingCamera ? MOUSE_CURSOR_RESIZE_ALL
                               : (anyUiHovered ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT));

                EndDrawing();
            }
        }
    };

} // namespace FmCoskunStudio

// ============================================================================
// UYGULAMA GİRİŞ NOKTASI (MAIN ENTRY POINT)
// ============================================================================
int main() {
    FmCoskunStudio::StudioPortfolioApp app;
    app.run();
    return 0;
}
