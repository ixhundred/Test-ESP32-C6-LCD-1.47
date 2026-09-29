#include <Arduino.h>
#include <esp_system.h>

void setup() {
    Serial.begin(115200);
    while(!Serial); // รอให้ Serial Monitor พร้อม (สำหรับ ESP32-S3)
    delay(1000);
    
    Serial.println("\n--- ESP32-S3 System Memory Information ---");
    // ตรวจสอบว่าบอร์ดมี PSRAM ภายนอกหรือไม่
    if (psramFound()) {
        Serial.println("✅ PSRAM detected");
        uint32_t psramSize = ESP.getPsramSize();
        Serial.printf("PSRAM size: %u bytes (%.2f KB)\n", psramSize, psramSize / 1024.0);
    } else {
        Serial.println("❌ No external PSRAM found");
    }
}

void loop() {
    // แก้ไขการเรียกใช้ฟังก์ชันของ ESP class ให้ถูกต้อง (CamelCase)
    uint32_t free_heap = ESP.getFreeHeap();
    // ใช้ฟังก์ชันที่มีใน Arduino core เพื่อรับขนาดบล็อกที่สามารถจองได้มากที่สุด
    uint32_t max_block = ESP.getMaxAllocHeap();
    
    // การคำนวณหน่วยเป็น KB
    float free_heap_kb = (float)free_heap / 1024.0;
    float max_block_kb = (float)max_block / 1024.0;

    Serial.printf("Free Heap:     %u bytes (%.2f KB)\n", free_heap, free_heap_kb);
    Serial.printf("Max Block:     %u bytes (%.2f KB)\n", max_block, max_block_kb);
    
    // การดึงขนาด Flash
    // ใช้ฟังก์ชันจาก Arduino core เพื่อดึงขนาด Flash ของบอร์ด
    uint32_t flash_size = ESP.getFlashChipSize();
    Serial.printf("Total Flash:   %u bytes (%.2f KB)\n", flash_size, (float)flash_size / 1024.0);


    Serial.printf("Total PSRAM: %d bytes\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());

    Serial.println("-------------------------------------------");
    
    delay(5000);
}