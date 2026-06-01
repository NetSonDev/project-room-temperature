// ---------- Библиотека ----------
#include <DHT.h>

// ---------- Пины ----------
const int BUTTON_PIN   = 3;   // кнопка
const int RED_LED_PIN  = 4;   // красный диод
const int GREEN_LED_PIN= 5;   // зелёный диод
const int DHT_PIN      = 2;   // DHT‑датчик (цифровой вывод)

// ---------- Тип датчика ----------
/*  Если у тебя DHT11 → #define DHTTYPE DHT11
    Если DHT22 / AM2302 → #define DHTTYPE DHT22               */
#define DHTTYPE DHT11         // <‑ поменяй при необходимости

DHT dht(DHT_PIN, DHTTYPE);

// ---------- Параметры ----------
const unsigned long LED_ON_TIME   = 5000; // мс, как долго светит диод
const unsigned long DEBOUNCE_MS   = 50;   // защита от дребезга кнопки

// Диапазон «комнатной» температуры (с небольшим запасом)
const float ROOM_LOW  = 19.5;   // чуть ниже 20 °C
const float ROOM_HIGH = 27.5;   // чуть выше 27 °C

// При необходимости можно добавить системную поправку
const float TEMP_OFFSET = 0.0;

// ---------- Переменные для дебаунса ----------
bool lastButtonState = HIGH;   // INPUT_PULLUP → HIGH = кнопка не нажата
bool buttonState;
unsigned long lastDebounceTime = 0;

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    digitalWrite(RED_LED_PIN,   LOW);
    digitalWrite(GREEN_LED_PIN, LOW);

    Serial.begin(9600);
    dht.begin();                 // инициализируем датчик
    Serial.println(F("System ready"));
}

void loop() {
    // ----- Дебаунс кнопки -----
    int reading = digitalRead(BUTTON_PIN);
    if (reading != lastButtonState) {
        lastDebounceTime = millis();            // сброс таймера
    }

    if ((millis() - lastDebounceTime) > DEBOUNCE_MS) {
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == LOW) {            // нажали кнопку
                handlePress();
            }
        }
    }
    lastButtonState = reading;
}

// ---------- Обработчик нажатия ----------
void handlePress() {
    // Читаем температуру (°C)
    float rawTemp = dht.readTemperature();   // <-- возвращает °C
    if (isnan(rawTemp)) {                     // если чтение провалилось
        Serial.println(F("ERROR: Failed to read from DHT sensor"));
        return;
    }

    float temperature = rawTemp + TEMP_OFFSET;   // учитываем поправку
    Serial.print(F("Temperature (raw): "));
    Serial.print(rawTemp);
    Serial.print(F("  corrected: "));
    Serial.print(temperature);
    Serial.println(F(" °C"));

    // Выбираем, какой светодиод подсветить
    if (temperature >= ROOM_LOW && temperature <= ROOM_HIGH) {
        Serial.println(F("Room temperature → GREEN LED"));
        digitalWrite(GREEN_LED_PIN, HIGH);
    } else {
        Serial.println(F("Out of range → RED LED"));
        digitalWrite(RED_LED_PIN, HIGH);
    }

    // Держим светодиод включённым 5 сек.
    delay(LED_ON_TIME);

    // Выключаем оба диода (на всякий случай)
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN,   LOW);
}
