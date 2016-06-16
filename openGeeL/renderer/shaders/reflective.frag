#version 330 core

struct Camera {
	vec3 position;
};

in vec3 Normal;
in vec3 Position;
out vec4 color;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform Camera camera;

void main() {
             
    vec3 I = normalize(Position - camera.position);
    vec3 Refl = reflect(I, normalize(Normal));
	vec3 Refr = refract(I, normalize(Normal), 1.00 / 1.52);

    color = texture(skybox, Refl) * 0.7 + texture(skybox, Refr) * 0.3;
}