#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN 14
#define LED_COUNT 30

void shot_sound();
void get_distance();
void check_if_on_off_initialized();
void set_pixels();

// Initialisierung Ultraschallsensor
int trigger = 15;
int echo = 4;
long dauer = 0;
long loop_distance = 0;
long initial_distance = 0;
float schallgeschwindigkeit = 0.0342;
float led_distance = 1.66666;
bool initial_on_off = false;
int loop_led = 0;
int initial_led = 0;
int last_led = 0;
bool imprecise_measure = false;

bool pixel_status[LED_COUNT];

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_RGBW + NEO_KHZ800);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  for (int i = 0; i < LED_COUNT; i++)
  {
    pixel_status[i] = false;
  }
}

void loop()
{
  // put your main code here, to run repeatedly:

  check_if_on_off_initialized();
  if (initial_on_off)
  {
    initial_led = round(initial_distance / led_distance);
    Serial.println(initial_led);
    strip.setPixelColor(initial_led, strip.Color(0, 100, 0, 0));
    pixel_status[initial_led] = true;
    last_led = initial_led;
    strip.show();
  }

  while (loop_distance != 0)
  {
    // Jetzt muss noch eine Funktion geschrieben werden um die Lampen aus zu machen
    set_pixels();
  }

  Serial.println("war null");
  initial_on_off = false;
  loop_distance = 0;
  delay(100);
}

void check_if_on_off_initialized()
{
  shot_sound();
  dauer = pulseIn(echo, HIGH);
  long distance_first_check = (dauer / 2) * schallgeschwindigkeit;
  delay(200);

  shot_sound();
  dauer = pulseIn(echo, HIGH);
  long distance_second_check = (dauer / 2) * schallgeschwindigkeit;
  if (abs((distance_second_check - distance_first_check)) <= 2 && distance_first_check < 50 && distance_first_check > 0)
  {
    initial_distance = distance_first_check;
    loop_distance = distance_first_check;
    initial_on_off = true;
  }
}

void get_distance()
{
  long first_maesured_distance;
  long second_maesured_distance;
  long third_maesured_distance;

  shot_sound();
  dauer = pulseIn(echo, HIGH);
  first_maesured_distance = (dauer / 2) * schallgeschwindigkeit;

  shot_sound();
  dauer = pulseIn(echo, HIGH);
  second_maesured_distance = (dauer / 2) * schallgeschwindigkeit;
  shot_sound();

  dauer = pulseIn(echo, HIGH);
  third_maesured_distance = (dauer / 2) * schallgeschwindigkeit;

  if (first_maesured_distance == second_maesured_distance && first_maesured_distance == third_maesured_distance)
  {
    loop_distance = first_maesured_distance;
    imprecise_measure = false;
  }
  else if (first_maesured_distance != second_maesured_distance && first_maesured_distance == third_maesured_distance)
  {
    loop_distance = first_maesured_distance;
    imprecise_measure = true;
  }
  else if (first_maesured_distance == second_maesured_distance && first_maesured_distance != third_maesured_distance)
  {
    loop_distance = first_maesured_distance;
    imprecise_measure = true;
  }

  else if (first_maesured_distance != second_maesured_distance && second_maesured_distance == third_maesured_distance)
  {
    loop_distance = second_maesured_distance;
    imprecise_measure = true;
  }
  else
  {
    loop_distance = 1000;
  }

  if (loop_distance > 50 && loop_distance < 55)
  {
    loop_distance = 0;
  }
}

void shot_sound()
{
  digitalWrite(trigger, LOW);
  delay(5);
  digitalWrite(trigger, HIGH);
  delay(10);
  digitalWrite(trigger, LOW);
}

void set_pixels()
{
  get_distance();
    if (loop_distance != 0 && loop_distance != 1000)
    {
      loop_led = round(loop_distance / led_distance);
      if ((loop_led - last_led) == 1)
      {
        if (pixel_status[loop_led - 1] || pixel_status[loop_led + 1])
        {
          strip.setPixelColor(loop_led, strip.Color(0, 100, 0, 0));
          pixel_status[loop_led] = true;
          strip.show();
        }
      }
      else
      {
        if (loop_led == last_led)
        {
        }
        else if (loop_led < last_led)
        {
          for (int i = loop_led; i < last_led; i++)
            strip.setPixelColor(i, strip.Color(0, 100, 0, 0));
        }
        else if (loop_led > last_led)
        {
          for (int i = last_led + 1; i <= loop_led; i++)
            strip.setPixelColor(i, strip.Color(0, 100, 0, 0));
        }
        strip.show();
      }
      delay(200);
    }
}
