#ifndef SHADOWS_GLSL_
#define SHADOWS_GLSL_

float sample_shadow(vec3 uv, vec3 xy) {
    float color = 0.0;
    vec3 off1 = vec3(1.3846153846) * xy;
    vec3 off2 = vec3(3.2307692308) * xy;
    color += texture(u_shadows, uv) * 0.2270270270;
    color += texture(u_shadows, uv + (off1 / u_shadowsRes)) * 0.3162162162;
    color += texture(u_shadows, uv - (off1 / u_shadowsRes)) * 0.3162162162;
    color += texture(u_shadows, uv + (off2 / u_shadowsRes)) * 0.0702702703;
    color += texture(u_shadows, uv - (off2 / u_shadowsRes)) * 0.0702702703;
    return color;
}

float calc_shadow() {
    float shadow = 1.0;
    if (u_enableShadows) {
        vec4 mpos = u_shadowsMatrix * vec4(a_modelpos.xyz + a_realnormal * 0.04, 1.0);
        vec3 projCoords = mpos.xyz / mpos.w;
        projCoords = projCoords * 0.5 + 0.5;
        projCoords.z -= 0.0001;

        shadow = 0.0;
        
        if (dot(a_realnormal, u_sunDir) < 0.0) {
            const vec3 offsets[4] = vec3[4](
                vec3(0.5, 0.5, 0.0),
                vec3(-0.5, 0.5, 0.0),
                vec3(0.5, -0.5, 0.0),
                vec3(-0.5, -0.5, 0.0)
            );
            for (int i = 0; i < 4; i++) {
                shadow += texture(u_shadows, projCoords.xyz + offsets[i] / u_shadowsRes);
            }
            shadow /= 4;
            //shadow = sample_shadow(projCoords, normalize(vec3(1.0, 1.0, 0.0)) * 0.5);
            shadow = shadow * 0.5 + 0.5;
        } else {
            shadow = 0.5;
        }
    }
    return shadow;
}

#endif // SHADOWS_GLSL_
