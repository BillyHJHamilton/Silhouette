uniform sampler2D texture;
uniform vec2 light;

void main()
{
    //const vec2 light = vec2(0.5,0.5);

    // Get pixel from the texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    // Remap normal from the texture
    vec2 normal = vec2(2.0*pixel.r - 1.0, 2.0*pixel.g - 1.0);

    // Multiply normal by light direction
    float brightness = (0.2 + 0.8*dot(light, normal)) * pixel.b;

    gl_FragColor = vec4(brightness, brightness, 0.0, pixel.a);
}
