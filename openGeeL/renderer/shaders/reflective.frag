#version 330 core

in vec3 Normal;
in vec3 Position;
in vec3 CameraPosition;
out vec4 color;

uniform samplerCube skybox;

void main() {
             
    vec3 I = normalize(Position - CameraPosition);
    vec3 Refl = reflect(I, normalize(Normal));
	vec3 Refr = refract(I, normalize(Normal), 1.00 / 1.52);

    color = texture(skybox, Refl) * 0.7 + texture(skybox, Refr) * 0.3;
}