#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "esp_camera.h"
#include "FS.h"
#include "SD.h"
#include <TJpg_Decoder.h>

// ================= TFT =================
#define TFT_CS D7
#define TFT_DC D3
#define TFT_RST D4

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// ================= BUTTON =================
#define BTN_SHOOT D2
#define BTN_NEXT D1
#define BTN_PREV D5

// ================= SD =================
#define SD_CS 21

// ================= CAMERA =================
#define CAMERA_MODEL_XIAO_ESP32S3
#include "camera_pins.h"

// ================= STATE =================
bool modePreview = true;
int imageCount = 1;
int currentImage = 1;
int lastImage = -1;

// ================= BUFFER =================
static uint16_t temp[240 * 240];

// ================= SWAP COLOR (HANYA UNTUK PREVIEW) =================
void swap565(uint16_t *img, int len) {
  for (int i = 0; i < len; i++) {
    uint16_t c = img[i];
    img[i] = (c >> 8) | (c << 8);
  }
}

// ================= WRITE FILE =================
void writeFile(const char *path, uint8_t *data, size_t len) {
  File file = SD.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Gagal save");
    return;
  }
  file.write(data, len);
  file.close();
}

// ================= SHOW IMAGE =================
void showImage(const char *path) {
  tft.fillScreen(ST77XX_BLACK);
  TJpgDec.drawSdJpg(0, 0, path);
}

// ================= JPEG CALLBACK (JANGAN SWAP DI SINI) =================
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  tft.drawRGBBitmap(x, y, bitmap, w, h);
  return 1;
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(BTN_SHOOT, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_PREV, INPUT_PULLUP);

  // ==== SPI ====
  SPI.begin(7, 8, 9, 21);
  SPI.setFrequency(27000000);  // atau turunin jadi 16MHz

  // ==== TFT ====
  tft.init(240, 240);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  // ==== JPEG ====
  TJpgDec.setCallback(tft_output);

  // ==== SD ====
  if (!SD.begin(SD_CS)) {
    Serial.println("SD FAIL");
    while (1)
      ;
  }
  Serial.println("SD OK");

  // ==== CAMERA ====
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_RGB565;
  config.frame_size = FRAMESIZE_240X240;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.fb_count = 2;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera gagal");
    while (1)
      ;
  }
sensor_t *s = esp_camera_sensor_get();

// Mirror horizontal
s->set_hmirror(s, 1);

// (opsional) flip vertikal kalau perlu
// s->set_vflip(s, 1);
  Serial.println("Camera OK");
}

// ================= LOOP =================
void loop() {

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) return;

  if (modePreview) {

    // ===== PREVIEW (PAKAI COPY BUFFER) =====
    memcpy(temp, fb->buf, 240 * 240 * 2);
    swap565(temp, 240 * 240);

    tft.drawRGBBitmap(0, 0, temp, 240, 240);

    // ===== SHOOT =====
    if (digitalRead(BTN_SHOOT) == LOW) {
      delay(50);

      char filename[32];
      sprintf(filename, "/img%d.jpg", imageCount);

      size_t len;
      uint8_t *buf;

      if (frame2jpg(fb, 12, &buf, &len)) {
        writeFile(filename, buf, len);
        Serial.println("Foto disimpan");
        imageCount++;
        free(buf);
      }

      delay(500);
    }

    // ===== MASUK GALLERY =====
    if (digitalRead(BTN_NEXT) == LOW) {
      modePreview = false;
      currentImage = imageCount - 1;
      delay(300);
    }
  }

  else {

    if (currentImage != lastImage) {

      char filename[32];
      sprintf(filename, "/img%d.jpg", currentImage);

      tft.fillScreen(ST77XX_BLACK);
      TJpgDec.drawSdJpg(0, 0, filename);

      lastImage = currentImage;
    }

    if (digitalRead(BTN_NEXT) == LOW) {
      currentImage++;
      if (currentImage >= imageCount) currentImage = imageCount - 1;
      delay(300);
    }

    if (digitalRead(BTN_PREV) == LOW) {
      currentImage--;
      if (currentImage < 1) currentImage = 1;
      delay(300);
    }

    if (digitalRead(BTN_SHOOT) == LOW) {
      modePreview = true;
      lastImage = -1;  // reset biar render ulang nanti
      delay(300);
    }
  }

  esp_camera_fb_return(fb);
}