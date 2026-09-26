#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_IMAGES 10
#define VECTOR_DIM 64 // Vector 64 chiều (16 ô x 4 hướng)

// Hàm hỗ trợ sắp xếp mảng để tìm giá trị Trung vị (Median)
int compare(const void *a, const void *b) {
    float diff = *(float*)a - *(float*)b;
    return (diff > 0) - (diff < 0);
}

// Thuật toán PPED lọc biên đồng thời trích xuất Vector đặc trưng APED 64D
void pped_and_extract_aped_vector(const unsigned char *I, unsigned char *dst, int w, int h, float vector[VECTOR_DIM]) {
    // 1. Khởi tạo ảnh đầu ra màu đen và xóa trắng vector 64 chiều
    for (int i = 0; i < w * h; i++) dst[i] = 0;
    for (int i = 0; i < VECTOR_DIM; i++) vector[i] = 0.0f;

    // Các mảng đếm điểm biên cho 4 hướng {H, P, V, M} tương ứng 16 ô
    float H_vec[16] = {0.0f}, P_vec[16] = {0.0f}, V_vec[16] = {0.0f}, M_vec[16] = {0.0f};

    // Tính kích thước ô lưới 4x4 linh hoạt theo W và H của ảnh gốc
    float cell_w = (float)w / 4.0f;
    float cell_h = (float)h / 4.0f;

    // 2. Lặp qua từng pixel của ảnh gốc (tránh viền 2 pixel)
    for (int y = 2; y < h - 2; y++) {
        for (int x = 2; x < w - 2; x++) {
            
            float diff_vals[40];
            int count = 0;

            // --- Công thức (4): Chênh lệch hướng Ngang H_nm ---
            for (int n = -2; n <= 1; n++) {
                for (int m = -2; m <= 2; m++) {
                    float val1 = I[(y + m) * w + (x + n + 1)];
                    float val2 = I[(y + m) * w + (x + n)];
                    diff_vals[count++] = fabsf(val1 - val2);
                }
            }

            // --- Công thức (5): Chênh lệch hướng Dọc V_nm ---
            for (int n = -2; n <= 1; n++) {
                for (int m = -2; m <= 2; m++) {
                    float val1 = I[(y + n + 1) * w + (x + m)];
                    float val2 = I[(y + n) * w + (x + m)];
                    diff_vals[count++] = fabsf(val1 - val2);
                }
            }

            // --- Tìm Trung vị Med(x,y) từ 40 giá trị ---
            qsort(diff_vals, 40, sizeof(float), compare);
            float Med = (diff_vals[19] + diff_vals[20]) / 2.0f;

            // --- Công thức (6): Tính Ngưỡng TH(x, y) ---
            float TH = Med * 5.0f;

            // --- TÍNH ĐỘ CHÊNH LỆCH THEO 4 HƯỚNG {H, P(45°), V, M(-45°)} ---
            float I_H = fabsf((float)I[y * w + (x + 1)] - (float)I[y * w + (x - 1)]);
            float I_P = fabsf((float)I[(y - 1) * w + (x + 1)] - (float)I[(y + 1) * w + (x - 1)]);
            float I_V = fabsf((float)I[(y + 1) * w + x] - (float)I[(y - 1) * w + x]);
            float I_M = fabsf((float)I[(y + 1) * w + (x + 1)] - (float)I[(y - 1) * w + (x - 1)]);

            // Tìm giá trị chênh lệch lớn nhất giữa 4 hướng và hướng tương ứng
            float I_max = I_H;
            int max_dir = 0; // 0: H, 1: P, 2: V, 3: M

            if (I_P > I_max) { I_max = I_P; max_dir = 1; }
            if (I_V > I_max) { I_max = I_V; max_dir = 2; }
            if (I_M > I_max) { I_max = I_M; max_dir = 3; }

            // --- Công thức (7): Phân ngưỡng xác định điểm biên ---
            if (I_max > TH && I_max > 20.0f) {
                dst[y * w + x] = 255; // Đánh dấu điểm biên lên ảnh đầu ra

                // --- TÍCH LŨY BIÊN VÀO CÁC Ô LƯỚI (Lưới 4x4 -> 16 Ô) ---
                int a = (int)(x / cell_w); // Chỉ số cột ô (0, 1, 2, 3)
                int b = (int)(y / cell_h); // Chỉ số hàng ô (0, 1, 2, 3)

                // Giới hạn an toàn tránh viền ảnh
                if (a > 3) a = 3;
                if (b > 3) b = 3;

                int idx = a + 4 * b; // Vị trí ô từ 0 đến 15

                // Tăng số lượng đếm điểm biên cho hướng tương ứng tại ô `idx`
                if (max_dir == 0)      H_vec[idx] += 1.0f;
                else if (max_dir == 1) P_vec[idx] += 1.0f;
                else if (max_dir == 2) V_vec[idx] += 1.0f;
                else if (max_dir == 3) M_vec[idx] += 1.0f;
            } else {
                dst[y * w + x] = 0;   // Không phải biên
            }
        }
    }

    // --- GHÉP 4 VECTOR THÀNH VECTOR 64 CHIỀU & CHUẨN HÓA MẬT ĐỘ ---
    float total_pixels = (float)(w * h);
    for (int i = 0; i < 16; i++) {
        vector[0 + i]  = H_vec[i] / total_pixels; // 16 chiều Ngang H (0-15)
        vector[16 + i] = P_vec[i] / total_pixels; // 16 chiều Chéo P (16-31)
        vector[32 + i] = V_vec[i] / total_pixels; // 16 chiều Dọc V (32-47)
        vector[48 + i] = M_vec[i] / total_pixels; // 16 chiều Chéo M (48-63)
    }
}

int main() {
    int widths[NUM_IMAGES], heights[NUM_IMAGES];

    // Đọc kích thước của 10 ảnh từ config.txt
    FILE *f_cfg = fopen("config.txt", "r");
    if (!f_cfg) {
        printf("Loi: Khong mo duoc file config.txt!\n");
        return -1;
    }
    
    for (int k = 0; k < NUM_IMAGES; k++) {
        fscanf(f_cfg, "%d %d", &widths[k], &heights[k]);
    }
    fclose(f_cfg);

    // Mảng 2D chứa 10 Vector 64 chiều cho 10 bức ảnh
    float feature_vectors[NUM_IMAGES][VECTOR_DIM];
    char input_filename[64], output_filename[64];

    for (int k = 1; k <= NUM_IMAGES; k++) {
        int width = widths[k - 1];
        int height = heights[k - 1];
        size_t num_pixels = (size_t)width * height;

        // Đường dẫn file ảnh xám đầu vào và file ảnh biên đầu ra
        sprintf(input_filename, "grey_images/grey%d.raw", k);
        sprintf(output_filename, "pped_images/pped%d.raw", k);

        FILE *f_in = fopen(input_filename, "rb");
        if (!f_in) {
            printf("Khong mo duoc file: %s\n", input_filename);
            continue;
        }

        unsigned char *gray_buf = (unsigned char *)malloc(num_pixels);
        unsigned char *pped_buf = (unsigned char *)malloc(num_pixels);

        fread(gray_buf, 1, num_pixels, f_in);
        fclose(f_in);

        // Lọc biên PPED + Trích xuất Vector APED 64D đồng thời
        pped_and_extract_aped_vector(gray_buf, pped_buf, width, height, feature_vectors[k - 1]);

        // Ghi file ảnh biên PPED ra đĩa
        FILE *f_out = fopen(output_filename, "wb");
        if (f_out) {
            fwrite(pped_buf, 1, num_pixels, f_out);
            fclose(f_out);
            printf("Xu ly thanh cong: %s -> %s (%dx%d)\n", input_filename, output_filename, width, height);
        }

        free(gray_buf);
        free(pped_buf);
    }

    // --- GHI KẾT QUẢ 10 VECTOR ĐẶC TRƯNG 64 CHIỀU RA FILE RIÊNG ---
    FILE *f_vec = fopen("aped_feature_vectors.txt", "w");
    if (f_vec) {
        for (int k = 0; k < NUM_IMAGES; k++) {
            fprintf(f_vec, "Anh_%d: ", k + 1);
            for (int j = 0; j < VECTOR_DIM; j++) {
                fprintf(f_vec, "%.6f ", feature_vectors[k][j]);
            }
            fprintf(f_vec, "\n");
        }
        fclose(f_vec);
        printf("\nHoan thanh! Da trich xuat & luu 10 Vector 64D vao file 'aped_feature_vectors.txt'\n");
    } else {
        printf("Loi: Khong the tao file aped_feature_vectors.txt de ghi vector!\n");
    }

    return 0;
}