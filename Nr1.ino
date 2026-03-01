#include <Keyboard.h>

// ==========================================
// 模式設定：正式掛機前請確保下面這一行是被註解掉的 (前面加 //)
// ==========================================
//#define DEBUG_MODE 

#ifdef DEBUG_MODE
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_BEGIN(x) Serial.begin(x)
#else
  #define DEBUG_PRINT(x)   
  #define DEBUG_PRINTLN(x) 
  #define DEBUG_BEGIN(x)   
#endif

// ==========================================
// 全域變數設定
// ==========================================
bool running = false;
bool finished = false;
unsigned long startTime = 0;

const float runTimeMinutes = 30;                             // 設定執行分鐘數
unsigned long maxRunTime = runTimeMinutes * 60UL * 1000UL;    // 轉換為毫秒
const unsigned long startDelay = 5000UL;                     // 上電後等待 10 秒才開始

void setup() {
  DEBUG_BEGIN(9600);
  Keyboard.begin();
  pinMode(13, OUTPUT); // 初始化板載 LED
  randomSeed(analogRead(0));

  DEBUG_PRINTLN("系統初始化... 10 秒後自動開始執行");
  delay(startDelay);

  running = true;
  startTime = millis();
  DEBUG_PRINTLN("==== 腳本開始執行 ====");
}

void loop() {
  #ifdef DEBUG_MODE
  if (running && Serial.available()) {
    char c = Serial.read();
    if (c == 'Q' || c == 'q') {
      stopScript("收到手動停止指令");
      return;
    }
  }
  #endif

  if (running && !finished) {
    if (millis() - startTime >= maxRunTime) {
      stopScript("設定時間已到，自動停止");
      return;
    }
    attack();
  }
}

// ==========================================
// 停止與通知邏輯
// ==========================================
void stopScript(const char* reason) {
  running = false;
  finished = true;
  Keyboard.releaseAll();
  DEBUG_PRINTLN(reason);

  //  結束後按一下= 可設定為回家卷軸
  delay(500);
  Keyboard.press('=');
  delay(random(100, 200));
  Keyboard.release('=');
  delay(200); // 稍微停頓
  
  // 按下 ENTER
  Keyboard.press(KEY_RETURN);
  delay(random(100, 200));
  Keyboard.release(KEY_RETURN);

  // 1. 硬體通知：讓板載 LED 快速閃爍
  for(int i = 0; i < 30; i++) {
    digitalWrite(13, HIGH);
    delay(50);
    digitalWrite(13, LOW);
    delay(50);
  }

  // 2. 軟體視覺通知：打開 Win+R 並打字
  sendWinRNotice();
  

  
}

void sendWinRNotice() {
  Keyboard.releaseAll();
  delay(500); 

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(random(150, 250)); 
  Keyboard.releaseAll();
  
  delay(1500); 

  // 清除輸入框
  for(int i=0; i<25; i++) {
    Keyboard.write(KEY_BACKSPACE);
  }
  delay(200);

  // 打字提醒
  Keyboard.print("1");
}

// ==========================================
// 按鍵動作邏輯
// ==========================================
void attack() {
  // 1. 雙面神 (右)
  pressKey('v', "V 鍵動作", 500, 1000); 
  // --- 強制延遲：V 放開後，死等 0.8~1.2 秒才准按下一個鍵 ---
  smartDelay(random(500, 1000)); 
  pressKey(KEY_UP_ARROW, "上箭頭", 120, 300); 

  // 2. 雙面神 (左)
  pressKey('v', "V 鍵動作", 500, 1000);
  smartDelay(random(350, 800)); 

  pressKey('A', "快速A", 50, 100); 
  smartDelay(random(3000, 4000));
  
  pressKey(KEY_UP_ARROW, "上箭頭", 120, 300);

  // 3. 雙面神 (中)
  pressKey('v', "V 鍵動作", 500, 1000);
  smartDelay(random(3000, 4000)); // 最後一發稍微停久一點

  // 隨機機率點按 ALT
  if (random(0, 2) == 1) { 
    Keyboard.press(KEY_LEFT_ALT);
    delay(random(100, 250)); 
    Keyboard.release(KEY_LEFT_ALT);
    smartDelay(random(400, 600)); 
  }

  pressKey(KEY_UP_ARROW, "上箭頭", 120, 300);

  // 進入 85~105 秒的等待期，期間隨機按 S
  unsigned long waitTime = random(75000, 105001);
  holdKey(waitTime);
}

void pressKey(uint8_t key, const char* name, int minDelay, int maxDelay) {
  smartDelay(random(minDelay, maxDelay));
  Keyboard.press(key);
  smartDelay(random(80, 180));
  Keyboard.release(key);
}

void holdKey(unsigned long duration) {
  unsigned long start = millis();
  unsigned long lastSAction = millis(); 
  unsigned long nextSInterval = random(2000, 5001); // 隨機決定 2~4 秒按一次

  while (millis() - start < duration) {
    if (!running) break; 

    // 檢查是否到了按 S 的時間
    if (millis() - lastSAction >= nextSInterval) {
      Keyboard.press('s');
      delay(random(80, 150)); 
      Keyboard.release('s');
      
      lastSAction = millis(); 
      nextSInterval = random(2000, 5001); // 重新決定下一次 2~4 秒
    }

    delay(10); // 基礎掃描頻率，保持系統靈敏
  }
}

void smartDelay(unsigned long duration) {
  unsigned long start = millis();
  while (millis() - start < duration) {
    if (!running) return;
    delay(random(5, 15)); 
  }
}