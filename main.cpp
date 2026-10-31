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
 * - Fare Tekerleği    : 3B Sahneye Yakınlaş / Uzaklaş (Smooth FOV Zoom)
 * - [1], [2], [3]     : Yörünge Düğümlerini Seç (ORBIT-ALPHA, BETA, GAMMA)
 * - [ENTER]           : Seçili Projenin Derin Mimari & Metrik Modalını Aç
 * - [F] Tuşu          : Kamerayı Seçili 3B Uyduya Kilitle / Serbest Bırak
 * - [L] Tuşu          : projects.json Yapılandırmasını Canlı Yeniden Yükle
 * - [S] Tuşu          : Sinyal Akışlarını (Signal Streams) Aç / Kapat
 * - [R] Tuşu          : Reduced Motion (Statik / Hareketli Mod) Geçişi
 * - [Yukarı / Aşağı]  : Yörünge Hız Çarpanını (Velocity Multiplier) Ayarla
 * - [SPACE]           : FMCOSKUN Logosu 360° Karakter Dönüşünü Tetikle
 * - [F11] / [F12]     : Tam Ekran Geçişi / Ekran Görüntüsü Kaydet (.png)
 * ============================================================================
 */

#include "include/PortfolioConfig.hpp"
#include "include/OrbitalEntities.hpp"
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

        void drawWrappedUi(const std::string& text, float x, float y, float maxWidth, float fontSize, float lineGap, Color color) const {
            std::istringstream iss(text);
            std::string word;
            std::string currentLine;
            float currentY = y;

            while (iss >> word) {
                std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
                if (measureUi(testLine, fontSize) > maxWidth && !currentLine.empty()) {
                    drawUi(currentLine, x, currentY, fontSize, color);
                    currentLine = word;
                    currentY += fontSize + lineGap;
                } else {
                    currentLine = testLine;
                }
            }
            if (!currentLine.empty()) {
                drawUi(currentLine, x, currentY, fontSize, color);
            }
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
    // 3. ANA UYGULAMA KONTROLCÜSÜ (MAIN PORTFOLIO APPLICATION CONTROLLER)
    // ========================================================================
    class StudioPortfolioApp {
    private:
        int screenWidth{1400};
        int screenHeight{860};

        Camera3D camera{};
        Vector3 cameraTargetSmooth{-1.35f, 0.0f, 0.0f};
        float targetFov{44.0f};

        TypographyManager typo;
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
        bool jsonLoadedFromDisk{false};
        bool anyUiHovered{false};
        Vector2 smoothPointer{0.0f, 0.0f};

        // Bildirim (Toast Banner) Durumu
        std::string toastMessage{};
        float toastTimer{0.0f};

        void showToast(const std::string& msg, float duration = 3.0f) {
            toastMessage = msg;
            toastTimer = duration;
        }

        void rebuildOrbitsFromConfig() {
            auto configs = PortfolioConfigLoader::loadFromJsonFile("projects.json", jsonLoadedFromDisk);
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
        }

        void initializeSceneData() {
            camera.position   = { 0.0f, 1.65f, 8.2f };
            camera.target     = { -1.35f, 0.0f, 0.0f };
            camera.up         = { 0.0f, 1.0f, 0.0f };
            camera.fovy       = 44.0f;
            camera.projection = CAMERA_PERSPECTIVE;

            centralCore = std::make_unique<CentralCore3D>("FMCOSKUN-01", "Central Studio Core");
            particleField = std::make_unique<ParticleField3D>(240);

            rebuildOrbitsFromConfig();
        }

        void selectOrbitNode(int index) {
            if (index < 0 || index >= static_cast<int>(orbitNodes.size())) return;
            selectedOrbitIndex = index;
            for (size_t i = 0; i < orbitNodes.size(); ++i) {
                orbitNodes[i]->setHighlighted(static_cast<int>(i) == selectedOrbitIndex);
            }
        }

        bool drawInteractiveButton(Rectangle rect, const std::string& label, bool primaryStyle) {
            Vector2 mouse = GetMousePosition();
            bool hovered = CheckCollisionPointRec(mouse, rect);
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
            if (showDeepDiveModal || mouse.x < 560.0f || mouse.y < 64.0f || mouse.y > static_cast<float>(screenHeight - 42)) {
                centralCore->setHovered(false);
                for (auto& node : orbitNodes) node->setHovered(false);
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
                        cameraFollowSelected = false;
                        targetFov = 44.0f;
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

            // Modal açıkken ESC ile kapatma
            if (showDeepDiveModal) {
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                    showDeepDiveModal = false;
                }
                return;
            }

            if (IsKeyPressed(KEY_ONE))   selectOrbitNode(0);
            if (IsKeyPressed(KEY_TWO))   selectOrbitNode(1);
            if (IsKeyPressed(KEY_THREE)) selectOrbitNode(2);

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
                rebuildOrbitsFromConfig();
                showToast(jsonLoadedFromDisk
                    ? "HOT-RELOADED: projects.json SUCCESSFULLY APPLIED"
                    : "RELOADED BUILT-IN FALLBACK CONFIG", 2.6f);
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
                TakeScreenshot("fmcoskun_studio_capture.png");
                showToast("SCREENSHOT SAVED: fmcoskun_studio_capture.png", 3.0f);
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

            if (!reducedMotion) {
                camera.position.x = smoothPointer.x * 0.85f;
                camera.position.y = 1.65f - smoothPointer.y * 0.55f;
            } else {
                camera.position.x = 0.0f;
                camera.position.y = 1.65f;
            }

            // Kamera Hedef Kilidi (Overview veya Seçili 3B Uydu Takibi)
            Vector3 desiredTarget = { -1.35f, 0.0f, 0.0f };
            if (cameraFollowSelected && !orbitNodes.empty()) {
                Vector3 nodePos = orbitNodes[selectedOrbitIndex]->getPosition();
                desiredTarget = { nodePos.x - 0.85f, nodePos.y * 0.65f, nodePos.z * 0.65f };
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
        }

        // 3B Uzaydaki Uyduların Üzerinde Yüzen 2B Telemetri Etiketleri (World-to-Screen)
        void draw3DWorldLabelsHUD() {
            for (size_t i = 0; i < orbitNodes.size(); ++i) {
                const auto& node = orbitNodes[i];
                Vector2 screenPos = GetWorldToScreen(node->getPosition(), camera);

                // Sol panelin arkasına geçen etiketleri gizle
                if (screenPos.x < 565.0f || screenPos.x > static_cast<float>(screenWidth - 20) ||
                    screenPos.y < 75.0f  || screenPos.y > static_cast<float>(screenHeight - 55)) {
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
            DrawRectangle(0, 0, screenWidth, 64, Tokens::BG_HEADER);
            DrawLine(0, 64, screenWidth, 64, Tokens::BORDER_COLOR);

            brandLogo.draw(28, 15, typo, jsonLoadedFromDisk);

            int rightX = screenWidth - 595;

            // [F] Kamera Takip Rozeti
            Rectangle camRect = {static_cast<float>(rightX), 16.0f, 165.0f, 32.0f};
            if (drawInteractiveButton(camRect, cameraFollowSelected ? "[F] CAM: LOCKED" : "[F] CAM: OVERVIEW", cameraFollowSelected)) {
                cameraFollowSelected = !cameraFollowSelected;
            }

            // [R] Hareket Modu Rozeti
            Rectangle motRect = {static_cast<float>(rightX + 175), 16.0f, 180.0f, 32.0f};
            if (drawInteractiveButton(motRect, reducedMotion ? "[R] MOTION: STATIC" : "[R] MOTION: ORBITAL", false)) {
                reducedMotion = !reducedMotion;
            }

            // [ENTER] Mimari İnceleme Butonu
            Rectangle specRect = {static_cast<float>(rightX + 365), 16.0f, 202.0f, 32.0f};
            if (drawInteractiveButton(specRect, "[ENTER] DEEP-DIVE SPEC", true)) {
                showDeepDiveModal = true;
            }
        }

        void drawLeftPortfolioDeck() {
            const int panelX = 28;
            const int panelY = 82;
            const int panelW = 520;

            // 1. Üst Telemetri Etiketi
            DrawRectangle(panelX, panelY, 315, 24, Tokens::BG_SURFACE);
            DrawRectangleLines(panelX, panelY, 315, 24, Tokens::BORDER_COLOR);
            DrawCircle(panelX + 12, panelY + 12, 4.0f, Tokens::ACCENT_PRIMARY);
            typo.drawMono("FMCOSKUN STUDIO // ORBITAL PORTFOLIO", static_cast<float>(panelX + 24), static_cast<float>(panelY + 6), 11.0f, Tokens::TEXT_SECONDARY);

            // 2. Ana Başlık (Hero Editorial Heading)
            typo.drawUi("Streaming Your Success", static_cast<float>(panelX), static_cast<float>(panelY + 34), 34.0f, Tokens::TEXT_PRIMARY);
            typo.drawUi("To New Orbits", static_cast<float>(panelX), static_cast<float>(panelY + 70), 34.0f, Tokens::ACCENT_PRIMARY);

            typo.drawUi("We transform ambitious ideas into intelligent digital", static_cast<float>(panelX), static_cast<float>(panelY + 114), 15.0f, Tokens::TEXT_SECONDARY);
            typo.drawUi("experiences that move businesses forward.", static_cast<float>(panelX), static_cast<float>(panelY + 133), 15.0f, Tokens::TEXT_SECONDARY);

            // 3. Yörünge Düğümü Seçici Kartlar (Interactive Node Switcher)
            typo.drawMono("SELECT ORBITAL TELEMETRY LOCK [KEYS 1-3 OR CLICK 3D NODE]:", static_cast<float>(panelX), static_cast<float>(panelY + 166), 11.0f, Tokens::ACCENT_PRIMARY);

            Vector2 mouse = GetMousePosition();
            for (size_t i = 0; i < orbitNodes.size(); ++i) {
                float cardY = static_cast<float>(panelY + 186 + static_cast<int>(i) * 52);
                Rectangle cardRect = { static_cast<float>(panelX), cardY, static_cast<float>(panelW), 44.0f };

                bool hovered = CheckCollisionPointRec(mouse, cardRect);
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

                std::string keyTag = "[" + std::to_string(i + 1) + "] " + orbitNodes[i]->getId();
                typo.drawMono(keyTag, static_cast<float>(panelX + 14), cardY + 7.0f, 11.0f, Tokens::ACCENT_PRIMARY);
                typo.drawUi(orbitNodes[i]->getLabel(), static_cast<float>(panelX + 14), cardY + 22.0f, 15.0f, Tokens::TEXT_PRIMARY);

                std::ostringstream radStream;
                radStream << "R: " << std::fixed << std::setprecision(2) << orbitNodes[i]->getRadius() << " AU";
                typo.drawMono(radStream.str(), static_cast<float>(panelX + panelW - 92), cardY + 16.0f, 11.0f, Tokens::TEXT_SECONDARY);
            }

            // 4. Seçili Yörüngenin Detaylı Konsept Proje Kartı (Project Inspector)
            const ProjectSpec& proj = orbitNodes[selectedOrbitIndex]->getProjectSpec();
            float inspectorY = static_cast<float>(panelY + 350);
            Rectangle inspRect = { static_cast<float>(panelX), inspectorY, static_cast<float>(panelW), 302.0f };

            DrawRectangleRec(inspRect, Tokens::BG_SURFACE);
            DrawRectangleLinesEx(inspRect, 1.5f, Tokens::BORDER_COLOR);
            DrawLine(panelX, static_cast<int>(inspectorY) + 32, panelX + panelW, static_cast<int>(inspectorY) + 32, Tokens::BORDER_COLOR);

            // Üst Bar: Proje Kodu & Statüsü
            typo.drawMono(proj.code, static_cast<float>(panelX + 16), inspectorY + 10.0f, 11.0f, Tokens::ACCENT_PRIMARY);
            float statusW = typo.measureMono(proj.status, 11.0f);
            typo.drawMono(proj.status, static_cast<float>(panelX + panelW - 16) - statusW, inspectorY + 10.0f, 11.0f, Tokens::TEXT_SECONDARY);

            // Proje Başlığı ve Açıklaması
            typo.drawUi(proj.title, static_cast<float>(panelX + 16), inspectorY + 42.0f, 22.0f, Tokens::TEXT_PRIMARY);
            typo.drawMono(proj.category, static_cast<float>(panelX + 16), inspectorY + 68.0f, 12.0f, Tokens::ACCENT_PRIMARY);

            typo.drawWrappedUi(proj.summary, static_cast<float>(panelX + 16), inspectorY + 90.0f,
                               static_cast<float>(panelW - 32), 14.0f, 4.0f, Tokens::TEXT_SECONDARY);

            typo.drawMono("ARCHITECTURAL SOLUTION:", static_cast<float>(panelX + 16), inspectorY + 136.0f, 11.0f, Tokens::ACCENT_PRIMARY);
            typo.drawWrappedUi(proj.architectureNote, static_cast<float>(panelX + 16), inspectorY + 152.0f,
                               static_cast<float>(panelW - 32), 14.0f, 4.0f, Tokens::TEXT_PRIMARY);

            // Teknoloji Etiketleri (Tech Stack Pills)
            float tagX = static_cast<float>(panelX + 16);
            float tagY = inspectorY + 202.0f;
            for (const auto& tag : proj.techStack) {
                float tagW = typo.measureMono(tag, 11.0f) + 18.0f;
                DrawRectangle(static_cast<int>(tagX), static_cast<int>(tagY), static_cast<int>(tagW), 24, Tokens::BG_PRIMARY);
                DrawRectangleLines(static_cast<int>(tagX), static_cast<int>(tagY), static_cast<int>(tagW), 24, Tokens::BORDER_COLOR);
                typo.drawMono(tag, tagX + 9.0f, tagY + 6.0f, 11.0f, Tokens::ACCENT_PRIMARY);
                tagX += tagW + 8.0f;
            }

            // 5. Canlı Etkileşim Aksiyon Butonları (OpenURL & Deep-Dive Modal)
            float btnY = inspectorY + 244.0f;
            float btnW = (panelW - 48.0f) / 3.0f;

            if (drawInteractiveButton({static_cast<float>(panelX + 16), btnY, btnW, 38.0f}, "ARCH & METRICS", true)) {
                showDeepDiveModal = true;
            }
            if (drawInteractiveButton({static_cast<float>(panelX + 24) + btnW, btnY, btnW, 38.0f}, "OPEN LIVE DEMO", false)) {
                OpenURL(proj.liveUrl.c_str());
                showToast("OPENING BROWSER: " + proj.liveUrl, 2.5f);
            }
            if (drawInteractiveButton({static_cast<float>(panelX + 32) + btnW * 2.0f, btnY, btnW, 38.0f}, "GITHUB SOURCE", false)) {
                OpenURL(proj.repoUrl.c_str());
                showToast("OPENING REPOSITORY: " + proj.repoUrl, 2.5f);
            }
        }

        // Seçili Projenin Derin Mimari Diyagramı ve Telemetri Metrikleri Modalı
        void drawDeepDiveModal() {
            if (!showDeepDiveModal || orbitNodes.empty()) return;

            DrawRectangle(0, 0, screenWidth, screenHeight, Tokens::BG_MODAL_BACK);

            const float modalW = 780.0f;
            const float modalH = 500.0f;
            const float modalX = (screenWidth - modalW) * 0.5f;
            const float modalY = (screenHeight - modalH) * 0.5f;
            Rectangle modalRect = { modalX, modalY, modalW, modalH };

            DrawRectangleRec(modalRect, Tokens::BG_SURFACE);
            DrawRectangleLinesEx(modalRect, 2.0f, Tokens::ACCENT_PRIMARY);
            DrawLine(static_cast<int>(modalX), static_cast<int>(modalY + 48),
                     static_cast<int>(modalX + modalW), static_cast<int>(modalY + 48), Tokens::BORDER_COLOR);

            const auto& node = orbitNodes[selectedOrbitIndex];
            const ProjectSpec& proj = node->getProjectSpec();

            typo.drawMono("DEEP-DIVE ARCHITECTURE & TELEMETRY BENCHMARK // " + proj.code,
                          modalX + 24.0f, modalY + 17.0f, 12.0f, Tokens::ACCENT_PRIMARY);

            if (drawInteractiveButton({modalX + modalW - 118.0f, modalY + 10.0f, 96.0f, 28.0f}, "[ESC] CLOSE", false)) {
                showDeepDiveModal = false;
            }

            typo.drawUi(proj.title, modalX + 24.0f, modalY + 64.0f, 26.0f, Tokens::TEXT_PRIMARY);
            typo.drawMono("ORBIT NODE: " + node->getId() + "   |   DOMAIN: " + proj.category + "   |   STATUS: " + proj.status,
                          modalX + 24.0f, modalY + 96.0f, 12.0f, Tokens::TEXT_SECONDARY);

            typo.drawWrappedUi(proj.summary + " " + proj.architectureNote,
                               modalX + 24.0f, modalY + 124.0f, modalW - 48.0f, 15.0f, 5.0f, Tokens::TEXT_PRIMARY);

            // 1. Mimari Akış Diyagramı (4 Aşamalı Sistem Boru Hattı)
            typo.drawMono("SYSTEM EXECUTION PIPELINE (END-TO-END TOPOLOGY):", modalX + 24.0f, modalY + 190.0f, 11.0f, Tokens::ACCENT_PRIMARY);

            float pipeY = modalY + 214.0f;
            size_t stepCount = proj.pipeline.size();
            if (stepCount > 0) {
                float boxW = (modalW - 48.0f - static_cast<float>(stepCount - 1) * 28.0f) / static_cast<float>(stepCount);
                for (size_t i = 0; i < stepCount; ++i) {
                    float bx = modalX + 24.0f + static_cast<float>(i) * (boxW + 28.0f);
                    Rectangle stepRect = { bx, pipeY, boxW, 56.0f };
                    DrawRectangleRec(stepRect, Tokens::BG_PRIMARY);
                    DrawRectangleLinesEx(stepRect, 1.5f, Tokens::ACCENT_PRIMARY);

                    std::string stepIndex = "STAGE 0" + std::to_string(i + 1);
                    typo.drawMono(stepIndex, bx + 12.0f, pipeY + 10.0f, 10.0f, Tokens::TEXT_SECONDARY);
                    typo.drawUi(proj.pipeline[i], bx + 12.0f, pipeY + 26.0f, 14.0f, Tokens::TEXT_PRIMARY);

                    if (i + 1 < stepCount) {
                        int arrowX = static_cast<int>(bx + boxW + 4.0f);
                        int arrowY = static_cast<int>(pipeY + 28.0f);
                        DrawLine(arrowX, arrowY, arrowX + 20, arrowY, Tokens::ACCENT_PRIMARY);
                        DrawCircle(arrowX + 16, arrowY, 3.0f, Tokens::ACCENT_PRIMARY);
                    }
                }
            }

            // 2. Canlı Telemetri & SLA Performans Barları
            typo.drawMono("VERIFIED TELEMETRY & BENCHMARK METRICS:", modalX + 24.0f, modalY + 292.0f, 11.0f, Tokens::ACCENT_PRIMARY);

            float metricY = modalY + 316.0f;
            for (const auto& m : proj.metrics) {
                typo.drawUi(m.label, modalX + 24.0f, metricY, 14.0f, Tokens::TEXT_PRIMARY);
                typo.drawMono(m.value, modalX + modalW - 110.0f, metricY + 2.0f, 13.0f, Tokens::ACCENT_PRIMARY);

                Rectangle barBg = { modalX + 24.0f, metricY + 20.0f, modalW - 48.0f, 10.0f };
                Rectangle barFg = { modalX + 24.0f, metricY + 20.0f, (modalW - 48.0f) * Clamp(m.ratio, 0.0f, 1.0f), 10.0f };
                DrawRectangleRec(barBg, Tokens::BG_PRIMARY);
                DrawRectangleRec(barFg, Tokens::ACCENT_PRIMARY);
                DrawRectangleLinesEx(barBg, 1.0f, Tokens::BORDER_COLOR);

                metricY += 42.0f;
            }

            // Alt Butonlar
            if (drawInteractiveButton({modalX + 24.0f, modalY + modalH - 50.0f, 220.0f, 34.0f}, "LAUNCH LIVE DEPLOYMENT", true)) {
                OpenURL(proj.liveUrl.c_str());
            }
            if (drawInteractiveButton({modalX + 258.0f, modalY + modalH - 50.0f, 220.0f, 34.0f}, "INSPECT SOURCE REPOSITORY", false)) {
                OpenURL(proj.repoUrl.c_str());
            }
        }

        void drawToastNotification() {
            if (toastTimer <= 0.0f || toastMessage.empty()) return;

            float alpha = Clamp(toastTimer * 1.5f, 0.0f, 1.0f);
            float textW = typo.measureMono(toastMessage, 12.0f);
            float boxW  = textW + 42.0f;
            float boxX  = (screenWidth - boxW) * 0.5f;
            float boxY  = 76.0f;

            DrawRectangleRounded({boxX, boxY, boxW, 34.0f}, 0.25f, 6, Fade(Tokens::BG_ELEVATED, 0.95f * alpha));
            DrawRectangleLinesEx({boxX, boxY, boxW, 34.0f}, 1.5f, Fade(Tokens::ACCENT_PRIMARY, alpha));
            DrawCircle(static_cast<int>(boxX + 16.0f), static_cast<int>(boxY + 17.0f), 4.5f, Fade(Tokens::ACCENT_PRIMARY, alpha));
            typo.drawMono(toastMessage, boxX + 28.0f, boxY + 11.0f, 12.0f, Fade(Tokens::TEXT_PRIMARY, alpha));
        }

        void drawBottomTelemetryBar() {
            int barY = screenHeight - 42;
            DrawRectangle(0, barY, screenWidth, 42, Tokens::BG_SURFACE);
            DrawLine(0, barY, screenWidth, barY, Tokens::BORDER_COLOR);

            std::string coreTele = centralCore->getTelemetrySummary();
            std::string nodeTele = orbitNodes[selectedOrbitIndex]->getTelemetrySummary();

            typo.drawMono(coreTele, 28.0f, static_cast<float>(barY + 15), 11.0f, Tokens::ACCENT_PRIMARY);
            typo.drawMono(nodeTele, 285.0f, static_cast<float>(barY + 15), 11.0f, Tokens::TEXT_PRIMARY);

            std::ostringstream ctrlStream;
            ctrlStream << "[S] STREAMS: " << (showSignalStreams ? "ON" : "OFF")
                       << "  |  [WHEEL] FOV: " << std::fixed << std::setprecision(0) << camera.fovy << " deg"
                       << "  |  [UP/DN] VEL: " << std::setprecision(1) << (reducedMotion ? 0.0f : orbitSpeedMultiplier) << "x"
                       << "  |  [L] RELOAD JSON  |  [F11/F12] SCREEN  |  FPS: " << GetFPS();

            float rightTextW = typo.measureMono(ctrlStream.str(), 11.0f);
            typo.drawMono(ctrlStream.str(), static_cast<float>(screenWidth - 28) - rightTextW,
                          static_cast<float>(barY + 15), 11.0f, Tokens::TEXT_SECONDARY);
        }

    public:
        StudioPortfolioApp() {
            SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
            InitWindow(screenWidth, screenHeight, "FMCOSKUN STUDIO // Interactive 3D Desktop Portfolio v2.0 (C++17 OOP)");
            SetExitKey(KEY_NULL); // ESC tuşunun uygulamayı kapatmak yerine sadece modalı kapatmasını sağla
            SetTargetFPS(60);
            typo.initialize();
            initializeSceneData();
            showToast("ORBITAL PORTFOLIO INITIALIZED // CLICK ANY 3D SATELLITE OR PRESS [ENTER]", 3.5f);
        }

        ~StudioPortfolioApp() {
            typo.shutdown();
            CloseWindow();
        }

        void run() {
            while (!WindowShouldClose()) {
                screenWidth  = GetScreenWidth();
                screenHeight = GetScreenHeight();
                float dt = GetFrameTime();

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
                drawToastNotification();

                // Etkileşimli elemanların üzerinde fare imlecini el ikonuna dönüştür
                SetMouseCursor(anyUiHovered ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);

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
