#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "../headers/pre_processors.h"
#include "../headers/structs/camera.h"
#include "../headers/structs/particle.h"

Particle particles[NUM_PARTICLES];
Camera camera = {0, 0, 120, 0, 0};

#pragma region INIT AND UPDATE PARTICLES
void init_particles() {
    srand(time(NULL));
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        particles[i].x = ((float)rand() / RAND_MAX) * 100 - 50;
        particles[i].y = ((float)rand() / RAND_MAX) * 100 - 50;
        particles[i].z = ((float)rand() / RAND_MAX) * 100 - 50;
        particles[i].vx = ((float)rand() / RAND_MAX) * 2 - 1;
        particles[i].vy = ((float)rand() / RAND_MAX) * 2 - 1;
        particles[i].vz = ((float)rand() / RAND_MAX) * 2 - 1;
    }
}

void update_particles() {
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        particles[i].vy -= GRAVITY;

        particles[i].x += particles[i].vx;
        particles[i].y += particles[i].vy;
        particles[i].z += particles[i].vz;

        if (particles[i].x < -50 || particles[i].x > 50)
            particles[i].vx = -particles[i].vx;

        if (particles[i].y < -50 || particles[i].y > 50)
            particles[i].vy = -particles[i].vy;

        if (particles[i].z < -50 || particles[i].z > 50)
            particles[i].vz = -particles[i].vz;
    }
}
#pragma endregion

#pragma region DRAW CUBE PARTICLES
void draw_cube() {
    glBegin(GL_QUADS);

    // Front face
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);

    // Back face
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);

    // Left face
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);

    // Right face
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);

    // Top face
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);

    // Bottom face
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f, -0.5f,  0.5f);

    glEnd();
}
#pragma endregion

#pragma region RENDER PARTICLES
void render_particles() {
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        glPushMatrix();
        glTranslatef(particles[i].x, particles[i].y, particles[i].z);
        draw_cube();
        glPopMatrix();
    }
}
#pragma endregion

#pragma region SET PROJECTION AND VIEW MATRIX
void set_projection_matrix() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect_ratio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
    float zNear = 0.1f;
    float zFar = 1000.0f;
    float fH = tan(FOV / 360.0 * M_PI) * zNear;
    float fW = fH * aspect_ratio;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

void set_view_matrix() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float cosPitch = cosf(camera.pitch);
    float sinPitch = sinf(camera.pitch);
    float cosYaw = cosf(camera.yaw);
    float sinYaw = sinf(camera.yaw);

    float x = camera.x;
    float y = camera.y;
    float z = camera.z;

    float camX = x * cosYaw - z * sinYaw;
    float camZ = x * sinYaw + z * cosYaw;
    float camY = y * cosPitch - camZ * sinPitch;
    camZ = y * sinPitch + camZ * cosPitch;

    glRotatef(-camera.pitch * 180.0f / M_PI, 1.0f, 0.0f, 0.0f);
    glRotatef(-camera.yaw * 180.0f / M_PI, 0.0f, 1.0f, 0.0f);
    glTranslatef(-camera.x, -camera.y, -camera.z);
}
#pragma endregion

int main(int argc, char *argv[]) {
    #pragma region SDL INIT & CREATE WINDOW
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow(
                                            "3D Particle Simulator",
                                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                            WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    glewInit();
    #pragma endregion

    glEnable(GL_DEPTH_TEST);

    init_particles();

    #pragma region CAMERA MOVEMENT KEYS & RENDERING
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        camera.z -= 10;
                        break;
                    case SDLK_s:
                        camera.z += 10;
                        break;
                    case SDLK_a:
                        camera.x -= 10;
                        break;
                    case SDLK_d:
                        camera.x += 10;
                        break;
                    case SDLK_UP:
                        camera.pitch += 0.1f;
                        break;
                    case SDLK_DOWN:
                        camera.pitch -= 0.1f;
                        break;
                    case SDLK_LEFT:
                        camera.yaw += 0.1f;
                        break;
                    case SDLK_RIGHT:
                        camera.yaw -= 0.1f;
                        break;
                }
            }
        }

        update_particles();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        set_projection_matrix();
        set_view_matrix();

        render_particles();

        SDL_GL_SwapWindow(window);
        SDL_Delay(16); // Approximately 60 frames per second
    }
    #pragma endregion

    #pragma region DESTROY WINDOW WHEN CLOSING
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    #pragma endregion

    return EXIT_SUCCESS;
}