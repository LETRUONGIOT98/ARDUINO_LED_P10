#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include "SystemFont5x7.h"  // Font nhỏ, mỗi ký tự có độ rộng là 5 pixel

#define DISPLAYS_ACROSS 1 //-> Number of P10 panels used, side to side.
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);  // Khai báo duy nhất

char DisplayText[6];  // Chuỗi hiển thị 2 số nguyên và 2 số thập phân
#define nutnhan 2
int old = 1;
int state = 0; // Trạng thái: 0 = đếm, 1 = dừng, 2 = hiển thị 00.00
float thoigian = 0;
unsigned long times = 0;

// Cấu hình hiển thị
const int LED_WIDTH = 32;   // Chiều ngang của bảng LED
const int LED_HEIGHT = 16;  // Chiều cao của bảng LED
const int CHAR_WIDTH = 5;   // Độ rộng mỗi ký tự (font SystemFont5x7)
const int DOT_WIDTH = 2;    // Độ rộng của dấu chấm (2x2 pixel)

void ScanDMD() { 
  dmd.scanDisplayBySPI();
}

void setup(void) {
  Timer1.initialize(200);          
  Timer1.attachInterrupt(ScanDMD);   
  dmd.clearScreen(false);   // Nền tối
  Serial.begin(115200);
  
  pinMode(nutnhan, INPUT_PULLUP);

  // Sử dụng font SystemFont5x7, mỗi ký tự có độ rộng 5 pixel
  dmd.selectFont(SystemFont5x7);  
}

void loop(void) {
  int val = digitalRead(nutnhan);  // Đọc trạng thái nút nhấn
  if (val != old) {
    if (val == 0) {  // Khi nút được nhấn
      if (state == 0) { 
        state = 1;  // Đổi sang trạng thái dừng
      } else if (state == 1) {
        state = 2;  // Hiển thị 00.00
        
      } else if (state == 2) {
        state = 0;  // Bắt đầu đếm lại
        thoigian = 0;  // Đặt lại thời gian về 0
        times = millis();  // Lưu thời điểm bắt đầu
      }
    }
  }
  old = val;  // Cập nhật trạng thái nút nhấn

  // Xử lý các trạng thái
  if (state == 0) { 
    thoigian = (millis() - times) / 1000.0;  // Tính thời gian dạng float (giây)
  } else if (state == 2) {
    thoigian = 0.0;  // Đặt giá trị hiển thị là 00.00
    delay(100);
  }

  // Chuyển đổi giá trị thời gian sang chuỗi hiển thị
  snprintf(DisplayText, sizeof(DisplayText), "%02d.%02d", (int)thoigian, (int)((thoigian - (int)thoigian) * 100));

  // Xóa màn hình trước khi vẽ
  dmd.clearScreen(false);   // Nền tối

  // Tính toán vị trí hiển thị
  const int startX = (LED_WIDTH - (strlen(DisplayText) * (CHAR_WIDTH + 1))) / 2;  // Căn giữa theo chiều ngang
  const int startY = (LED_HEIGHT - 7) / 2;                                       // Căn giữa theo chiều dọc

  // Hiển thị từng ký tự
  for (int i = 0; i < strlen(DisplayText); i++) {
    dmd.drawChar(startX + i * (CHAR_WIDTH + 1), startY, DisplayText[i], GRAPHICS_TOGGLE);
  }
}
