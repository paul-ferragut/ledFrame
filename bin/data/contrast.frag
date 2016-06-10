#version 120

uniform sampler2DRect tex0;     // base image to render


   uniform int inBlack;
   uniform float inGamma;
   uniform int inWhite;
   uniform float alpha;



void main (){

    // Original pixel color
    vec2 pos = gl_TexCoord[0].st;
    vec4 src = texture2DRect(tex0, pos);

    // Convert color to grayscale
    //float l = 0.2989 * src.r + 0.5870 * src.g + 0.1140 * src.b;

    // Pick color in gradient, according to grayscale value
    //vec4 map = texture2DRect(gradient, vec2(l * gradientWidth, 5.0) );

    // Return this color
    // Keep alpha value from original pixel
   // gl_FragColor = vec4( map.rgb, src.a );


       //float4 inPixel = tex2D(texture, uv);
    vec4 color =  vec4(vec3(
    pow(((src.r * 255.0) - float(inBlack)) / ( float(inWhite) -  float(inBlack)), inGamma)  / 255.0,
    pow(((src.g * 255.0) - float(inBlack)) / ( float(inWhite) -  float(inBlack)), inGamma)  / 255.0,
    pow(((src.b * 255.0) - float(inBlack)) / ( float(inWhite) -  float(inBlack)), inGamma)  / 255.0),alpha);



   gl_FragColor=color;
}
