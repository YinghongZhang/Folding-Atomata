
/******************************************************************************\
                     This file is part of Folding Atomata,
          a program that displays 3D views of Folding@home proteins.

                      Copyright (c) 2013, Jesse Victors

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see http://www.gnu.org/licenses/

                For information regarding this software email:
                                Jesse Victors
                         jvictors@jessevictors.com
\******************************************************************************/

#include "Light.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <sstream>
#include <stdexcept>


std::size_t Light::nLights_ = 0;


Light::Light(const glm::vec3& position, const glm::vec3& color, float power):
    position_(position), color_(glm::normalize(color)),
    power_(power), emitting_(true)
{
    nLights_++;
}



glm::vec3 Light::getPosition() const
{
    return position_;
}



void Light::setPosition(const glm::vec3& newPos)
{
    position_ = newPos;
}



glm::vec3 Light::getColor() const
{
    return color_;
}



void Light::setColor(const glm::vec3& newColor)
{
    color_ = newColor;
}



float Light::getPower() const
{
    return power_;
}



void Light::setPower(float power)
{
    power_ = power;
}



bool Light::isEmitting() const
{
    return emitting_;
}



// Turn the light on or off
void Light::setEmitting(bool emitting)
{
    emitting_ = emitting;
}



void Light::sync(GLuint handle, std::size_t lightID)
{
    //should look into http://stackoverflow.com/questions/8099979/glsl-c-arrays-of-uniforms
    auto lightRef = "lights[" + std::to_string(lightID) + "]";

    GLint posLoc = glGetUniformLocation(handle, (lightRef + ".position").c_str());
    glUniform3fv(posLoc, 1, glm::value_ptr(getPosition()));

    GLint colorLoc = glGetUniformLocation(handle, (lightRef + ".color").c_str());
    glUniform3fv(colorLoc, 1, glm::value_ptr(getColor()));

    GLint powerLoc = glGetUniformLocation(handle, (lightRef + ".power").c_str());
    float power = isEmitting() ? getPower() : 0;
    glUniform1f(powerLoc, power);

    //std::cout << handle << " " << lightID << " " << nLights_ << " " << posLoc << " " << colorLoc << " " << powerLoc << std::endl;

    if (posLoc < 0 || colorLoc < 0 || powerLoc < 0)
        throw std::runtime_error("Unable to find Light uniform variables!");
}



SnippetPtr Light::getVertexShaderGLSL()
{
    return std::make_shared<ShaderSnippet>(
        R".(
            //Light fields
            varying vec3 fragmentPosition;
        ).",
        R".(
            //Light methods
        ).",
        R".(
            //Light main method code
            fragmentPosition = (modelMatrix * vec4(vertex, 1)).xyz;
        )."
    );
}



SnippetPtr Light::getFragmentShaderGLSL()
{
    //http://www.opengl.org/discussion_boards/showthread.php/164100-GLSL-multiple-lights
    //http://en.wikibooks.org/wiki/GLSL_Programming/GLUT/Multiple_Lights
    //http://www.geeks3d.com/20091013/shader-library-phong-shader-with-multiple-lights-glsl/
    //http://Viewerdev.stackexchange.com/questions/53822/variable-number-of-lights-in-a-glsl-shader
    //http://stackoverflow.com/questions/8202173/setting-the-values-of-a-struct-array-from-js-to-glsl

    std::stringstream fieldStrStream("");
    fieldStrStream << R".(
            //Light fields
            // http://stackoverflow.com/questions/8202173/setting-the-values-of-a-struct-array-from-js-to-glsl

            struct Light
            {
                vec3 position, color;
                float power; //its maximum distance of influence
            };

            uniform Light lights[)." << nLights_ << R".(];
            varying vec3 fragmentPosition;
        ).";

    return std::make_shared<ShaderSnippet>(
        fieldStrStream.str(),
        R".(
            //Light methods
        ).",
        R".(
            //Light main method code
            colors.lightBlend = vec3(0); //see Scene::getFragmentShaderGLSL()

            for (int j = 0; j < lights.length(); j++)
            {
                float distance = length(fragmentPosition - lights[j].position);
                float scaledDistance = distance / lights[j].power;
                vec3 luminosity = lights[j].color * (1 - scaledDistance);
                colors.lightBlend += clamp(luminosity, 0, 1);
            }

            //colors.lightBlend = normalize(colors.lightBlend);
        )."
    );
}
