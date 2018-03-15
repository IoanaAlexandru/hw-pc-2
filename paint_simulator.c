#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#define EPERM 1
#define EINVAL 2
#define ENOMEM 3

int main()
{
    int opCode;
    int width, height;
    pixel **img;

    int start_col, start_line, end_col, end_line;
    int new_width, new_height;
    unsigned char r, g, b;
    int num_iter, num_rot;

    scanf("%d", &opCode);
    if(opCode == 1) {
        scanf(" %d %d", &width, &height);

        if (height < 1 || height > 1024 ||
                width < 1 || width > 1024) {
                fprintf(stderr, "%d\n", EINVAL);
                exit(EXIT_FAILURE);
            }

        img = init_img(width, height);
    }
    else {
        fprintf(stderr, "%d\n", EPERM);
        exit(EXIT_FAILURE);
    }

    do {
        scanf("%d", &opCode);

        if (opCode == 1) {
            int height_copy = height;

            scanf(" %d %d", &width, &height);

            if (height < 1 || height > 1024 ||
                width < 1 || width > 1024) {
                fflush(0);
                fprintf(stderr, "%d\n", EINVAL);
                free_img(&img, height_copy);
                exit(EXIT_FAILURE);
            }

            free_img(&img, height_copy);
            img = init_img(width, height);
        }

        if (opCode == 2) {
            pixel **cropped;

            scanf("%d %d %d %d", &start_col, &start_line, &end_col, &end_line);

            if (start_line < 0 || start_line > end_line || end_line >= height ||
                start_col < 0 || start_col > end_col || end_col >= width) {
                fprintf(stderr, "%d\n", EINVAL);
                free_img(&img, height);
                exit(EXIT_FAILURE);
            }

            cropped = crop(img, start_col, start_line, end_col, end_line);
            free_img(&img, height);
            img = cropped;

            width = end_col - start_col + 1;
            height = end_line - start_line + 1;
        }

        if (opCode == 3) {
            pixel **resized;

            scanf("%d %d", &new_width, &new_height);

            if (new_height < 1 || new_height > 1024 ||
                new_width < 1 || new_width > 1024) {
                fprintf(stderr, "%d\n", EINVAL);
                free_img(&img, height);
                exit(EXIT_FAILURE);
            }

            resized = resize(img, width, height, new_width, new_height);
            free_img(&img, height);
            img = resized;

            width = new_width;
            height = new_height;
        }

        if (opCode == 4) {
            scanf(" %d %d %d %d %hhu %hhu %hhu", &start_col, &start_line,
                    &end_col, &end_line, &r, &g, &b);

            if (start_line < 0 || start_line > end_line || end_line >= height ||
                start_col < 0 || start_col > end_col || end_col >= width ||
                r > 255 || r < 0 || g > 255 || g < 0 || r > 255 || g < 0) {
                fprintf(stderr, "%d\n", EINVAL);
                free_img(&img, height);
                exit(EXIT_FAILURE);
            }

            color_region(&img, start_col, start_line,
                            end_col, end_line, r, g, b);
        }

        if (opCode == 5) {
            scanf(" %d", &num_iter);

            if (num_iter > 2000 || num_iter < 0) {
                fflush(0);
                fprintf(stderr, "%d\n", EINVAL);
                free_img(&img, height);
                exit(EXIT_FAILURE);
            }

            blur(&img, width, height, num_iter);
        }

        if (opCode == 6) {
            scanf(" %d", &num_rot);

            if (num_rot > 3 || num_rot < 1) {
                fprintf(stderr, "%d\n", EINVAL);
                free_img(&img, height);
                exit(EXIT_FAILURE);
            }

            rotate(&img, &width, &height, num_rot);
        }

        if (opCode == 7) {
            scanf(" %d %d %hhu %hhu %hhu", &start_col, &start_line, &r, &g, &b);

            if (start_line < 0 || start_col < 0 ||
                start_line >= width || start_col >= height ||
                r > 255 || r < 0 || g > 255 || g < 0 || r > 255 || g < 0) {
                fprintf(stderr, "%d\n", EINVAL);
                free_img(&img, height);
                exit(EXIT_FAILURE);
            }

            pixel init = img[start_line][start_col];
            // ^ culoare iniţială ^
            pixel change; change.r = r; change.g = g; change.b = b;
            // ^ culoare schimbată ^

            if(init.r != change.r || init.g != change.g || init.b != change.b ){
                fill(&img, width, height, start_col, start_line, init, change);
            }
        }

        if (opCode == 8) {
            printf("%d %d\n", width, height);
            print_img(img, width, height);
        }

        if (opCode > 8 || opCode < 0) {
            fprintf(stderr, "%d\n", EPERM);
            free_img(&img, height);
            exit(EXIT_FAILURE);
        }
    } while (opCode);

    free_img(&img, height);

    return 0;
}
