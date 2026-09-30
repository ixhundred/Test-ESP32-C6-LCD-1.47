#include <Arduino.h>
#include <esp_system.h>
#include <SPI.h>  // Include the SPI library for communication with the display 

#define MOSI	6
#define SCLK	7
#define LCD_CS	14
#define LCD_DC	15
#define LCD_RST	21
#define LCD_BL	22

#define RGB_Control	8

#define MISO	5
//#define MOSI	6
//#define SCLK	7
#define CS	    4
#define SD_D1	-1
#define SD_D2	-1

int i = 0;

#include "image_data.h"
#include <Arduino_GFX_Library.h>
#define GFX_BL LCD_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin
Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, SCLK, MOSI, NOT_A_PIN);
Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST, 0, true, 172, 320, 34, 0, 34, 0);

#include <JPEGDEC.h>
JPEGDEC jpeg;

// ฟังก์ชัน Callback สำหรับส่งข้อมูลพิกเซลที่ถอดรหัสแล้วไปวาดลงจอ
int JPEGDraw(JPEGDRAW *pDraw) {
  gfx->draw16bitRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

#include <Adafruit_NeoPixel.h>
#define LED_COUNT 1
Adafruit_NeoPixel strip(LED_COUNT, RGB_Control, NEO_GRB + NEO_KHZ800);

void init_display() 
{
    gfx->fillScreen(RGB565_BLACK);    
    gfx->setCursor(0, 0); // Set the cursor position before printing text
    gfx->setTextColor(RGB565_WHITE); // Set the text color before printing text
    gfx->setTextSize(2); // Set the text size before printing text
    gfx->println("#Hello, ESP32-C6!");
    gfx->println("#I'm Yada");    
    gfx->println();    
    gfx->setTextColor(RGB565_YELLOW);
    gfx->setTextSize(1); // Set the text size before printing text
    gfx->println("#Hello, ESP32-C6!");
    gfx->println("#I'm Yada");
    gfx->println();
    delay(2000); // เพิ่มดีเลย์ 2 วินาทีเพื่อให้ข้อความแสดงผลบนจอได้ชัดเจน
}

void setup() {
    Serial.begin(115200);
    //while(!Serial); // รอให้ Serial Monitor พร้อม (สำหรับ ESP32-S3)
    //delay(1000);
    
    Serial.println("\n--- ESP32-S3 System Memory Information ---");
    // ตรวจสอบว่าบอร์ดมี PSRAM ภายนอกหรือไม่
    if (psramFound()) {
        Serial.println("✅ PSRAM detected");
        uint32_t psramSize = ESP.getPsramSize();
        Serial.printf("PSRAM size: %u bytes (%.2f KB)\n", psramSize, psramSize / 1024.0);
    } else {
        Serial.println("❌ No external PSRAM found");
    }

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

    if (!gfx->begin())
    {
        Serial.println("gfx->begin() failed!");
    }
    else {
        Serial.println("gfx->begin() succeeded!");
        pinMode(GFX_BL, OUTPUT);
        digitalWrite(GFX_BL, HIGH);
    }
    init_display();
    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(100); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop() {
    gfx->setCursor(random(gfx->width()), random(gfx->height()));
    gfx->setTextColor(random(0xffff), random(0xffff));
    gfx->setTextSize(random(2) + 1 /* x scale */, random(2) + 1 /* y scale */, random(2) /* pixel_margin */);
    gfx->println("I'm Yada");
    Serial.println("#tick");

    ++i;

    gfx->setCursor(86-12,160-12);
    gfx->setTextSize(3);
    gfx->setTextColor(RGB565_YELLOW,RGB565_BLACK);
    if (i < 10)
        gfx->printf("%d", i);

    strip.setPixelColor(0, random(0xffffff)); //  Set pixel's color (in RAM)
    strip.show();  

    if (i % 10 == 0) {
        if (jpeg.openRAM((uint8_t *)my_jpeg_image, my_jpeg_image_len, JPEGDraw)) {
            Serial.println("Successfully opened JPEG from memory");
            
            // ตั้งค่าขนาดการแสดงผล (สามารถย่อขนาดลงมาได้ เช่น Scale 0 = ขนาดจริง)
            jpeg.setPixelType(RGB565_LITTLE_ENDIAN); // หรือ LITTLE_ENDIAN ตามสถาปัตยกรรมจอ
            
            // สั่งให้ทำการถอดรหัสและวาดภาพลงพิกัด X=0, Y=0 ของจอ
            jpeg.decode(0, 0, 0);
        } else {
            Serial.println("Failed to decode JPEG");
        }    
        delay(4000);
    }

    delay(1000); // 1 second 
    if (i % 10 == 0) {
        init_display();
        i = 0;
    }    
}