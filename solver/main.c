//#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SIZE 32 // Best not to raise this very high

extern void dens_step ( int M, int N, int O, float * x, float * x0, float * u, float * v, float * w, float diff, float dt );
extern void vel_step (int M, int N, int O, float * u, float * v,  float * w, float * u0, float * v0, float * w0, float visc, float dt );


//fluid field information
static const int M = SIZE; // grid x
static const int N = SIZE; // grid y
static const int O = SIZE; // grid z
static float diff = 0.0f; // diffuse
static float visc = 0.0f; // viscosity


/*static float force = 10.0f;  // added on keypress on an axis
static float source = 200.0f; // density
static float source_alpha =  0.05; //for displaying density

static int addforce[3] = {0, 0, 0};
static int addsource = 0;
*/

static float *u, *v, *w, *u_prev, *v_prev, *w_prev;
static float *dens, *dens_prev;

void allocate_it() {
    int size = (M+2)*(N+2)*(O+2);

    u         = (float *) malloc(size * sizeof(float));
    v         = (float *) malloc(size * sizeof(float));
    w         = (float *) malloc(size * sizeof(float));
    u_prev    = (float *) malloc(size * sizeof(float));
    v_prev    = (float *) malloc(size * sizeof(float));
    w_prev    = (float *) malloc(size * sizeof(float));
    dens      = (float *) malloc(size * sizeof(float));
    dens_prev = (float *) malloc(size * sizeof(float));
}

void free_it() {
}

#define IX(i,j,k) ((i)+(M+2)*(j) + (M+2)*(N+2)*(k))
void add_densities(float *d) {
    const int size = (M+2) * (N+2) * (O+2);
    /*for (int i = 0; i < size; i++) {
        d[i] = 0.0f;
    }*/

    d[IX(M / 2 + 1, N / 2, O / 2)] = 1;
    d[IX(M / 2 - 1, N / 2, O / 2)] = 1;

    d[IX(M / 2, N / 2 + 1, O / 2)] = 1;
    d[IX(M / 2, N / 2 - 1, O / 2)] = 1;
    d[IX(M / 2, N / 2, O / 2)] = 5;
}

void add_velocities(float *u, float *v, float *w) {
    const int size = (M+2) * (N+2) * (O+2);
    for (int i = 0; i < size; i++) {
        u[i] = v[i] = w[i] = 0.0f;
    }
    
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < M; x++) {
            u[IX(x, y, O / 2)] = 0.0f;
            v[IX(x, y, O / 2)] = 0.1f;
            w[IX(x, y, O / 2)] = 0.0f;
        }
    }
}

void simulate(float dt) {
    add_densities(dens_prev);
    add_velocities(u_prev, v_prev, w_prev);
    
    vel_step(M, N, O, u, v, w, u_prev, v_prev,w_prev, visc, dt);
    dens_step(M, N, O, dens, dens_prev, u, v, w, diff, dt);
}

void render(unsigned char* buffer, int width, int height) {
/*    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
        }
    }*/
    float hi = -INFINITY;
    float low = INFINITY;
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < M; x++) {
            for (int z = 0; z < O; z++) {
                if (dens[IX(x, y, z)] > hi) {
                    hi = dens[IX(x, y, z)];
                }
                if (dens[IX(x, y, z)] < low) {
                    low = dens[IX(x, y, z)];
                }
            }
        }
    }

    for (int y = 0; y < N; y++) {
        for (int x = 0; x < M; x++) {
            int v = 0;
            for (int z = 0; z < O; z++) {
                v += dens[IX(x, y, z)];
            }
            
            const int idx = (y * width + x);
            const float scaled = (255.0f * (v - low) / (hi - low));
            buffer[idx * 3 + 0] = scaled;
            buffer[idx * 3 + 1] = scaled;
            buffer[idx * 3 + 2] = v;
        }
    }
}

void _write(const unsigned char* buffer, size_t size, FILE* target) {
    if (fwrite(buffer, 3, size, target) != size) {
        perror("fwrite");
    }
}

int main() {
    const float dt = 0.4f; // delta time

    const int WIDTH = 320;
    const int HEIGHT = 200;

    unsigned char buffer[WIDTH * HEIGHT * 3];
    
    allocate_it();
    while (1) {
        simulate(dt);
        render(buffer, WIDTH, HEIGHT);
        _write(buffer, WIDTH * HEIGHT, stdout);
    }

    return 0;
}
