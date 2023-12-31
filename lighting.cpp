
#include "rbx.h"
#include "lighting.h"
#include "datamodel.h"

RTTR_REGISTRATION
{
    rttr::registration::class_ <RBX::Lighting>("Lighting")
         .constructor<>()
         .property("TimeOfDay", &RBX::Lighting::getTimeOfDay, &RBX::Lighting::setTimeOfDay)(rttr::metadata("Type", RBX::Data))
         .property("BottomAmbientV9", &RBX::Lighting::getBottomAmbient, &RBX::Lighting::setBottomAmbient)(rttr::metadata("Type", RBX::Appearance))
         .property("ClearColor", &RBX::Lighting::getClearColor, &RBX::Lighting::setClearColor)(rttr::metadata("Type", RBX::Appearance))
         .property("SpotLightV9", &RBX::Lighting::getSpotLight, &RBX::Lighting::setSpotLight)(rttr::metadata("Type", RBX::Appearance))
         .property("TopAmbientV9", &RBX::Lighting::getTopAmbient, &RBX::Lighting::setTopAmbient)(rttr::metadata("Type", RBX::Appearance))
         .property("GeographicLatitude",&RBX::Lighting::getGeoLatitude, &RBX::Lighting::setGeoLatitude)(rttr::metadata("Type", RBX::Appearance));
}

RBX::Lighting* RBX::Lighting::singleton()
{
    return RBX::Datamodel::getDatamodel()->lighting;
}

void RBX::Lighting::begin(RenderDevice* device)
{

    LightingParameters lighting;
    Color3 ambientColor, dynamicTop, dynamicBottom;
    
    lighting = getParameters();

    device->enableLighting();
    device->setSpecularCoefficient(Color3::white());

    device->setShininess(50.0f);

    Vector3 dir = lighting.lightDirection;
    ambientColor = (bottom_ambient + top_ambient) / 2.0f;

    device->setColorClearValue(clear_color);
    device->setAmbientLightColor(ambientColor);

     device->setLight(0, GLight::directional(dir, (spotLight_color * 0.8999999761581421f), 1, 1));

     if (ambientColor != top_ambient)
     {
         dynamicTop = top_ambient - ambientColor;
         device->setLight(1, GLight::directional(-Vector3::unitX(), dynamicTop, 0, 1));
         device->setLight(2, GLight::directional(Vector3::unitY(), dynamicTop, 0, 1));
         device->setLight(3, GLight::directional(-Vector3::unitZ(), dynamicTop, 0, 1));
     }
     if (ambientColor != bottom_ambient)
     {
         dynamicBottom = bottom_ambient - ambientColor;
         device->setLight(4, GLight::directional(toLight, dynamicBottom, 0, 1));
     }
}

void RBX::Lighting::end(RenderDevice* device)
{
    device->disableLighting();
}

LightingParameters RBX::Lighting::getParameters()
{
    if (!params)
    {
        params = new LightingParameters();

        params->setLatitude(latitude);
        params->setTime(timeOfDay);

    }

    return *params;
}

GameTime RBX::Time::fromString(const char* c)
{
    int hour = 0, minute = 0, second = 0;

    sscanf(c, "%d:%d:%d", &hour, &minute, &second);

    return toSeconds(hour, minute, second, PM);
}

std::string RBX::Time::toString(int seconds)
{
    int hour, minute, second;

    hour = seconds / 3600;
    minute = seconds % (hour * 60);
    second = (seconds % (hour * 3600)) - ((seconds % (hour * 60)) * 60);

    return RBX::Format("%d:%d:%d", hour, minute, second);
}
