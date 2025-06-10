#ifndef SHADOWS_GLSL_
#define SHADOWS_GLSL_

uniform sampler2DShadow u_shadows[2];
uniform mat4 u_shadowsMatrix[2];
uniform float u_dayTime;

uniform int u_shadowsRes;

float calc_shadow() {
    float shadow = 1.0;
    if (u_enableShadows) {
        vec4 mpos = u_shadowsMatrix[0] * vec4(a_modelpos.xyz + a_realnormal * 0.04, 1.0);
        vec3 projCoords = mpos.xyz / mpos.w;
        projCoords = projCoords * 0.5 + 0.5;
        projCoords.z -= 0.0001;

        vec4 wmpos = u_shadowsMatrix[1] * vec4(a_modelpos.xyz + a_realnormal * 0.2, 1.0);
        vec3 wprojCoords = wmpos.xyz / wmpos.w;
        wprojCoords = wprojCoords * 0.5 + 0.5;
        wprojCoords.z -= 0.0001;

        shadow = 0.0;
        
        // TODO: optimize
        if (dot(a_realnormal, u_sunDir) < 0.0 || true) {
            if (a_distance > 128) {
                for (int y = -1; y <= 1; y++) {
                    for (int x = -1; x <= 1; x++) {
                        shadow += texture(
                            u_shadows[1],
                            wprojCoords.xyz +
                                vec3(x, y, -(abs(x) + abs(y))) /
                                    u_shadowsRes
                        );
                    }
                }
                shadow /= 9;
            } else {
                for (int y = -2; y <= 2; y++) {
                    for (int x = -2; x <= 2; x++) {
                        shadow += texture(
                            u_shadows[0],
                            projCoords.xyz +
                                vec3(x, y, -(abs(x) + abs(y))) /
                                    u_shadowsRes * 1.0
                        );
                    }
                }
                shadow /= 25;
            }
            float s = abs(cos(u_dayTime * 3.141592 * 2.0));
            s = pow(s, 0.7);
            shadow = mix(0.5, shadow * 0.5 + 0.5, s);
        } else {
            shadow = 0.5;
        }
    }
    return shadow;
}

#endif // SHADOWS_GLSL_
