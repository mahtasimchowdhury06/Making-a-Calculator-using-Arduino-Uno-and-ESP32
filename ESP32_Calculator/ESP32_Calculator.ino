#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD Configuration (I2C address: 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad Layout & Wiring
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', '+'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', 'C'},
  {'*', '0', '=', '/'}
};
byte rowPins[ROWS] = {16, 17, 18, 19}; 
byte colPins[COLS] = {5, 23, 15, 25}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Calculator Variables
String input = "";
int num1 = 0, num2 = 0;
char op = '\0';
bool resetAfterEquals = false;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESP32 Calculator");
  delay(1500);
  lcd.clear();
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    handleKeyPress(key);
  }
}

void handleKeyPress(char key) {
  // Clear calculator
  if (key == 'C') {
    resetCalculator();
    lcd.clear();
    lcd.setCursor(0, 0);
    return;
  }

  // Handle equals (=)
  if (key == '=' && op != '\0' && input.length() > 0) {
    num2 = input.toInt();
    int result = calculate(num1, num2, op);
    displayResult(result);
    num1 = result; // Store result for next operation
    resetAfterEquals = true;
    return;
  }

  // Handle operators (+, -, *, /)
  if (isOperator(key) && input.length() > 0) {
    if (op == '\0') {
      num1 = input.toInt();
    }
    op = key;
    resetAfterEquals = false;
    updateDisplay();
    input = "";
    return;
  }

  // Handle digits
  if (key >= '0' && key <= '9') {
    if (resetAfterEquals) {
      input = "";
      resetAfterEquals = false;
    }
    input += key;
    updateDisplay();
  }
}

bool isOperator(char k) {
  return (k == '+' || k == '-' || k == '*' || k == '/');
}

int calculate(int a, int b, char operation) {
  switch (operation) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': return (b != 0) ? a / b : INT_MAX; // Handle division by zero
    default: return INT_MAX;
  }
}

void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  // Show previous number and operator
  if (op != '\0') {
    lcd.print(num1);
    lcd.print(" ");
    lcd.print(op);
  }
  // Show current input
  lcd.setCursor(0, 1);
  lcd.print(input);
}

void displayResult(int result) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(num1);
  lcd.print(" ");
  lcd.print(op);
  lcd.print(" ");
  lcd.print(num2);
  
  lcd.setCursor(0, 1);
  if (result == INT_MAX) {
    lcd.print("Error: Div/0!");
  } else {
    lcd.print("= ");
    lcd.print(result);
  }
}

void resetCalculator() {
  input = "";
  num1 = 0;
  num2 = 0;
  op = '\0';
  resetAfterEquals = false;
}