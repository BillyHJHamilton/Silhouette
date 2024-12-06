#version 120

//varying vec2 worldPosition;

uniform sampler2D texture;
uniform vec3 flashColour;
uniform float modTime; // Looping 1-minute timer

const float c_Pi = 3.14159265358979323;
const float c_2Pi = 2.0 * c_Pi;
const float c_Period = 0.25f;
const float c_PeriodFactor = c_2Pi/c_Period;

void main()
{
    // Get pixel from the texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    float brightness = 0.5f + 0.5*sin(c_PeriodFactor * modTime);

    gl_FragColor = vec4(brightness * flashColour.rgb, pixel.a);
}
