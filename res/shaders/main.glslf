layout (location = 0) out vec4 f_color;
layout (location = 1) out vec4 f_position;
layout (location = 2) out vec4 f_normal;

in vec4 a_color;
in vec2 a_texCoord;
in float a_fog;
in vec3 a_position;
in vec3 a_dir;
in vec3 a_normal;
in vec3 a_realnormal;
in vec4 a_modelpos;

uniform sampler2D u_texture0;
uniform samplerCube u_cubemap;
uniform sampler2DShadow u_shadows;

// flags
uniform bool u_alphaClip;
uniform bool u_debugLights;
uniform bool u_debugNormals;
uniform bool u_enableShadows;

uniform mat4 u_shadowsMatrix;

const int BLUR_SAMPLES = 6;

void main() {
    float shadow = 1.0;
    if (u_enableShadows) {
        vec4 mpos = u_shadowsMatrix * vec4(a_modelpos.xyz, 1.0);
        vec3 projCoords = mpos.xyz / mpos.w;
        projCoords = projCoords * 0.5 + 0.5;
        projCoords.z -= 0.0001;

        shadow = 0.0;
        
        for (int i = 0; i < BLUR_SAMPLES; i++) {
            shadow += texture(u_shadows, projCoords.xyz + vec3(
                -0.002*(i%2==0?1:0)*i/BLUR_SAMPLES / 4.0,
                -0.002*(i%2==0?0:1)*i/BLUR_SAMPLES / 4.0,
                -0.0005 * i/BLUR_SAMPLES));
        }
        shadow /= BLUR_SAMPLES;
        shadow = shadow * 0.5 + 0.5;
    }

    vec3 fogColor = texture(u_cubemap, a_dir).rgb;
    vec4 texColor = texture(u_texture0, a_texCoord);
    float alpha = a_color.a * texColor.a;
    if (u_alphaClip) {
        if (alpha < 0.2f)
            discard;
        alpha = 1.0;
    } else {
        if (alpha < 0.002f)
            discard;
    }
    if (u_debugLights)
        texColor.rgb = u_debugNormals ? (a_normal * 0.5 + 0.5) : vec3(1.0);
    else if (u_debugNormals) {
        texColor.rgb *= a_normal * 0.5 + 0.5;
    }
    f_color = a_color * texColor;
    f_color.rgb *= shadow;
    f_color = mix(f_color, vec4(fogColor,1.0), a_fog);
    f_color.a = alpha;
    f_position = vec4(a_position, 1.0);
    f_normal = vec4(a_normal, 1.0);
}
