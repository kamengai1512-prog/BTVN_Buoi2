import os
import numpy as np
from PIL import Image

# 1. Tên thư mục đầu vào và đầu ra
input_folder = "images"       # Thư mục chứa 10 ảnh JPG ban đầu
output_folder = "raw_images"  # Thư mục chứa 10 file .raw đầu ra

# 2. Mở file config.txt để ghi kích thước [width height] của 10 ảnh
with open("config.txt", "w") as f_cfg:
    for k in range(1, 11):
        # Đường dẫn ảnh đầu vào: images/face1.jpg, ...
        input_jpg = os.path.join(input_folder, f"face{k}.jpg")
        
        # Đường dẫn file đầu ra: raw_images/face1.raw, ...
        output_raw = os.path.join(output_folder, f"face{k}.raw")

        # Đọc ảnh JPG
        img_jpg = Image.open(input_jpg).convert("RGB")
        img_array = np.array(img_jpg, dtype=np.uint8)

        # Lấy kích thước ảnh
        height, width, _ = img_array.shape

        # Ghi kích thước riêng của từng ảnh vào config.txt
        f_cfg.write(f"{width} {height}\n")

        # Xuất mảng dữ liệu ra file .raw trong thư mục raw_images
        img_array.tofile(output_raw)
        print(f"Da xuat: {input_jpg} -> {output_raw} ({width}x{height})")

print("\nHoan thanh!/'.")
