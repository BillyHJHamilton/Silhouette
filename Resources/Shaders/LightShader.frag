#version 120

varying vec2 worldPosition;

uniform sampler2D texture;

uniform vec2 pointLightPosition[8];
uniform vec3 pointLightColour[8];
uniform float pointLightRadius[8];
uniform int pointLightNum;

uniform vec2 normalTransform0;
uniform vec2 normalTransform1;

void main()
{
    //vec2 pixelToLight = pointLightPosition[0] - worldPosition;
    //gl_FragColor = vec4(pixelToLight.x/100.0, pixelToLight.y/100.0, 1, 1);

    // Get pixel from the texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    // Remap normal from the texture
    vec2 normal = vec2(2.0*pixel.r - 1.0, 1.0 - 2.0*pixel.g);

    mat2 normalTransform = mat2(normalTransform0, normalTransform1);
    normal = normalTransform * normal;

    //mat2 testMat = mat2(-1.0, 0.0,
    //                     0.0, 1.0 );
    //normal = testMat * normal;

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

    //gl_FragColor = vec4(gl_ModelViewMatrix[0][0], gl_ModelViewMatrix[1][1], gl_ModelViewMatrix[0][1], 1.0);
}
