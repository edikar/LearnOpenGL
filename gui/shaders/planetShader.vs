#version 330 core

/* Uniforms */
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMat;

uniform float perlinFreq;
uniform float perlinOffset;
uniform int perlinDepth;

/* In attributes */
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

/* Out attributes */
out vec3 FragPos;  
out float FragHeight;
out vec3 Normal;

const float HEIGHT_FACTOR = 0.6;

const int  SEED = 1985;
int HASH[256] = int[256](
	    208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
	    185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
	    9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
	    70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
	    203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
	    164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
	    228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
	    232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
	    193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
	    101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
	    135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
	    114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219
	    );

int noise2(int x, int y)
{
    int  yindex = (y + SEED) % 256;
    if (yindex < 0)
        yindex += 256;
    int  xindex = (HASH[yindex] + x) % 256;
    if (xindex < 0)
        xindex += 256;
    int  result = HASH[xindex];
    return result;
}

float lin_inter(float x, float y, float s)
{
    return x + s * (y-x);
}

float smooth_inter(float x, float y, float s)
{
    return lin_inter( x, y, s * s * (3 - 2 * s) );
}

float noise2d(float x, float y)
{
    int  x_int = int(floor( x ));
    int  y_int = int(floor( y ));
    float  x_frac = x - x_int;
    float  y_frac = y - y_int;
    int  s = noise2( x_int, y_int );
    int  t = noise2( x_int+1, y_int );
    int  u = noise2( x_int, y_int+1 );
    int  v = noise2( x_int+1, y_int+1 );
    float  low = smooth_inter( s, t, x_frac );
    float  high = smooth_inter( u, v, x_frac );
    float  result = smooth_inter( low, high, y_frac );
    return result;
}

float perlin2d(float x, float y, float freq, int depth)
{
    float  xa = x*freq;
    float  ya = y*freq;
    float  amp = 1.0;
    float  fin = 0;
    float  div = 0.0;
    for (int i=0; i<depth; i++)
    {
        div += 256 * amp;
        fin += noise2d( xa, ya ) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }
    return fin/div;
}

void main()
{
    // note that we read the multiplication from right to left
    float phi = atan(aPos.z/aPos.x);
    float theta = acos(aPos.y);

    float perlin = perlin2d(phi + perlinOffset, theta + perlinOffset, perlinFreq, perlinDepth);
    float perlinFactor = (perlinDepth == 0) ? 1.0f : (1.0f + 0.2 * (perlin-0.5));
    vec4 tempV = vec4(aPos * perlinFactor, 1.0);
    

    gl_Position = projection * view * model * tempV;
    //Save the fragment position in world space
    FragPos = vec3(model * tempV);
    FragHeight = 2 * (perlin - 0.5f);

    //transform the normals with normal matrix (transpose inverse of model matrix)
    //inverse is a costly operation! it should be done on the CPU and passed as a uniform!!!
    //Normal = mat3(transpose(inverse(model))) * aNormal;
    Normal = mat3(normalMat) * aNormal;
}














