#include <stdio.h>
#include <SDL2/SDL.h>
#include <pthread.h>


#undef main


#define EXIT_CODE 1


typedef struct Palette_var{
    Uint8 red, green, blue;
}Palette_t;


typedef struct Args_var{
    SDL_Renderer *renderer;
    Palette_t *arr_palette;
    int width, height, number;
    unsigned int depth;
    long double x0, y0, x1, y1;
    char flag;
    unsigned int *arr_pixels;
}Args_t;


typedef struct Args_h{
    long double x;
    long double y;
    unsigned int depth;
    unsigned int count;
}Args_I_t;


void* Iterative_transform(void* args_h){
    Args_I_t *args = (Args_I_t*) args_h;
    long double x = args->x, y = args->y;
    unsigned int depth = args->depth;
    long double help, x_start = x, y_start = y;
    for(unsigned int i = 1; i < depth; i++){
        help = x;
        x = x * x - y * y + x_start;
        y = 2 * help * y + y_start;
        if(x * x + y * y > 4.0){
            args->count = i;
            return 0;
        }
    }
    args->count = 0;
    return 0;
}


void Calc_color(Palette_t arr_palette[], unsigned int depth){
    for(unsigned int count = 0; count < depth; count++){
        if(count == 0){
            arr_palette[count].red = 0;
            arr_palette[count].green = 0;
            arr_palette[count].blue = 0;
        }
        else{
            if(count < 128){
                arr_palette[count].red = (Uint8) ((double) count * 1.8 + (double) 0.5);
                arr_palette[count].green = (Uint8) ((double) count * 1.9 + (double) 0.5);
                arr_palette[count].blue = count + count;
            }
            else if(count < 154){
                arr_palette[count].red = arr_palette[count - 1].red +  1;
                arr_palette[count].green += arr_palette[127].green + (Uint8) ((double) (count - 128) * 0.5 + (double) 0.5) + 1;
                arr_palette[count].blue = 255;
            }
            if(count < 410){
                arr_palette[count].red = arr_palette[152].red - (Uint8) (((double) count - 154) * 0.9 + (double) 0.5);
                if(arr_palette[count - 1].green > 0){
                    arr_palette[count].green = arr_palette[count - 1].green - 1;
                }
                arr_palette[count].blue = 255;
            }
            else if(count < 922){
                arr_palette[count].red = arr_palette[409].red + (Uint8) ((double) (count - 410) * 230.0 / 512.0);
                arr_palette[count].green = (Uint8) ((count - 410) / 2);
                arr_palette[count].blue = 255;
            }
            else if(count < 1946){
                arr_palette[count].red = arr_palette[921].red - (Uint8) ((count - 922) / 8);
                arr_palette[count].green = 255;
                arr_palette[count].blue = arr_palette[921].blue  - (Uint8) ((count - 922) / 32);
            }
            else if(count < 3994){
                arr_palette[count].red = arr_palette[1945].red;
                arr_palette[count].green = 255 - (Uint8) ((count - 1946) / 8);
                arr_palette[count].blue = arr_palette[1945].blue - (Uint8) ((count - 1946) / 32);
            }
            else if(count < 6092){
                arr_palette[count].red = arr_palette[3993].red;
                arr_palette[count].green = arr_palette[3993].green;
                arr_palette[count].blue = arr_palette[3993].blue - (Uint8) ((count - 3994) / 32);
            }
            else if(count < 10188){
                arr_palette[count].red = arr_palette[6091].red - (Uint8) ((double) ((unsigned int) ((count - 6092) / 32)) * 0.8);
                arr_palette[count].green = arr_palette[6091].green - (Uint8) ((count - 6092) / 32);
                arr_palette[count].blue = arr_palette[6092].blue - (Uint8) ((count - 6092) / 64);
            }
            else{
                Uint8 ii = (count - 10188) / 16;
                arr_palette[count].red = arr_palette[10188].red - ii;
                arr_palette[count].green = arr_palette[10188].blue - (Uint8) ((double) ii * 1.1);
                arr_palette[count].green = arr_palette[10188].blue - (Uint8) ((double) ii * 1.2);
            }
        }
    }
}


void* Draw_Fractal(void* args_h){
    Args_t *args = (Args_t*) args_h;
    SDL_Renderer *renderer = args->renderer;
    Palette_t *arr_palette = args->arr_palette;
    Args_I_t args_1;
    int width = args->width, height = args->height;
    unsigned int depth = args->depth, color1, color2, color3, color4;
    long double x0 = args->x0, y0 = args->y0, x1 = args->x1, y1 = args->y1;
    long double dx = (x1 - x0) / (long double) width, dy = (y1 - y0) / (long double) height, x_d = x0, y_d;
    y0 += (long double) args->number * dy * (long double) height / 4;
    args_1.depth = depth;
    for (int x = 0; x < width; x += 3) {
        y_d = y0;
        color1 = depth + 1; color2 = depth + 1;
        for (int y = height * args->number / 4; y < height * (args->number + 1) / 4; y += 2) {
            color3 = depth + 1; color4 = depth + 1;
            args_1.x = x_d; args_1.y = y_d;
            if(color1 == depth + 1) {
                Iterative_transform(&args_1);
                color1 = args_1.count;
            }
            if(x + 2 < width && y < height * (args->number + 1) / 4 && color2 == depth + 1){
                args_1.x = x_d + dx + dx; args_1.y = y_d;
                Iterative_transform(&args_1);
                color2 = args_1.count;
            }
            if(x < width && y + 2 < height * (args->number + 1) / 4){
                args_1.x = x_d; args_1.y = y_d + dy + dy;
                Iterative_transform(&args_1);
                color3 = args_1.count;
            }
            if(x + 2 < width && y + 2 < height * (args->number + 1) / 4){
                args_1.x = x_d + dx + dx; args_1.y = y_d + dy + dy;
                Iterative_transform(&args_1);
                color4 = args_1.count;
            }
            if(color1 == 0 && color2 == 0 && color3 == 0 && color4 == 0){
                for(int i = x; i < x + 3; i++){
                    for(int j = y; j < y + 3; j++){
                        args->arr_pixels[i + (j - args->number * height / 4) * width] = 0;
                    }
                }
            }
            else{
                for(int i = x; i < width && i < x + 3; i++){
                    for(int j = y; j < height * (args->number + 1) / 4 && j < y + 3; j++){
                        if(i == x && j == y){
                            args->arr_pixels[x + (y - args->number * height / 4) * width] = color1;
                            continue;
                        }
                        else if(i == x + 2 && j == y){
                            args->arr_pixels[x + 2 + (y - args->number * height / 4) * width] = color2;
                            continue;
                        }
                        else if(i == x && j == y + 2){
                            args->arr_pixels[x + (y + 2 - args->number * height / 4) * width] = color3;
                            continue;
                        }
                        if(i == x + 2 && j == y + 2){
                            args->arr_pixels[x + 2 + (y + 2 - args->number * height / 4) * width] = color4;
                            continue;
                        }
                        args_1.x = x_d + dx * (i - x); args_1.y = y_d + dy * (j - y);
                        Iterative_transform(&args_1);
                        args->arr_pixels[i + (j - args->number * height / 4) * width] = args_1.count;
                    }
                }
            }
            color1 = color3; color2 = color4;
            y_d += dy + dy;
        }
        x_d += dx + dx + dx;
    }
    if(args->flag == '1'){
        pthread_exit((void*) EXIT_CODE);
    }
    return NULL;
}


void Find_name_fp(char* path, int* number){
    FILE *fp = fopen(path, "r");
    char help[10];
    for(int i = 0; fread(help, sizeof(char), 10, fp) != 0; i++){
        fclose(fp);
        path[0] = 0;
        sprintf(path, "Mandelbrot_%d.bmp", (*number)++);
        fopen(path, "r");
    }
    fclose(fp);
}


void Work_with_SDL(SDL_Renderer* renderer, SDL_Window* window, Palette_t* arr_palette, int width, int height, unsigned int depth){
    SDL_Event event;
    pthread_t thread1, thread2, thread3, thread4;
    long double coefficient = (long double) width / (long double ) height, coefficient_rev = 1.0 / coefficient;
    int quit = 0, x1_help = 0, y1_help = 0, x2_help = 0, y2_help = 0, number_screen = 1;
    long double x1_d = 0.0, x2_d = 0.0, y1_d = 0.0, y2_d = 0.0;
    long double cx1 = -4.0, cx2 = 4.0, cy1 = -4.0 * coefficient_rev, cy2 = 4.0 * coefficient_rev;
    char flag, path[100] = "Mandelbrot_0.bmp";
    unsigned int *arr_pixels1 = (unsigned int*) malloc(sizeof(unsigned int) * width * height / 4);
    unsigned int *arr_pixels2 = (unsigned int*) malloc(sizeof(unsigned int) * width * height / 4);
    unsigned int *arr_pixels3 = (unsigned int*) malloc(sizeof(unsigned int) * width * height / 4);
    unsigned int *arr_pixels4 = (unsigned int*) malloc(sizeof(unsigned int) * width * height / 4);
    Args_t args1 = {.depth = depth, .width = width, .height = height, .renderer = renderer, .arr_palette = arr_palette, .flag = '1', .arr_pixels = arr_pixels1}, args2 = {.depth = depth, .width = width, .height = height, .renderer = renderer, .arr_palette = arr_palette, .flag = '1', .arr_pixels = arr_pixels2}, args3 = {.depth = depth, .width = width, .height = height, .renderer = renderer, .arr_palette = arr_palette, .flag = '1', .arr_pixels = arr_pixels3}, args4 = {.depth = depth, .width = width, .height = height, .renderer = renderer, .arr_palette = arr_palette, .flag = '1', .arr_pixels = arr_pixels4};
    while(1) {
        flag = '0';
        while (flag == '0') {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    free(arr_pixels1); free(arr_pixels2); free(arr_pixels3); free(arr_pixels4);
                    return;
                }
                else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    SDL_GetMouseState(&x1_help, &y1_help);
                    x1_d = (long double) x1_help; y1_d = (long double) y1_help;
                    continue;
                }
                else if (event.type == SDL_MOUSEBUTTONUP) {
                    SDL_GetMouseState(&x2_help, &y2_help);
                    x2_d = (long double) x2_help; y2_d = (long double ) y2_help;
                    if (y2_d < y1_d) {
                        long double c = y2_d;
                        y2_d = y1_d;
                        y1_d = c;
                    }
                    if (x2_d < x1_d) {
                        long double c = x2_d;
                        x2_d = x1_d;
                        x1_d = c;
                    }
                    if (x2_d - x1_d > coefficient * (y2_d - y1_d)) {
                        y2_d = (x2_d - x1_d) * coefficient_rev  + y1_d;
                        if (y2_d > (long double) height) {
                            y1_d -= y2_d - (long double) height;
                            y2_d = (long double) height;
                        }
                    }
                    else {
                        x2_d = x1_d + coefficient * (y2_d - y1_d);
                        if (x2_d > (long double) width) {
                            x1_d -= x2_d - (long double) width;
                            x2_d = (long double) width;
                        }
                    }
                    SDL_Rect rect = {.x = (int) x1_d, .y = (int) y1_d, .w = (int) (x2_d - x1_d), .h = (int) (y2_d - y1_d)};
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    SDL_RenderDrawRect(renderer, &rect);
                    SDL_RenderPresent(renderer);
                    continue;
                }
                else if (event.type == SDL_KEYDOWN) {
                    flag = '1';
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        free(arr_pixels1); free(arr_pixels2); free(arr_pixels3); free(arr_pixels4);
                        return;
                    }
                    else if(event.key.keysym.scancode == SDL_SCANCODE_PRINTSCREEN || event.key.keysym.scancode == SDL_SCANCODE_S){
                        Find_name_fp(path, &number_screen);
                        SDL_Surface* screen_surf = SDL_GetWindowSurface(window);
                        if (SDL_RenderReadPixels(renderer, NULL, 0, screen_surf->pixels, screen_surf->pitch) != 0)
                        {
                            // error use SDL_GetError()
                        }
                        SDL_SaveBMP_RW(screen_surf, SDL_RWFromFile(path, "wb"), 1);
                    }
                    else if (event.key.keysym.scancode == SDL_SCANCODE_RETURN || event.key.keysym.scancode == SDL_SCANCODE_RETURN2) {
                        long double dcx = cx2 - cx1, dcy = cy2 - cy1;
                        cx2 = cx1 + x2_d * dcx / (long double) width;  cy2 = cy1 + y2_d * dcy / (long double) height;
                        cx1 += x1_d * dcx / (long double) width;  cy1 += y1_d * dcy / (long double) height;
                    }
                    else if(event.key.keysym.scancode == SDL_SCANCODE_D){
                        flag = '0';
                        SDL_RenderPresent(renderer);
                        continue;
                    }
                    else if(event.key.keysym.scancode == SDL_SCANCODE_EQUALS){
                        long double delta = (cx2 - cx1) / 4.0;
                        cx1 += delta; cx2 -= delta;
                        delta = (cy2 - cy1) / 4.0;
                        cy1 += delta; cy2 -= delta;
                    }
                    else if(event.key.keysym.scancode == SDL_SCANCODE_MINUS){
                        long double delta = (cx2 - cx1) / 2.0;
                        cx1 -= delta; cx2 += delta;
                        delta = (cy2 - cy1) / 2.0;
                        cy1 -= delta; cy2 += delta;
                    }
                    else if (event.key.keysym.scancode == SDL_SCANCODE_R) {
                        cx1 = -4.0; cx2 = 4.0; cy1 = -4.0 * coefficient_rev; cy2 = 4.0 * coefficient_rev;
                    }
                }
            }
        }
        args1.x0 = cx1; args1.x1 = cx2; args1.y0 = cy1; args1.y1 = cy2; args1.number = 0;
        args2.x0 = cx1; args2.x1 = cx2; args2.y0 = cy1; args2.y1 = cy2; args2.number = 1;
        args3.x0 = cx1; args3.x1 = cx2; args3.y0 = cy1; args3.y1 = cy2; args3.number = 2;
        args4.x0 = cx1; args4.x1 = cx2; args4.y0 = cy1; args4.y1 = cy2; args4.number = 3;
        if(pthread_create(&thread1, NULL, Draw_Fractal, &args1) != 0){
            printf("can't create thread1\n");
            return;
        }
        if(pthread_create(&thread2, NULL, Draw_Fractal, &args2) != 0){
            printf("can't create thread2\n");
            return;
        }
        if(pthread_create(&thread3, NULL, Draw_Fractal, &args3) != 0){
            printf("can't create thread3\n");
            return;
        }
        if(pthread_create(&thread4, NULL, Draw_Fractal, &args4) != 0){
            printf("can't create thread3\n");
            return;
        }
        void *res1, *res2, *res3, *res4;
        pthread_join(thread1, res1);
        pthread_join(thread2, res2);
        pthread_join(thread3, res3);
        pthread_join(thread4, res4);
        for(int y = 0; y < height / 4; y++){
            for(int x = 0; x < width; x++){
                unsigned int color = arr_pixels1[x + y * width];
                SDL_SetRenderDrawColor(renderer, arr_palette[color].red, arr_palette[color].green, arr_palette[color].blue, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
        for(int y = 0; y < height / 4; y++){
            for(int x = 0; x < width; x++){
                unsigned int color = arr_pixels2[x + y * width];
                SDL_SetRenderDrawColor(renderer, arr_palette[color].red, arr_palette[color].green, arr_palette[color].blue, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawPoint(renderer, x, y + height / 4);
            }
        }
        for(int y = 0; y < height / 4; y++){
            for(int x = 0; x < width; x++){
                unsigned int color = arr_pixels3[x + y * width];
                SDL_SetRenderDrawColor(renderer, arr_palette[color].red, arr_palette[color].green, arr_palette[color].blue, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawPoint(renderer, x, y + height / 2);
            }
        }
        for(int y = 0; y < height / 4; y++){
            for(int x = 0; x < width; x++){
                unsigned int color = arr_pixels4[x + y * width];
                SDL_SetRenderDrawColor(renderer, arr_palette[color].red, arr_palette[color].green, arr_palette[color].blue, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawPoint(renderer, x, y + 3 * height / 4);
            }
        }
        SDL_RenderPresent(renderer);
    }
}


int main() {
    int height, width;
    unsigned int depth;
    printf("Enter width of window>");
    if(scanf("%u", &width) != 1){
        printf("\nbad input");
        return 0;
    }
    printf("Enter height of window>");
    if(scanf("%u", &height) != 1){
        printf("\nbad input");
        return 0;
    }
    printf("Enter depth of cycle>");
    if(scanf("%u", &depth) != 1){
        printf("\nbad input");
        return 0;
    }

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Pre-Defined Fill Styles",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);


    Palette_t arr_palette[depth];
    Calc_color(arr_palette, depth);
    Work_with_SDL(renderer, window, arr_palette, width, height, depth);


    SDL_Surface* screen_surf = SDL_GetWindowSurface(window);
    if (SDL_RenderReadPixels(renderer, NULL, 0, screen_surf->pixels, screen_surf->pitch) != 0)
    {
        // error use SDL_GetError()
    }
    SDL_SaveBMP_RW(screen_surf, SDL_RWFromFile("Mandelbrot.bmp", "wb"), 1);


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}