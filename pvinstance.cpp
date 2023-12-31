#include "pvinstance.h"
#include "lighting.h"
#include "pvenums.h"
#include "rbxmath.h"
#include "stdout.h"
#include "mesh.h"
#include "ray.h"

#define FRONT_CROSS Color3(0.35f, 0.35f, 0.35f)
#define BACK_CROSS  Color3(1,1,1)

RTTR_REGISTRATION
{
    rttr::registration::class_ <RBX::PVInstance>("PVInstance")
         .constructor<>()
         .property("Anchored", &RBX::PVInstance::getAnchored, &RBX::PVInstance::setAnchored)(rttr::metadata("Type", RBX::Behavior))
         .property("CanCollide", &RBX::PVInstance::getCanCollide, &RBX::PVInstance::setCanCollide)(rttr::metadata("Type", RBX::Behavior))
         .property("Locked", &RBX::PVInstance::getLocked, 
             &RBX::PVInstance::setLocked)(rttr::metadata("Type", RBX::Behavior))
         .property("Position", &RBX::PVInstance::getPosition, &RBX::PVInstance::setPosition)(rttr::metadata("Type", RBX::Data))
         .property("CFrame", &RBX::PVInstance::getCFrame, &RBX::PVInstance::setCFrame)
         .property("size", &RBX::PVInstance::getSizeExternal, &RBX::PVInstance::setSize)(rttr::metadata("Type", RBX::Part))
         .property("shape", &RBX::PVInstance::getShape, &RBX::PVInstance::setShape)(rttr::metadata("Type", RBX::Part))
         .property("FrontSurface", &RBX::PVInstance::getFrontSurface, &RBX::PVInstance::setFrontSurface)
         .property("BackSurface", &RBX::PVInstance::getBackSurface, &RBX::PVInstance::setBackSurface)
         .property("TopSurface", &RBX::PVInstance::getTopSurface, &RBX::PVInstance::setTopSurface)
         .property("BottomSurface", &RBX::PVInstance::getBottomSurface, &RBX::PVInstance::setBottomSurface)
         .property("RightSurface", &RBX::PVInstance::getRightSurface, &RBX::PVInstance::setRightSurface)
         .property("LeftSurface", &RBX::PVInstance::getLeftSurface, &RBX::PVInstance::setLeftSurface)
         .property("rawFormFactor", &RBX::PVInstance::getFormFactor, &RBX::PVInstance::setFormFactor)
         .property("formFactor", &RBX::PVInstance::getFormFactor, &RBX::PVInstance::setFormFactor)
         .property("Transparency", &RBX::PVInstance::getTransparency, &RBX::PVInstance::setTransparency)(rttr::metadata("Type", RBX::Appearance));
}

void drawFace(Vector2 uv, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3)
{
    glNormal((v0 - v1).cross(v0 - v2).direction());

    glTexCoord2d(uv.x, uv.y);
    glVertex(v0);
    glTexCoord2d(0, uv.y);
    glVertex(v1);
    glTexCoord2d(0, 0);
    glVertex(v2);
    glTexCoord2d(uv.x, 0);
    glVertex(v3);
}

void RBX::PVInstance::setFace(NormalId f, SurfaceType s)
{
    switch (f)
    {
    case NormalId::TOP:
    {
        top = s;
        initFace(idTop, s);
        break;
    }
    case NormalId::BOTTOM:
    {
        bottom = s;
        initFace(idBottom, s);
        break;
    }
    case NormalId::LEFT:
    {
        left = s;
        initFace(idLeft, s);
        break;
    }
    case NormalId::RIGHT:
    {
        right = s;
        initFace(idRight, s);
        break;
    }
    case NormalId::FRONT:
    {
        front = s;
        initFace(idFront, s);
        break;
    }
    case NormalId::BACK:
    {
        back = s;
        initFace(idBack, s);
        break;
    }
    }
}

void RBX::PVInstance::initFace(unsigned int& f, SurfaceType s)
{
    if (f == -1)
    {
        if(s != Smooth)
            f = RBX::RenderSurfaceFactory::getSurfaceTexture(s);
    }
}

void RBX::PVInstance::renderSurfaces(RenderDevice* rd)
{
    if (!specialShape)
    {
        glColor(1, 1, 1, 1 - (color.r * 0.5f));

        renderSurface(rd, this, TOP, top, idTop);
        renderSurface(rd, this, BOTTOM, bottom, idBottom);
        renderSurface(rd, this, RIGHT, right, idRight);
        renderSurface(rd, this, LEFT, left, idLeft);
        renderSurface(rd, this, FRONT, front, idFront);
        renderSurface(rd, this, BACK, back, idBack);

    }
}

void RBX::PVInstance::render3DSurfaces(RenderDevice* rd)
{

    render3dSurface(rd, TOP);
    render3dSurface(rd, BOTTOM);
    render3dSurface(rd, FRONT);
    render3dSurface(rd, BACK);
    render3dSurface(rd, LEFT);
    render3dSurface(rd, RIGHT);

}

void RBX::renderSurface(RenderDevice* rd, RBX::PVInstance* pv, NormalId n, SurfaceType s, unsigned int& glid)
{
    if (glid == -1) return;

        if (pv->getTransparency() > 0) /* already blended, no need to reapply blend */
        {
            RBX::Render::rawDecal(rd, pv, n, glid);
        }
        else
        {
            RBX::Render::rawDecal(rd, pv, n, glid, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
}

void RBX::PVInstance::render(RenderDevice* d)
{

    d->setObjectToWorldMatrix(getCFrame());
    glColor(color.r, color.g, color.b, alpha);

    if (transparency < 1)
    {
        switch (shape)
        {
        case part:
        {

            renderFace(d, TOP);
            renderFace(d, BOTTOM);
            renderFace(d, FRONT);
            renderFace(d, BACK);
            renderFace(d, LEFT);
            renderFace(d, RIGHT);

            renderSurfaces(d);
            
            break;
        }
        case ball:
        {
            RBX::Primitives::drawBall(d, this);
            break;
        }
        case cylinder:
        {

            RBX::Primitives::drawCylinder(d, this);
            drawCylinderPluses(d);

            break;
        }
        }
    }

}

void RBX::PVInstance::renderFace(RenderDevice* rd, NormalId face)
{

    glBegin(GL_QUADS);

    switch (face)
    {
    case NormalId::TOP:
    {
        drawFace(uv0,
            Vector3(size.x, size.y, -size.z),
            Vector3(-size.x, size.y, -size.z),
            Vector3(-size.x, size.y, size.z),
            Vector3(size.x, size.y, size.z));
        break;
    }
    case NormalId::BOTTOM:
    {
        drawFace(uv0,
            Vector3(size.x, -size.y, size.z),
            Vector3(-size.x, -size.y, size.z),
            Vector3(-size.x, -size.y, -size.z),
            Vector3(size.x, -size.y, -size.z));
        break;
    }
    case NormalId::FRONT:
    {
        drawFace(uv1,
            Vector3(size.x, size.y, size.z),
            Vector3(-size.x, size.y, size.z),
            Vector3(-size.x, -size.y, size.z),
            Vector3(size.x, -size.y, size.z));
        break;
    }
    case NormalId::BACK:
    {
        drawFace(uv1,
            Vector3(size.x, -size.y, -size.z),
            Vector3(-size.x, -size.y, -size.z),
            Vector3(-size.x, size.y, -size.z),
            Vector3(size.x, size.y, -size.z));
        break;
    }
    case NormalId::LEFT:
    {
        drawFace(uv2,
            Vector3(-size.x, size.y, size.z),
            Vector3(-size.x, size.y, -size.z),
            Vector3(-size.x, -size.y, -size.z),
            Vector3(-size.x, -size.y, size.z));
        break;
    }
    case NormalId::RIGHT:
    {
        drawFace(uv2,
            Vector3(size.x, size.y, -size.z),
            Vector3(size.x, size.y, size.z),
            Vector3(size.x, -size.y, size.z),
            Vector3(size.x, -size.y, -size.z));
        break;
    }
    }

    glEnd();

}

void RBX::PVInstance::drawCylinderPluses(RenderDevice* d)
{
    float radius, axis;

    radius = getSize().x;
    axis = getSize().y;

    glRotatef(-90.0, 0.0, 1.0, 0.0);

    glTranslatef(0.0, 0.0, -axis * 0.5);
    glTranslatef(0.0, 0.0, axis);

    RBX::Primitives::drawLine(cylinderOriginX, d, FRONT_CROSS, lineWidth, -lineHeight);
    RBX::Primitives::drawLine(cylinderOriginX, d, FRONT_CROSS, -lineWidth, -lineHeight);
    RBX::Primitives::drawLine(cylinderOriginY, d, FRONT_CROSS, -lineHeight, lineWidth);
    RBX::Primitives::drawLine(cylinderOriginY, d, FRONT_CROSS, -lineHeight, -lineWidth);

    glRotatef(180.0, 0.0, 1.0, 0.0);
    glTranslatef(0.0, 0.0, axis);

    RBX::Primitives::drawLine(cylinderOriginX, d, BACK_CROSS, lineWidth, -lineHeight);
    RBX::Primitives::drawLine(cylinderOriginX, d, BACK_CROSS, -lineWidth, -lineHeight);
    RBX::Primitives::drawLine(cylinderOriginY, d, BACK_CROSS, -lineHeight, lineWidth);
    RBX::Primitives::drawLine(cylinderOriginY, d, BACK_CROSS, -lineHeight, -lineWidth);

}

float RBX::getAffectedFormFactor(RBX::PVInstance* pv)
{
    switch (pv->getFormFactor())
    {
    case RBX::Brick:
    {
        return 1.2f;
    }
    case RBX::Plate:
    {
        return 0.4f;
    }
    case RBX::Symmetric:
    {
        return 1.0f;
    }
    }
}

void RBX::PVInstance::calculateCylinderOffsets()
{
    float radius, scale;

    radius = getSize().x;

    scale = radius * 0.2;

    cylinderOriginX = Vector2(0, scale / 2);
    cylinderOriginY = Vector2(scale / 2, 0);

    lineWidth = radius / 1.5f;
    lineHeight = scale;
}

void RBX::PVInstance::calculateUvs()
{
    uv0 = Vector2(size.x, -size.z/2);
    uv1 = Vector2(-size.x, size.y/2);
    uv2 = Vector2(size.z, size.y/2);
}

void RBX::PVInstance::render3dSurface(RenderDevice* d, NormalId face)
{
    SurfaceType type;
    type = getSurface(face);

    if (type != SurfaceType::Smooth)
    {

        CoordinateFrame center;
        center = getSurfaceCenter(face, getSize(), getLocalExtents());

        d->setObjectToWorldMatrix(getCFrame());

        switch (type)
        {
        case SurfaceType::Hinge:
        {
            Draw::cylinder(Cylinder(center.translation - center.lookVector() * 0.5f, center.translation + center.lookVector() * 0.5f, 0.2f), d, Color3::yellow(), Color4::clear());
            break;
        }
        case SurfaceType::SteppingMotor:
        case SurfaceType::Motor:
        {

            Draw::cylinder(Cylinder(center.translation - center.lookVector() * 0.5f, center.translation + center.lookVector() * 0.5f, 0.2f), d, Color3::yellow(), Color4::clear());
            Draw::cylinder(Cylinder(center.translation, center.translation + center.lookVector() * 0.1f, 0.5f), d, Color3::gray(), Color4::clear());

            break;
        }
        default:
        {
            break;
        }
        }

    }
}

RBX::SurfaceType RBX::PVInstance::getSurface(NormalId face)
{
    switch (face)
    {
    case NormalId::TOP:
    {
        return top;
    }
    case NormalId::BOTTOM:
    {
        return bottom;
    }
    case NormalId::FRONT:
    {
        return front;
    }
    case NormalId::BACK:
    {
        return back;
    }
    case NormalId::LEFT:
    {
        return left;
    }
    case NormalId::RIGHT:
    {
        return right;
    }
    }
    return Smooth;
}
