#version 120

varying vec2 worldPosition;

uniform sampler2D texture;

uniform vec2 pointLightPosition[16];
uniform vec3 pointLightColour[16];
uniform float pointLightRadius[16];
uniform int pointLightNum;

void main()
{
    //vec2 pixelToLight = pointLightPosition[0] - worldPosition;
    //gl_FragColor = vec4(pixelToLight.x/100.0, pixelToLight.y/100.0, 1, 1);

    // Get pixel from the texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    // Remap normal from the texture
    vec2 normal = vec2(2.0*pixel.r - 1.0, 1.0 - 2.0*pixel.g);

    // Accumulate brightness from each light.
    vec3 accumulatedColour = vec3(0, 0, 0);
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

    gl_FragColor = vec4(accumulatedColour.r, accumulatedColour.g, accumulatedColour.b, pixel.a);
}
