#include <Keyboard.h>

// ==========================================
// 1. 核心參數設定區
// ==========================================
const float RUN_TIME_MINS = 14.0;       // 總掛機時間
const int START_DELAY_SEC = 10;         // 初始等待時間

const char KEY_ATTACK    = 'a';         
const char KEY_BUFF      = 'c';         
const char KEY_MAPLE_X   = 'x';         
const char KEY_ALT_JUMP  = KEY_LEFT_ALT; 
const char MY_HOME_KEY   = '=';         

// --- 動作節奏參數 ---
const int JUMP_PRESS_TIME = 80;         
const int DOWN_JUMP_HOLD  = 180;        
const int DOUBLE_JUMP_GAP = 320;        
const int GAP_BETWEEN_JUMPS_MIN = 800;  
const int GAP_BETWEEN_JUMPS_MAX = 1200; 

// --- 待命時間範圍 ---
const unsigned long STAND_TIME_MIN = 55000; 
const unsigned long STAND_TIME_MAX = 60000; 

// ==========================================
// 2. 系統變數
// ==========================================
unsigned long scriptStartTime = 0;
unsigned long maxDurationMS = RUN_TIME_MINS * 60UL * 1000UL;

void setup() {
  Keyboard.begin();
  pinMode(13, OUTPUT);
  randomSeed(analogRead(0));
  delay(START_DELAY_SEC * 1000UL);
  scriptStartTime = millis();
}

void loop() {
  if (millis() - scriptStartTime < maxDurationMS) {
    doFullAttackCycle(); 
  } else {
    stopScript();
    while(true); 
  }
}

// ==========================================
// 3. 基礎動作封裝 (人性化微調版)
// ==========================================

void mapleAltXJump() {
  Keyboard.press(KEY_ALT_JUMP);
  delay(random(20, 40)); 
  Keyboard.press(KEY_MAPLE_X);
  delay(JUMP_PRESS_TIME + random(-5, 15)); 
  Keyboard.release(KEY_MAPLE_X);
  Keyboard.release(KEY_ALT_JUMP);
}

void performDoubleJump() {
  mapleAltXJump();
  Keyboard.press(KEY_RIGHT_ARROW); 
  delay(DOUBLE_JUMP_GAP + random(-10, 20));
  mapleAltXJump();
}

void mapleDownJump() {
  Keyboard.press(KEY_DOWN_ARROW);
  delay(DOWN_JUMP_HOLD + random(-10, 20)); 
  Keyboard.press(KEY_ALT_JUMP);
  delay(JUMP_PRESS_TIME + random(0, 10));
  Keyboard.release(KEY_ALT_JUMP);
  delay(random(100, 150)); 
  Keyboard.release(KEY_DOWN_ARROW);
}

// --- 核心：定點待命攻擊 (含隨機動作與位移補償) ---
void standAndAttack(unsigned long duration) {
  unsigned long start = millis();
  
  // 隨機動作計時器 (每 7~15 秒判斷一次)
  unsigned long lastAction = millis(); 
  unsigned long nextActionInterval = random(7000, 15001); 

  // 攻擊計時器 (約 0.5~1 秒攻擊一次)
  unsigned long lastAttack = millis();
  unsigned long nextAttackInterval = random(500, 1001);

  while (millis() - start < duration) {
    if (millis() - scriptStartTime >= maxDurationMS) return;

    // 1. 穩定攻擊：模擬真人按壓時間
    if (millis() - lastAttack >= nextAttackInterval) {
      Keyboard.press(KEY_ATTACK);
      delay(random(80, 150));
      Keyboard.release(KEY_ATTACK);
      lastAttack = millis();
      nextAttackInterval = random(500, 1100);
    }

    // 2. 隨機動作判斷 (跳躍或橫移回原點)
    if (millis() - lastAction >= nextActionInterval) {
      int dice = random(0, 100);
      
      if (dice < 20) { // 20% 原地跳
        Keyboard.press(KEY_ALT_JUMP);
        delay(random(100, 150)); 
        Keyboard.release(KEY_ALT_JUMP);
      }
      else if (dice < 40) { // 20% 左右橫移歸位
        int moveTime = random(250, 450); // 位移距離隨機化
        
        Keyboard.releaseAll(); 
        delay(random(100, 200));
        
        // 往左走 (moveTime + 100ms 補償，抵銷往右偏的傾向)
        Keyboard.press(KEY_LEFT_ARROW);
        delay(moveTime + 300); 
        Keyboard.release(KEY_LEFT_ARROW);
        
        delay(random(350, 500)); 
        
        // 往右走 (回到原位附近)
        Keyboard.press(KEY_RIGHT_ARROW);
        delay(moveTime); 
        Keyboard.release(KEY_RIGHT_ARROW);

        delay(random(150, 250));
      }
      
      lastAction = millis(); 
      nextActionInterval = random(8000, 16001); 
    }
    delay(10); 
  }
}

// ==========================================
// 4. 循環主邏輯
// ==========================================
void doFullAttackCycle() {
  // A. 下跳 3 次
  for (int i = 0; i < 3; i++) {
    mapleDownJump();
    delay(random(1000, 1350)); 
  }

  // B. 放 Buff (C 鍵)
  delay(random(500, 1000));
  Keyboard.press(KEY_BUFF);
  delay(random(100, 200));
  Keyboard.release(KEY_BUFF);
  delay(random(1600, 2100)); // 等待 Buff 動作結束

  // C. 往右跑圖 (雙二段跳)
  Keyboard.press(KEY_RIGHT_ARROW);
  delay(random(80, 150));
  performDoubleJump();
  delay(random(GAP_BETWEEN_JUMPS_MIN, GAP_BETWEEN_JUMPS_MAX)); 
  performDoubleJump();
  
  // D. 跳完後的最後衝刺 (隨機 500~1200ms)
  unsigned long randomStrideTime = random(500, 1201);
  unsigned long strideTimer = millis();
  while (millis() - strideTimer < randomStrideTime) {
    if (random(0, 100) < 8) { // 衝刺時隨機揮刀
      Keyboard.press(KEY_ATTACK);
      delay(random(60, 110));
      Keyboard.release(KEY_ATTACK);
    }
    delay(50);
  }
  Keyboard.release(KEY_RIGHT_ARROW); 
  delay(random(400, 700));

  // E. 進入定點待命攻擊
  unsigned long waitTime = random(STAND_TIME_MIN, STAND_TIME_MAX + 1);
  standAndAttack(waitTime); 
  
  Keyboard.releaseAll(); 
  delay(random(200, 500));
}

// ==========================================
// 5. 腳本結束 (時間到)
// ==========================================
void stopScript() {
  Keyboard.releaseAll();
  delay(500);
  Keyboard.write(MY_HOME_KEY);
  delay(500); 
  Keyboard.write(KEY_RETURN);
  for(int i = 0; i < 20; i++) {
    digitalWrite(13, HIGH); delay(100);
    digitalWrite(13, LOW); delay(100);
  }
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(500);
  Keyboard.releaseAll();
  delay(1000);
  Keyboard.print("1"); 
}