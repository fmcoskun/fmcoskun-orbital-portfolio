#ifndef FMCOSKUN_ORBITAL_ENTITIES_HPP
#define FMCOSKUN_ORBITAL_ENTITIES_HPP

#include "PortfolioConfig.hpp"
#include "raymath.h"
#include "rlgl.h"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace FmCoskunStudio {

    // ========================================================================
    // 1. SOYUT 3B YÖRÜNGE NESNESİ SINIFI (ABSTRACT BASE CLASS - OOP)
    // ========================================================================
    class OrbitalEntity3D {
    protected:
        std::string id;
        std::string label;
        Vector3 position;
        bool highlighted{false};
        bool hovered{false};

    public:
        OrbitalEntity3D(std::string entityId, std::string entityLabel, Vector3 initialPos = {0.0f, 0.0f, 0.0f})
            : id(std::move(entityId)), label(std::move(entityLabel)), position(initialPos) {}

        virtual ~OrbitalEntity3D() = default;

        // Saf sanal (Pure Virtual) fonksiyonlar: Çok Biçimlilik (Polymorphism)
        virtual void update(float dt, float speedMultiplier, bool reducedMotion) = 0;
        virtual void draw3D() const = 0;
        virtual std::string getTelemetrySummary() const = 0;
        virtual float getCollisionRadius() const = 0;

        // Kapsülleme (Encapsulation) Getter & Setter metotları
        const std::string& getId() const { return id; }
        const std::string& getLabel() const { return label; }
        Vector3 getPosition() const { return position; }
        void setHighlighted(bool state) { highlighted = state; }
        bool isHighlighted() const { return highlighted; }
        void setHovered(bool state) { hovered = state; }
        bool isHovered() const { return hovered; }
    };

    // ========================================================================
    // 2. MERKEZİ ÇEKİRDEK SINIFI (CENTRAL CORE - INHERITANCE)
    // ========================================================================
    class CentralCore3D : public OrbitalEntity3D {
    private:
        float outerRotationY{0.0f};
        float wireRotationY{0.0f};
        float innerPulse{0.0f};
        float shockwaveProgress{1.0f};

    public:
        CentralCore3D(std::string coreId, std::string coreLabel)
            : OrbitalEntity3D(std::move(coreId), std::move(coreLabel), {0.0f, 0.0f, 0.0f}) {}

        void triggerPulseWave() {
            shockwaveProgress = 0.0f;
        }

        void update(float dt, float speedMultiplier, bool reducedMotion) override {
            if (shockwaveProgress < 1.0f) {
                shockwaveProgress = Clamp(shockwaveProgress + dt * 1.35f, 0.0f, 1.0f);
            }
            if (reducedMotion) return;
            outerRotationY += dt * 18.0f * speedMultiplier;
            wireRotationY  -= dt * 26.0f * speedMultiplier;
            innerPulse     += dt * 2.5f;
        }

        void draw3D() const override {
            // İç Parlayan Çekirdek (Glowing Nucleus)
            float pulseRadius = 0.42f + std::sin(innerPulse) * 0.04f;
            DrawSphere(position, pulseRadius, Fade(Tokens::ACCENT_PRIMARY, hovered ? 0.95f : 0.80f));

            // Koyu Metalik İç Gövde
            DrawSphereEx(position, 0.88f, 14, 14, Tokens::BG_SURFACE);

            // Neon Yeşil Tel Kafes (Wireframe Lattice)
            rlPushMatrix();
            rlTranslatef(position.x, position.y, position.z);
            rlRotatef(wireRotationY, 0.0f, 1.0f, 0.0f);
            DrawSphereWires({0.0f, 0.0f, 0.0f}, 0.96f, 8, 8,
                            Fade(Tokens::ACCENT_PRIMARY, hovered ? 0.72f : 0.42f));
            rlPopMatrix();

            rlPushMatrix();
            rlTranslatef(position.x, position.y, position.z);
            rlRotatef(outerRotationY, 0.0f, 1.0f, 0.0f);
            DrawCubeWiresV({0.0f, 0.0f, 0.0f}, {1.15f, 1.15f, 1.15f},
                           Fade(hovered ? Tokens::ACCENT_PRIMARY : Tokens::BORDER_COLOR, 0.85f));
            rlPopMatrix();

            // Etkileşimli Enerji Dalga Efekti (Tıklandığında yayılan şok halkası)
            if (shockwaveProgress < 1.0f) {
                float waveRadius = 1.0f + shockwaveProgress * 3.5f;
                float waveAlpha  = (1.0f - shockwaveProgress) * 0.75f;
                DrawSphereWires(position, waveRadius, 10, 10, Fade(Tokens::ACCENT_PRIMARY, waveAlpha));
            }
        }

        std::string getTelemetrySummary() const override {
            std::ostringstream oss;
            oss << "CORE // " << id << " | ROT: " << std::fixed << std::setprecision(1) << outerRotationY << " deg";
            return oss.str();
        }

        float getCollisionRadius() const override { return 0.98f; }
    };

    // ========================================================================
    // 3. YÖRÜNGE DÜĞÜMÜ VE HALKASI SINIFI (ORBITING NODE & RING - INHERITANCE)
    // ========================================================================
    class OrbitingNode3D : public OrbitalEntity3D {
    private:
        float radius;
        float tiltX;
        float tiltZ;
        float angularSpeed;
        float currentAngle;
        float nodeSize;
        ProjectSpec linkedProject;

        Vector3 computeOrbitalPoint(float angleRad) const {
            float x = std::cos(angleRad) * radius;
            float z = std::sin(angleRad) * radius;
            Vector3 p = { x, 0.0f, z };

            Matrix rotX = MatrixRotateX(tiltX);
            Matrix rotZ = MatrixRotateZ(tiltZ);
            Matrix transform = MatrixMultiply(rotX, rotZ);
            return Vector3Transform(p, transform);
        }

    public:
        explicit OrbitingNode3D(const OrbitNodeConfig& cfg)
            : OrbitalEntity3D(cfg.id, cfg.label),
              radius(cfg.radius), tiltX(cfg.tiltX), tiltZ(cfg.tiltZ),
              angularSpeed(cfg.angularSpeed), currentAngle(cfg.initialPhase),
              nodeSize(cfg.nodeSize), linkedProject(cfg.project) {
            position = computeOrbitalPoint(currentAngle);
        }

        void update(float dt, float speedMultiplier, bool reducedMotion) override {
            if (!reducedMotion) {
                currentAngle += dt * angularSpeed * speedMultiplier;
            }
            position = computeOrbitalPoint(currentAngle);
        }

        void draw3D() const override {
            // 1. Eğimli 3B Yörünge Halkasını Çiz (Procedural Segmented Ring)
            const int segments = 80;
            Color ringColor = highlighted
                ? Fade(Tokens::ACCENT_PRIMARY, 0.88f)
                : (hovered ? Fade(Tokens::ACCENT_PRIMARY, 0.50f) : Fade(Tokens::BORDER_COLOR, 0.85f));

            for (int i = 0; i < segments; ++i) {
                float a1 = (static_cast<float>(i) / segments) * 2.0f * PI;
                float a2 = (static_cast<float>(i + 1) / segments) * 2.0f * PI;
                Vector3 p1 = computeOrbitalPoint(a1);
                Vector3 p2 = computeOrbitalPoint(a2);
                DrawLine3D(p1, p2, ringColor);
            }

            // 2. Yörünge Düğümünü (Satellite Node) Çiz
            Color nodeColor = (highlighted || hovered)
                ? Tokens::ACCENT_PRIMARY
                : Fade(Tokens::ACCENT_PRIMARY, 0.78f);
            DrawSphere(position, hovered ? nodeSize * 1.15f : nodeSize, nodeColor);

            // Seçili veya üzerine gelinen (hover) düğümün etrafına ekstra telemetri kafesi çiz
            if (highlighted || hovered) {
                DrawSphereWires(position, nodeSize * 1.90f, 6, 6,
                                hovered ? Tokens::ACCENT_SECONDARY : Tokens::ACCENT_PRIMARY);
            }
            if (highlighted) {
                DrawCubeWiresV(position, {nodeSize * 2.85f, nodeSize * 2.85f, nodeSize * 2.85f},
                               Fade(Tokens::ACCENT_PRIMARY, 0.60f));
            }
        }

        std::string getTelemetrySummary() const override {
            std::ostringstream oss;
            oss << id << " [" << label << "] R:" << std::fixed << std::setprecision(2) << radius
                << " | POS(" << std::setprecision(1) << position.x << ", "
                << position.y << ", " << position.z << ")";
            return oss.str();
        }

        float getCollisionRadius() const override {
            // 3B Raycasting ile kolay seçilebilmesi için cömert tıklama yarıçapı
            return nodeSize * 2.4f;
        }

        float getRadius() const { return radius; }
        const ProjectSpec& getProjectSpec() const { return linkedProject; }
    };

    // ========================================================================
    // 4. SİNYAL AKIŞI SINIFI (SIGNAL STREAM - BEZIER CURVE TELEMETRY)
    // ========================================================================
    class SignalStream3D {
    private:
        const OrbitalEntity3D& source;
        const OrbitalEntity3D& target;
        float curveArcHeight;
        float pulseProgress{0.0f};

    public:
        SignalStream3D(const OrbitalEntity3D& src, const OrbitalEntity3D& tgt, float arcHeight)
            : source(src), target(tgt), curveArcHeight(arcHeight) {}

        void update(float dt, bool reducedMotion) {
            if (!reducedMotion) {
                pulseProgress += dt * 0.65f;
                if (pulseProgress > 1.0f) pulseProgress -= 1.0f;
            }
        }

        void draw3D() const {
            Vector3 start = source.getPosition();
            Vector3 end   = target.getPosition();
            Vector3 mid   = Vector3Scale(Vector3Add(start, end), 0.5f);
            mid.y += curveArcHeight;

            bool activeHighlight = target.isHighlighted() || target.isHovered();
            Color streamColor = activeHighlight
                ? Fade(Tokens::ACCENT_PRIMARY, 0.78f)
                : Fade(Tokens::ACCENT_PRIMARY, 0.25f);

            const int steps = 22;
            Vector3 prev = start;
            for (int i = 1; i <= steps; ++i) {
                float t = static_cast<float>(i) / steps;
                float u = 1.0f - t;
                Vector3 pt = {
                    u * u * start.x + 2.0f * u * t * mid.x + t * t * end.x,
                    u * u * start.y + 2.0f * u * t * mid.y + t * t * end.y,
                    u * u * start.z + 2.0f * u * t * mid.z + t * t * end.z
                };
                DrawLine3D(prev, pt, streamColor);
                prev = pt;
            }

            float t = pulseProgress;
            float u = 1.0f - t;
            Vector3 packetPos = {
                u * u * start.x + 2.0f * u * t * mid.x + t * t * end.x,
                u * u * start.y + 2.0f * u * t * mid.y + t * t * end.y,
                u * u * start.z + 2.0f * u * t * mid.z + t * t * end.z
            };
            DrawSphere(packetPos, activeHighlight ? 0.068f : 0.042f, Tokens::ACCENT_PRIMARY);
        }
    };

    // ========================================================================
    // 5. ARKA PLAN YILDIZ / PARÇACIK ALANI (BACKGROUND PARTICLE FIELD)
    // ========================================================================
    class ParticleField3D {
    private:
        std::vector<Vector3> particles;
        float fieldRotationY{0.0f};

    public:
        explicit ParticleField3D(int count) {
            particles.reserve(count);
            for (int i = 0; i < count; ++i) {
                float r = 5.0f + static_cast<float>(GetRandomValue(0, 550)) / 100.0f;
                float theta = static_cast<float>(GetRandomValue(0, 628)) / 100.0f;
                float phi   = static_cast<float>(GetRandomValue(0, 314)) / 100.0f;

                Vector3 pt = {
                    r * std::sin(phi) * std::cos(theta),
                    r * std::sin(phi) * std::sin(theta),
                    r * std::cos(phi)
                };
                particles.push_back(pt);
            }
        }

        void update(float dt, bool reducedMotion) {
            if (!reducedMotion) {
                fieldRotationY += dt * 0.03f;
            }
        }

        void draw3D() const {
            Matrix rot = MatrixRotateY(fieldRotationY);
            for (const auto& p : particles) {
                Vector3 rp = Vector3Transform(p, rot);
                DrawPoint3D(rp, Fade(Tokens::ACCENT_PRIMARY, 0.45f));
            }
        }
    };

} // namespace FmCoskunStudio

#endif // FMCOSKUN_ORBITAL_ENTITIES_HPP

