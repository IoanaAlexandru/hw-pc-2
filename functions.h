#include <stdio.h>
#include <stdlib.h>
#define EPERM 1
#define EINVAL 2
#define ENOMEM 3

typedef struct {
    unsigned char r, g, b;
} pixel;

pixel** get_img(int width, int height)
{
    int i, j;
    pixel **img;

    img = malloc(height * sizeof(unsigned char *));
    if (img == NULL) {
        return NULL;
    }

    for (i = 0; i < height; i++) {
        img[i] = calloc(3 * width, sizeof(unsigned char));
        if (img[i] == NULL) {
            for (j = 0; j < i; j++) {
                free(img[j]);
            }
            free(img);
            return NULL;
        }
    }

    return img;
}

void free_img(pixel ***img, int height)
{
    int i;

    for (i = 0; i < height; i++) {
        free((*img)[i]);
    }

    free(*img);
    *img = NULL;
}

void read_img(pixel **img, int width, int height)
{
    int i, j;

    for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                scanf("%hhu %hhu %hhu",
                        &img[i][j].r, &img[i][j].g, &img[i][j].b);
            }
        }
}

pixel** init_img(int width, int height)
{
    pixel **img;

    img = get_img(width, height);
    if (img == NULL) {
        fprintf(stderr, "%d\n", ENOMEM);
        exit(EXIT_FAILURE);
    }

    read_img(img, width, height);
    return img;
}

void print_img(pixel **img, int width, int height)
{
    int i, j;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            printf("%hhu %hhu %hhu ", img[i][j].r, img[i][j].g, img[i][j].b);
        }
        printf("\n");
    }
}

pixel** crop(pixel **img, int start_col, int start_line,
            int end_col, int end_line)
{
    int i, j;
    pixel **cropped;
    int new_width = end_col - start_col + 1;
    int new_height = end_line - start_line + 1;

    cropped = get_img(new_width, new_height);

    if (cropped == NULL) {
            fprintf(stderr, "%d\n", ENOMEM);
            exit(EXIT_FAILURE);
    }

    for (i = 0; i < new_height; i++) {
        for (j = 0; j < new_width; j++) {
            cropped[i][j] = img[start_line + i][start_col + j];
        }
    }
    return cropped;
}

pixel** resize(pixel **img, int width, int height,
                int new_width, int new_height)
{
    pixel **resized;
    int i, j;
    if (new_width == width && new_height == height) {
        resized = img;
    }
    else {
        if (new_width < width && new_height < height) {
            resized = crop(img, 0, 0, new_width - 1, new_height - 1);
        }
        else {
            resized =  get_img(new_width, new_height);
            if (resized == NULL) {
                    fprintf(stderr, "%d\n", ENOMEM);
                    exit(EXIT_FAILURE);
            }

            for (i = 0; i < new_height; i++) {
                for (j = 0; j < new_width; j++) {
                    if (i < height && j < width) {
                        resized[i][j] = img[i][j];
                    }
                    else {
                        resized[i][j].r = 255;
                        resized[i][j].g = 255;
                        resized[i][j].b = 255;
                    }
                }
            }
        }
    }
    return resized;
}

void color_region(pixel ***img, int start_col, int start_line,
                    int end_col, int end_line, unsigned char r,
                    unsigned char g, unsigned char b)
{
    int i, j;
    for (i = start_line; i <= end_line; i++) {
        for (j = start_col; j <= end_col; j++) {
            (*img)[i][j].r = r;
            (*img)[i][j].g = g;
            (*img)[i][j].b = b;
        }
    }
}

void blur(pixel ***img, int width, int height, int num_iter)
{
    int i, j, k, nr_neighbors;

    pixel **blurred = get_img(width + 2, height + 2);

    if (blurred == NULL) {
            fprintf(stderr, "%d\n", ENOMEM);
            exit(EXIT_FAILURE);
    }

    for(i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            blurred[i + 1][j + 1] = (*img)[i][j];
        }
    }

    free_img(img, height);

    *img = get_img(width + 2, height + 2);

    if (*img == NULL) {
            fprintf(stderr, "%d\n", ENOMEM);
            exit(EXIT_FAILURE);
    }

    for(i = 0; i <= height + 1; i++) {
        for (j = 0; j <= width + 1; j++) {
            (*img)[i][j] = blurred[i][j];
        }
    }

    //am schimbat matricea img astfel încât să fie înconjurată de zerouri

    for (k = 1; k <= num_iter; k++) {
        for (i = 1; i <= height; i++) {
            for (j = 1; j <= width; j++) {
                nr_neighbors = 4;

                if (i == 1 || i == height) {
                    nr_neighbors--;
                }
                if (j == 1 || j == width) {
                    nr_neighbors--;
                }

                blurred[i][j].r = ((*img)[i - 1][j].r + (*img)[i + 1][j].r +
                                    (*img)[i][j - 1].r + (*img)[i][j + 1].r) /
                                    nr_neighbors;
                blurred[i][j].g = ((*img)[i - 1][j].g + (*img)[i + 1][j].g +
                                    (*img)[i][j - 1].g + (*img)[i][j + 1].g) /
                                    nr_neighbors;
                blurred[i][j].b = ((*img)[i - 1][j].b + (*img)[i + 1][j].b +
                                    (*img)[i][j - 1].b + (*img)[i][j + 1].b) /
                                    nr_neighbors;
            }
        }
        for(i = 0; i <= height + 1; i++) {
            for (j = 0; j <= width + 1; j++) {
                (*img)[i][j] = blurred[i][j];
            }
        }
    }
    free_img(img, height + 2);
    *img = crop(blurred, 1, 1, width, height);
    free_img(&blurred, height + 2);
}

void rotate(pixel ***img, int *width, int *height, int num_rot)
{
    pixel** rotated = get_img(*height, *width);
    if (rotated == NULL) {
        fprintf(stderr, "%d\n", ENOMEM);
        exit(EXIT_FAILURE);
    }

    int i, j, k, copy;

    for (k = 1; k <= num_rot; k++) {
        for (i = 0; i < *width; i++) {
            for (j = 0; j < *height; j++) {
                rotated[i][j] = (*img)[*height - j - 1][i];
            }
        }

        free_img(img, *height);
        *img = get_img(*height, *width);

        if (*img == NULL) {
            fprintf(stderr, "%d\n", ENOMEM);
            exit(EXIT_FAILURE);
        }

        for(i = 0; i < *width; i++) {
            for (j = 0; j < *height; j++) {
                (*img)[i][j] = rotated[i][j];
            }
        }

        free_img(&rotated, *width);
        rotated = get_img(*width, *height);
        if (rotated == NULL) {
            fprintf(stderr, "%d\n", ENOMEM);
            exit(EXIT_FAILURE);
        }

        copy = *height;
        *height = *width;
        *width = copy;
    }
    free_img(&rotated, *width);
}

void fill(pixel ***img, int width, int height, int start_col, int start_line,
            pixel init, pixel change)
{
    (*img)[start_line][start_col] = change;

    if(start_line < height - 1) {
        if((*img)[start_line + 1][start_col].r == init.r &&
            (*img)[start_line + 1][start_col].g == init.g &&
            (*img)[start_line + 1][start_col].b == init.b) {
            fill(img, width, height, start_col, start_line + 1, init, change);
        }
    }

    if(start_line > 0) {
        if((*img)[start_line - 1][start_col].r == init.r &&
            (*img)[start_line - 1][start_col].g == init.g &&
            (*img)[start_line - 1][start_col].b == init.b) {

            fill(img, width, height, start_col, start_line - 1, init, change);
        }
    }

    if(start_col < width - 1) {
        if((*img)[start_line][start_col + 1].r == init.r &&
            (*img)[start_line][start_col + 1].g == init.g &&
            (*img)[start_line][start_col + 1].b == init.b) {

            fill(img, width, height, start_col + 1, start_line, init, change);
        }
    }

    if(start_col > 0) {
        if((*img)[start_line][start_col - 1].r == init.r &&
            (*img)[start_line][start_col - 1].g == init.g &&
            (*img)[start_line][start_col - 1].b == init.r) {

            fill(img, width, height, start_col - 1, start_line, init, change);
        }
    }
}
