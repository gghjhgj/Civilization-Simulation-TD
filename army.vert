#version 120

uniform float spacingX;
uniform float spacingY;
uniform float time;
uniform float noise;
uniform float formationWidth;
uniform vec2 armyMain;

void main()
{
    float r = floor(gl_Color.r * 255.0 + 0.5);
    float g = floor(gl_Color.g * 255.0 + 0.5);
    float b = floor(gl_Color.b * 255.0 + 0.5);
    float id = r + (g * 256.0) + (b * 65536.0) + 0.5;

    float col = mod(id, formationWidth);
    float row = floor(id / formationWidth + 0.0001);

    float phaseX = fract(sin(id) * 43758.5453);
    float phaseY = fract(sin(id + 123.456) * 12345.6789);

    float sX = sin(time * 4.0 + phaseX * 6.28);
    float sY = sin(time * 3.7 + phaseY * 6.28);

    float quantX = step(0.5, sX) - step(0.5, -sX);
    float quantY = step(0.5, sY) - step(0.5, -sY);

    vec4 pos = gl_Vertex;

    pos.x += armyMain.x + (col * spacingX) + (quantX * noise);
    pos.y += armyMain.y + (row * spacingY) + (quantY * noise);
    
    gl_Position = gl_ModelViewProjectionMatrix * pos;
    
    float debug = fract(id * 0.001);
    gl_FrontColor = vec4(debug, 0.0, 0.0, 1.0);
}