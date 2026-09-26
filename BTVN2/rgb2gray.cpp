#include <stdio.h>
#include <stdlib.h>

int main() {
    int widths[10];
    int heights[10];

    // 1. Đọc kích thước của 10 ảnh từ file config.txt
    FILE *f_cfg = fopen("config.txt", "r");

    for (int k = 0; k < 10; k++) {
        if (fscanf(f_cfg, "%d %d", &widths[k], &heights[k]) != 2) {
            printf("Loi: Thieu du lieu kich thuoc o dong %d trong config.txt\n", k + 1);
            fclose(f_cfg);
            return -1;
        }
    }
    fclose(f_cfg);
    printf("Da doc xong config.txt.\n\n");

    char input_filename[64];
    char output_filename[64];

    // 2. Vòng lặp chuyển đổi 10 ảnh RGB sang Grayscale
    for (int k = 1; k <= 10; k++) {
        int width = widths[k - 1];
        int height = heights[k - 1];
        size_t num_pixels = (size_t)width * height;

        // Đường dẫn file
        sprintf(input_filename, "raw_images/face%d.raw", k);
        sprintf(output_filename, "grey_images/grey%d.raw", k);

        // Mở file RAW đầu vào
        FILE *f_in = fopen(input_filename, "rb");
        if (!f_in) {
            printf("Khong tim thay file: %s\n", input_filename);
            continue;
        }

        // Cấp phát bộ nhớ theo đúng kích thước từng ảnh
        unsigned char *rgb_buf = (unsigned char *)malloc(num_pixels * 3);
        unsigned char *grey_buf = (unsigned char *)malloc(num_pixels);

        // Đọc dữ liệu RGB từ file .raw
        fread(rgb_buf, 1, num_pixels * 3, f_in);
        fclose(f_in);

        // Chuyển đổi RGB sang Grayscale (8-bit) bằng công thức Luminance
        for (size_t i = 0; i < num_pixels; i++) {
            unsigned char r = rgb_buf[i * 3 + 0];
            unsigned char g = rgb_buf[i * 3 + 1];
            unsigned char b = rgb_buf[i * 3 + 2];

            grey_buf[i] = (unsigned char)(0.299f * r + 0.587f * g + 0.114f * b);
        }

        // Ghi dữ liệu xám ra file grey.raw
        FILE *f_out = fopen(output_filename, "wb");
        if (f_out) {
            fwrite(grey_buf, 1, num_pixels, f_out);
            fclose(f_out);
            printf("Thanh cong: %s -> %s (%dx%d)\n", input_filename, output_filename, width, height);
        } else {
            printf("Loi khi ghi file: %s\n", output_filename);
        }

        // Giải phóng bộ nhớ của ảnh hiện tại
        free(rgb_buf);
        free(grey_buf);
    }

    printf("\nHoan thanh chuyen doi 10 anh sang grayscale!\n");
    return 0;
}