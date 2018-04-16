//#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h> // for usleep

#define SIZE 32 // Best not to raise this very high

extern void dens_step ( int M, int N, int O, float * x, float * x0, float * u, float * v, float * w, float diff, float dt );
extern void vel_step (int M, int N, int O, float * u, float * v,  float * w, float * u0, float * v0, float * w0, float visc, float dt );


//fluid field information
static const int M = SIZE; // grid x
static const int N = SIZE; // grid y
static const int O = SIZE; // grid z
static float diff = 0.0f; // diffuse
static float visc = 0.0f; // viscosity


static float *u, *v, *w, *u_prev, *v_prev, *w_prev;
static float *dens, *dens_prev;

#define IX(i,j,k) ((i)+(M+2)*(j) + (M+2)*(N+2)*(k))


#define NUM_PARTICLES 100
static float *p_x;
static float *p_y;
static float *p_z;


void allocate_it() {
    const int size = (M+2)*(N+2)*(O+2);

    u         = (float *) malloc(size * sizeof(float));
    v         = (float *) malloc(size * sizeof(float));
    w         = (float *) malloc(size * sizeof(float));
    u_prev    = (float *) malloc(size * sizeof(float));
    v_prev    = (float *) malloc(size * sizeof(float));
    w_prev    = (float *) malloc(size * sizeof(float));
    dens      = (float *) malloc(size * sizeof(float));
    dens_prev = (float *) malloc(size * sizeof(float));

    p_x = (float*) malloc(NUM_PARTICLES * sizeof(float));
    p_y = (float*) malloc(NUM_PARTICLES * sizeof(float));
    p_z = (float*) malloc(NUM_PARTICLES * sizeof(float));
}

void free_it() {
}

float randf() {
    return (float)rand() / (float)RAND_MAX;
}

void random_particles() {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        p_x[i] = (M/2.0f) + (randf() - 0.5f) * 16.0f;
        p_y[i] = (N/2.0f) + (randf() - 0.5f) * 16.0f;
        p_z[i] = 4.0f; // Above ground
    }
}

void particles_step(float* u, float *v, float *w, float dt) {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        if (p_x[i] > 0 && p_x[i] < M &&
            p_y[i] > 0 && p_y[i] < N &&
            p_z[i] > 0 && p_z[i] < O) {

            const int idx = IX((int)p_x[i], (int)p_y[i], (int)p_z[i]);        
            p_x[i] += u[idx] * dt;
            p_y[i] += v[idx] * dt;
            p_z[i] += w[idx] * dt;
        } else {
            fprintf(stderr, "outside\n");
        }
    }
}

void add_densities(float *d) {
    /*const int size = (M+2) * (N+2) * (O+2);
    for (int i = 0; i < size; i++) {
        d[i] = 0.0f;
    }*/

    d[IX(M / 2 + 1, N / 2, O / 2)] = 10;
    d[IX(M / 2 - 1, N / 2, O / 2)] = 10;

    d[IX(M / 2, N / 2 + 1, O / 2)] = 10;
    d[IX(M / 2, N / 2 - 1, O / 2)] = 10;
    d[IX(M / 2, N / 2, O / 2)] = 5;
}

void add_velocities(float *u, float *v, float *w) {
    const int size = (M+2) * (N+2) * (O+2);
    for (int i = 0; i < size; i++) {
        u[i] = v[i] = w[i] = 0.0f;
    }
    
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < M; x++) {
            u[IX(x, y, 4)] = 0.0f;
            v[IX(x, y, 4)] = 0.2f;
            w[IX(x, y, 4)] = 0.0f;
        }
    }
}

void simulate(float dt) {    
    vel_step(M, N, O, u, v, w, u_prev, v_prev,w_prev, visc, dt);
    dens_step(M, N, O, dens, dens_prev, u, v, w, diff, dt);

    // step particles
    particles_step(u, v, w, dt);
}

void render(unsigned char* buffer, int width, int height) {
    for (int i = 0; i < width*height*3; i++) {
        buffer[i] = 33;
    }
    for (int i = 0; i < NUM_PARTICLES; i++) {
        float x = width/2.0f + p_x[i] * 5;
        float y = height/2.0f + p_y[i] * 5;
        if (x > 0 && x < width &&
            y > 0 && y < height) {
            //fprintf(stderr, "x: %.4f, y: %.4f\n", x, y);
            const int idx = ((int)y * width + (int)x) * 3;
            buffer[idx + 0] = 200;
            buffer[idx + 1] = 100;
            buffer[idx + 1] = 0;
        }
    }
}

void _write(const unsigned char* buffer, size_t size, FILE* target) {
    if (fwrite(buffer, 3, size, target) != size) {
        perror("fwrite");
        exit(-1);
    }
}

void write_particles(FILE *target) {
    if (fwrite(p_x, sizeof(float), NUM_PARTICLES, target) != NUM_PARTICLES) {
        perror("fwrite");
        exit(-1);
    }
    if (fwrite(p_y, sizeof(float), NUM_PARTICLES, target) != NUM_PARTICLES) {
        perror("fwrite");
        exit(-1);
    }
    if (fwrite(p_y, sizeof(float), NUM_PARTICLES, target) != NUM_PARTICLES) {
        perror("fwrite");
        exit(-1);
    }
    fflush(target);
}

int main() {
    const float dt = 0.4f; // delta time

    srand(1337);
    //const int WIDTH = 320;
    //const int HEIGHT = 200;
    //unsigned char buffer[WIDTH * HEIGHT * 3];
    
    allocate_it();
    random_particles();
    while (1) {
        //add_densities(dens_prev);
        add_velocities(u_prev, v_prev, w_prev);

        simulate(dt);
        write_particles(stdout);
        //render(buffer, WIDTH, HEIGHT);
        //_write(buffer, WIDTH * HEIGHT, stdout);
        usleep(1000000 / 4);
    }

    return 0;
}
