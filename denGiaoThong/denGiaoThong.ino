#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
byte led7seg[]={0x3f,0x06,0x5B,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};

#define SCLK 2
#define RCLK 1
#define DIO 0
#define d1 8
#define v1 9
#define x1 10
#define d2 11
#define v2 12
#define x2 13

int t1, t2;
int t=0;
//Normal
int x = 27;
int d = 30;
int v = 3;
//Rush hour
int xx = 57;
int dd = 60;
int vv = 3;

bool shouldStop = false; // dừng hoạt động
volatile bool shouldExit = false; // Biến cờ ngắt nguồn
bool shouldBreak = false;// thoát khỏi vòng lặp
bool upButton = false;
bool downButton = false;
bool selectButton = false;
bool exitButton = false; // Thêm biến cho nút thoát
int selectedItem = 0;// lưu trữ vị trí của mục được chọn trong menu
bool inSubMenu = false; // xác định CT có trong màn hình con(submenu) không


void setup() {
lcd.init(); // khởi động lcd
lcd.backlight(); // bật đèn nền
pinMode(8,OUTPUT);
pinMode(9,OUTPUT);
pinMode(10,OUTPUT);
pinMode(11,OUTPUT);
pinMode(12,OUTPUT);
pinMode(13,OUTPUT);

pinMode(SCLK,OUTPUT);
pinMode(RCLK,OUTPUT);
pinMode(DIO,OUTPUT);
}

void loop() {
  // đọc chân kỹ thuật không được kích hoạt
  upButton = digitalRead(6) == LOW;
  downButton = digitalRead(5) == LOW;
  selectButton = digitalRead(7) == LOW;
  exitButton = digitalRead(4) == LOW;
  hienthi_Menu();
  // Cập nhật menu dựa trên trạng thái của nút bấm
  updateMenu();
  delay(10);
}

void updateMenu() {
  // Xử lý sự kiện nút bấm
  if (!inSubMenu) {
    if (upButton) {
      selectedItem = selectedItem - 1;
      if (selectedItem <= 0) selectedItem = 0;
    } else if (downButton) {
      selectedItem = selectedItem + 1;
      if (selectedItem >= 3) selectedItem = 3;
    } else if (selectButton) {
      // Hiển thị thông tin tương ứng với mục đã chọn
      if (selectedItem == 0) {
        hienthi_Member();
        inSubMenu = true;
      } else if (selectedItem == 1) {
        t = 0;
        Normal();
        inSubMenu = true;
      } else if (selectedItem == 2) {
        t = 0;
        Rush_hour();
        inSubMenu = true;
      } else if (selectedItem == 3) {
        t = 0;
        Warning();
        inSubMenu = true;
      }
    }
  } else {
    if (exitButton) {
      // Trở lại menu chính khi ấn nút EXIT trong màn hình con
      Off();
      lcd.clear();
      inSubMenu = false;
      exitButton = false;  // Đặt lại trạng thái nút thoát
      shouldBreak = false;
      shouldStop = false;
    } else if (selectedItem == 1) {
      Normal();
    } else if (selectedItem == 2) {
      Rush_hour();
    } else if (selectedItem == 3) {
      Warning();
    }
  }
}
void Off() {
  shouldStop = true; // ngừng hoạt động chương tình
  digitalWrite(x1, LOW);
  digitalWrite(v1, LOW);
  digitalWrite(d1, LOW);
  digitalWrite(x2, LOW);
  digitalWrite(v2, LOW);
  digitalWrite(d2, LOW);
  Off_led7doan();
}
void Off_led7doan() {
  // truyền dữ liệu từ vi đk sang led, truyền giá trị 0xFF (255 trong hệ thập phân) tới LED 7 đoạn. 
  //Điều này sẽ tắt tất cả các segment trên LED 7 đoạn, đảm bảo rằng không có số nào được hiển thị.
  shiftOut(DIO, SCLK, MSBFIRST, 0x00);
  shiftOut(DIO, SCLK, MSBFIRST, 0x00);
  shiftOut(DIO, SCLK, MSBFIRST, 0x00);
  shiftOut(DIO, SCLK, MSBFIRST, 0x00);
// RCLK (nắp đặt) của LED xuống mức thấp.
  digitalWrite(RCLK, LOW);
// RCLK (nắp đặt) của LED lên mức cao, kích hoạt việc hiển thị dữ liệu đã được truyền.
  digitalWrite(RCLK, HIGH);
}

void demsoled(int duong1, int duong2) {
// truyền giá trị của chữ số hàng đơn vị của duong2 vào LED.
  shiftOut(DIO, SCLK, MSBFIRST, led7seg[duong2 % 10]);
  shiftOut(DIO, SCLK, MSBFIRST, led7seg[duong2 / 10]);//hàng chục
//giá trị hàng đơn vị duong1
  shiftOut(DIO, SCLK, MSBFIRST, led7seg[duong1 % 10]);
  shiftOut(DIO, SCLK, MSBFIRST, led7seg[duong1 / 10]);//hàng chục
  digitalWrite(RCLK, LOW);//RCLK xuống thấp
  digitalWrite(RCLK, HIGH);//RCLK lên mức cáo để hiển thị dữ liệu truyền
}

void Warning() {
  if (t >= 2) {
    t = 0;
  }
  if (t >= 0 && t < 1) {
    digitalWrite(v1, HIGH);
    digitalWrite(v2, HIGH);
  } else {
    digitalWrite(x1, LOW);
    digitalWrite(v1, LOW);
    digitalWrite(d1, LOW);
    digitalWrite(x2, LOW);
    digitalWrite(v2, LOW);
    digitalWrite(d2, LOW);
  }
  
  t++;
}
void Normal() {
  if (t >= 0 && t < x) {
      digitalWrite(d1, LOW);  // đèn d1 tắt
      digitalWrite(x2, LOW);  // đèn x2 tắt
      digitalWrite(v1, LOW);  // đèn v1 tắt
      digitalWrite(v2, LOW);  // đèn v2 tắt
      digitalWrite(x1, HIGH);  // đèn x1 sáng
      digitalWrite(d2, HIGH);  // đèn d2 sáng
    }
    if (t > x && t <= (x + v)) {
      digitalWrite(x1, LOW);   // đèn x1 tắt
      digitalWrite(v2, LOW);   // đèn v2 tắt
      digitalWrite(d1, LOW);   // đèn d1 tắt
      digitalWrite(x2, LOW);   // đèn x2 tắt
      digitalWrite(v1, HIGH);  // đèn v1 bật
      digitalWrite(d2, HIGH);  // đèn d2 bật
    }
    if (t > (x + v) && t <= (x + d)) {
      digitalWrite(v1, LOW);  // đèn v1 tắt
      digitalWrite(d2, LOW);  // đèn d2 tắt
      digitalWrite(v2, LOW);  // đèn v2 tắt
      digitalWrite(x1, LOW);  // đèn x1 tắt
      digitalWrite(d1, HIGH);  // đèn d1 bật
      digitalWrite(x2, HIGH);  // đèn x2 bật
    }
    if (t > (x + d) && t <= (x + d + v)) {
      digitalWrite(x1, LOW);  // đèn x1 tắt
      digitalWrite(v1, LOW);  // đèn v1 tắt
      digitalWrite(d2, LOW);  // đèn d2 tắt
      digitalWrite(x2, LOW);  // đèn x2 tắt
      digitalWrite(d1, HIGH);  // đèn d1 bật
      digitalWrite(v2, HIGH);  // đèn v2 bật
    }
    if (t > (x + d + v)) {
      t = 0;
    }
    //hien thi dem lui LED 7 doan
    if (t == 0) {
      t1 = x;
      t2 = d;
    }
    if (t == x) {
      t1 = v;
      t2 = d - x;
    }
    if (t == d) {
      t1 = d;
      t2 = x;
    }
    if (t == d + x - v) {
      t1 = d - x;
      t2 = v;
    }
    demsoled(t1, t2);
    delay(1000);
    t1--;
    t2--;
    t++;
}

void Rush_hour() {
  if (t >= 0 && t < xx) {
      digitalWrite(d1, LOW);  // đèn d1 tắt
      digitalWrite(x2, LOW);  // đèn x2 tắt
      digitalWrite(v1, LOW);  // đèn v1 tắt
      digitalWrite(v2, LOW);  // đèn v2 tắt

      digitalWrite(x1, HIGH);  // đèn x1 sáng
      digitalWrite(d2, HIGH);  // đèn d2 sáng
    }
    if (t > xx && t <= (xx + vv)) {
      digitalWrite(x1, LOW);   // đèn x1 tắt
      digitalWrite(v2, LOW);   // đèn v2 tắt
      digitalWrite(d1, LOW);   // đèn d1 tắt
      digitalWrite(x2, LOW);   // đèn x2 tắt
      digitalWrite(v1, HIGH);  // đèn v1 bật
      digitalWrite(d2, HIGH);  // đèn d2 bật
    }
    if (t > (xx + vv) && t <= (xx + dd)) {
      digitalWrite(v1, LOW);  // đèn v1 tắt
      digitalWrite(d2, LOW);  // đèn d2 tắt
      digitalWrite(v2, LOW);  // đèn v2 tắt
      digitalWrite(x1, LOW);  // đèn x1 tắt

      digitalWrite(d1, HIGH);  // đèn d1 bật
      digitalWrite(x2, HIGH);  // đèn x2 bật
    }

    if (t > (xx + dd) && t <= (xx + dd + vv)) {
      digitalWrite(x1, LOW);  // đèn x1 tắt
      digitalWrite(v1, LOW);  // đèn v1 tắt
      digitalWrite(d2, LOW);  // đèn d2 tắt
      digitalWrite(x2, LOW);  // đèn x2 tắt

      digitalWrite(d1, HIGH);  // đèn d1 bật
      digitalWrite(v2, HIGH);  // đèn v2 bật
    }
    if (t > (xx + dd + vv)) {
      t = 0;
    }
    //hien thi dem lui LED 7 doan
    if (t == 0) {
      t1 = xx;
      t2 = dd;
    }
    if (t == xx) {
      t1 = vv;
      t2 = dd - xx;
    }
    if (t == dd) {
      t1 = dd;
      t2 = xx;
    }
    if (t == dd + xx - vv) {
      t1 = dd - xx;
      t2 = vv;
    }
    demsoled(t1, t2);
    delay(1000);
    t1--;
    t2--;
    t++;
}
void hienthi_Member() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("1. Huyen");
    lcd.setCursor(0, 1);
    lcd.print("2. Thuong");
    lcd.setCursor(0, 2);
    lcd.print("3. Quan");
    lcd.setCursor(0, 3);
    lcd.print("3. Minh");
}
void hienthi_Menu() {
  if (!inSubMenu) {
    // Hiển thị menu ở dòng thứ hai
    lcd.setCursor(0, 0);
    lcd.print("1.Thanh vien nhom");
    lcd.setCursor(0, 1);
    lcd.print("2.Normal");
    lcd.setCursor(0, 2);
    lcd.print("3.Rush hour");
    lcd.setCursor(0, 3);
    lcd.print("4.Warning");
    lcd.setCursor(0,0 + selectedItem);
    lcd.print(">"); // Hiển thị con trỏ ở dòng đã chọn.......
    delay(100);
  }
}

