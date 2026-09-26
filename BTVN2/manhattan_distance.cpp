#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_IMAGES 10
#define VECTOR_DIM 64

// Hàm tính khoảng cách Manhattan giữa 2 vector 64 chiều
float manhattan_distance(const float v1[VECTOR_DIM], const float v2[VECTOR_DIM]) {
    float dist = 0.0f;
    for (int i = 0; i < VECTOR_DIM; i++) {
        dist += fabsf(v1[i] - v2[i]); // Tổng trị tuyệt đối hiệu từng chiều
    }
    return dist;
}

int main() {
    float vectors[NUM_IMAGES][VECTOR_DIM];

    // 1. Đọc dữ liệu 10 vector 64D từ file aped_feature_vectors.txt
    FILE *f_in = fopen("aped_feature_vectors.txt", "r");

    for (int k = 0; k < NUM_IMAGES; k++) {
        char dummy[32];
        // Đọc và bỏ qua nhãn "Anh_X:" ở đầu mỗi dòng
        if (fscanf(f_in, "%s", dummy) != 1) break;
        
        // Đọc 64 giá trị float của từng vector
        for (int j = 0; j < VECTOR_DIM; j++) {
            fscanf(f_in, "%f", &vectors[k][j]);
        }
    }
    fclose(f_in);

    // 2. Nhập vị trí ảnh bạn muốn dùng làm Query (từ 1 đến 10)
    int query_id = 5; //Vd: 5

    int query_idx = query_id - 1; // Chuyển về chỉ số mảng (0-9)
    float *query_vector = vectors[query_idx];

    printf("\n=== TINH 10 KHOANG CACH MANHATTAN CHO ANH %d ===\n", query_id);

    float distances[NUM_IMAGES];
    float min_dist = 1e9f; // Khởi tạo min ban đầu rất lớn
    int min_idx = -1;

    // 3. Tính khoảng cách Manhattan tới cả 10 ảnh trong CSDL và tìm Min
    for (int k = 0; k < NUM_IMAGES; k++) {
        distances[k] = manhattan_distance(query_vector, vectors[k]);
        
        printf("Khoang cach toi Anh_%-2d : %.6f", k + 1, distances[k]);
        if (k == query_idx) {
            printf("  <-- ");
        }
        printf("\n");

        // Cập nhật giá trị Min
        if (distances[k] < min_dist) {
            min_dist = distances[k];
            min_idx = k;
        }
    }

    // 4. In kết quả Min tìm được
    printf("--------------------------------------------------\n");
    printf("KHOANG CACH MIN TRONG 10 ANH: %.6f\n", min_dist);
    printf("--> ANH CAN TIM: Anh_%d\n", min_idx + 1);

    return 0;
}