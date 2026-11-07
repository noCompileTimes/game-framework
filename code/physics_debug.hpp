#pragma once

#include "geometry.hpp"

#include <LinearMath/btIDebugDraw.h>

#include <print>

class PhysicsDebug : public btIDebugDraw
{
public:
    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
    {
        geometry.vertices.emplace_back(editor_vertex { { from.x(), from.y(), from.z() }, { color.x(), color.y(), color.z() } });
        geometry.vertices.emplace_back(editor_vertex { { to.x(), to.y(), to.z() }, { color.x(), color.y(), color.z() } });
    }

    void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override
    {
        std::println("draw_contact_point");
    }

    void reportErrorWarning(const char* warningString) override
    {
        std::println("draw_error_warning");
    }

    void draw3dText(const btVector3& location, const char* textString) override
    {
        std::println("draw_text3d");
    }

    void setDebugMode(int debugMode) override
    {
    }

    int getDebugMode() const override
    {
        return DBG_DrawAabb /* | DBG_DrawWireframe */; // TODO check if DBG_DrawWireframe is needed also
    }

    virtual void clearLines()
    {
        geometry.clear();
    }

    virtual void flushLines()
    {
        std::println("flush_lines");
    }

    geometry<editor_vertex, uint32_t> geometry;
};