#include<stdio.h>
#include<math.h>
#include<string.h>


typedef struct Projection
{
    float theta_spacing, phi_spacing;

    int screen_width, screen_height;

    float R1, R2, K1, K2;
} Projection;

void render(float theta, float phi, Projection * ptrProj);


int main(void)
{
    Projection projection;
    projection.screen_width = 100;
    projection.screen_height = 30;

    projection.theta_spacing = 0.07;
    projection.phi_spacing = 0.07;

    
    // printf("%s", projection.output);
    projection.R1 = 1; // raio
    projection.R2 = 5; // centro do circulo partindo do eixo x

    projection.K2 = 100;
    // calcular K1 , o desenvolvimento e razao da conta estao no meu caderno
    projection.K1 = ((projection.screen_height*0.4 * projection.K2) / (projection.R1 + projection.R2));
    Projection * ptrProj = &projection;

    // render(10.0, 10.0, ptrProj);

    // int r1, r2, k2;

    float A = 0;
    float B = 0;
    do {
        render(A, B, ptrProj);
        // char a = getchar();
        // projection.R1 += 0.0001;
        A += 0.001;
        B += 0.0001;
        // projection.K2 += 0.001;
        // projection.R2 -= 0.0004;

        // scanf("%d %d %d", &r1, &r2, &k2);
        // projection.R1 = r1;
        // projection.R2 = r2;
        // projection.K2 = k2;
        
    } while (1);
}   

void render(float A, float B, Projection * ptrProj)
{

    char output[100][30];
    memset(output, ' ', sizeof(char)*ptrProj->screen_height*ptrProj->screen_width);

    float cos_a = cos(A);
    float sin_a = sin(A);

    float cos_b = cos(B);
    float sin_b = sin(B);

//     char output[71][21];

    // for (int j = 0; j < ptrProj->screen_height; j++)
    // {
        // for (int i = 0; j < ptrProj->screen_width; i++)
        // {
            // output[i][j] = '#';
    //     // }
    // }

    // char z_buffer[200][100] = {0};

    for (float theta = 0; theta < 2*3.14; theta+= ptrProj->theta_spacing)
    {
        float cos_t = cos(theta);
        float sin_t = sin(theta);

        for (float phi = 0; phi < 2*3.14; phi += ptrProj->phi_spacing)
        {    
            float cos_p = cos(phi);
            float sin_p = sin(phi);

            float r1_r2 =  ptrProj->R2 + ptrProj->R1*cos_t;

            float circlex = (r1_r2)*cos_p;
            float circley = (ptrProj->R1*sin_t*cos_a) + r1_r2*sin_p*sin_a;
            float circlez =  (-sin_p * r1_r2*cos_a + ptrProj->R1*sin_t*sin_a);

            // rotacionar em torno do eixo z
            circlex = circlex*cos_b - circley*sin_b;
            circley = circlex*sin_b + circley*cos_b;

            // rotacionar em torno do eixo y
            circlex = circlex*cos_a + circlez*sin_a;
            circlez = circlex*(-sin_a) + circlez*cos_a;
        
            circlez += ptrProj->K2; 
            // Projecaox
            float z_inverso = 1/circlez;
            // printf("%f", z_inverso);

            // e preciso adicionar metade da tela pois o circulo foi definido a partir da metade
            // e o sistema de coordenadas comeca no canto
            // alem disso, y deve ser invertido pois no nosso sistema y+ para cima
            // porem o buffer que definimos toma y+ para baixo
            int x = (int) (ptrProj->screen_width/2 + ( ptrProj->K1 * circlex * z_inverso));
            int y = (int) (ptrProj->screen_height/2 - (ptrProj->K1 * circley * z_inverso));
          
            float nx = cos_t;
            float ny = sin_t;
            float nz = 0;

            // rotacionando em torno do eixo y
            nx = nx*cos_p ;
            nz = -nx*sin_p;

            // rot em torno do eixo x
            ny = cos_a*ny  - sin_a*nz;
            nz = ny*sin_a + nz*cos_a;

            // rot em torno do eixo z
            nx = nx*cos_b - ny*sin_b;
            ny = nx*sin_b + ny*cos_b; 


            output[x][y] = 'O';
           
    }
    }

    printf("\x1b[H");
    for (int j = 0; j < ptrProj->screen_height; j++)
    {
        for (int i = 0 ; i <ptrProj->screen_width; i++)
        {
            putchar(output[i][j]);
        }
        putchar('\n');
    }
    
    
}
