#version 120

varying vec2 worldPosition;

uniform sampler2D texture;

uniform vec2 pointLightPosition[8];
uniform vec3 pointLightColour[8];
uniform float pointLightRadius[8];
uniform int pointLightNum;

uniform vec2 areaLightCentre[8];
uniform vec2 areaLightExtent[8];
uniform vec2 areaLightBorder[8];
uniform vec2 areaLightVector[8];
uniform vec3 areaLightColour[8];
uniform int areaLightNum;

uniform vec2 normalTransform0;
uniform vec2 normalTransform1;

void main()
{
    // Get pixel from the texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    // Remap normal from the texture
    vec2 normal = vec2(2.0*pixel.r - 1.0, 1.0 - 2.0*pixel.g);

    mat2 normalTransform = mat2(normalTransform0, normalTransform1);
    normal = normalTransform * normal;

    // Now we will accumulate colour from each light.
    vec3 accumulatedColour = vec3(0, 0, 0);

    // Point lights
    for (int i = 0; i < pointLightNum; ++i)
    {
        vec2 pixelToLight = pointLightPosition[i] - worldPosition;

        float lightDistance = length(pixelToLight);
        float attenuation = max(0.0, 1.0 - (lightDistance / pointLightRadius[i]));

        vec2 lightNormal = normalize(pixelToLight);
        float strangeDot = max(0.0, 0.2 + 0.8*dot(lightNormal, normal));
        float brightness = attenuation * strangeDot * pixel.b;

        accumulatedColour += brightness*pointLightColour[i];
    }

    // Area lights
    for (int i = 0; i < areaLightNum; ++i)
    {
        vec2 distToLight = abs(areaLightCentre[i] - worldPosition);
        float attenuationX = clamp((areaLightExtent[i].x - distToLight.x)/areaLightBorder[i].x, 0.0, 1.0);
        float attenuationY = clamp((areaLightExtent[i].y - distToLight.y)/areaLightBorder[i].y, 0.0, 1.0);
        float attenuation = min(attenuationX, attenuationY);

        vec2 lightNormal = -normalize(areaLightVector[i]);
        float strangeDot = max(0.0, 0.2 + 0.8*dot(lightNormal, normal));
        float brightness = attenuation * strangeDot * pixel.b;
        accumulatedColour += brightness*areaLightColour[i];
    }

    gl_FragColor = vec4(accumulatedColour.r, accumulatedColour.g, accumulatedColour.b, pixel.a);
}
