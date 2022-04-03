#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <algorithm>
#include <vector>

#define PIN 5
#define LED_COUNT 59

float get_distance();
float detect_first_hand_contact();
void set_pixels(float last_distances[5]);
void light_led(int led_index);
float maesure();

// Initialisierung Ultraschallsensor
const int trigger = 15;
const int echo = 4;
const float schallgeschwindigkeit = 0.0342;
const float led_distance = 1.66666;

bool pixel_status[LED_COUNT];
int last_led = 0;

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
  float initial_distance = detect_first_hand_contact();
  float last_distances[5] = { 0 , 0 , 0 , 0 , 0 };
  // TODO es muss überprüft werden, ob die Hand in einem Bereich ist, der angeschaltet oder ausgeschaltet ist
  // je nach Fall muss dieser Bereich aus- oder angeschaltet werden
  if ( initial_distance != 0 )
  {
    int initial_led = round(initial_distance / led_distance);
    light_led(initial_led);
    last_distances[0] = initial_distance;
    while (last_distances[0] < 98 || last_distances[0] > 102)
    {
      last_distances[4] = last_distances[3];
      last_distances[3] = last_distances[2];
      last_distances[2] = last_distances[1];
      last_distances[1] = last_distances[0];
      last_distances[0] = get_distance();
      set_pixels(last_distances);
    }
  delay(100);
  }
  delay(100);
}

float detect_first_hand_contact()
{
  float first_distance = get_distance();
  delay(200);
  float second_distance = get_distance();
  if ( (first_distance - second_distance) < 1) 
  {
    if (first_distance < 0 || first_distance > 100)
    {
      return 0;
    }
    else 
    {
      return (first_distance + second_distance) / 2;
    }
  }
  else return 0;
}

float get_distance()
{
  // use mean as distance
  float measurements[5] = { maesure(), maesure(), maesure(), maesure(), maesure() };
  std::sort(measurements, measurements+5);
  return measurements[3];
}

float maesure()
{
  digitalWrite(trigger, LOW);
  delay(5);
  digitalWrite(trigger, HIGH);
  delay(10);
  digitalWrite(trigger, LOW);
  float dauer = pulseIn(echo, HIGH);
  return (dauer / 2) * schallgeschwindigkeit;
}

// TODO: Hier muss die Funktion noch schlau gemacht werden.
// Es müssen realistische Werte aus den Messungen davor abgeleitet werden
void set_pixels(float last_distances[5])
{
  float loop_distance = last_distances[0];
  if (loop_distance != 0 && loop_distance != 1000)
  {
    int loop_led = round(loop_distance / led_distance);
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
        // Mach hier mal nix
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

void light_led(int led_index)
{
  strip.setPixelColor(led_index, strip.Color(0, 100, 0, 0));  
  pixel_status[led_index] = true;

  if ( led_index > 2 && led_index < (LED_COUNT - 3))
  {
    if ( !pixel_status[led_index - 1])
    {
    strip.setPixelColor(led_index - 2, strip.Color(0, 5, 0, 0));
    strip.setPixelColor(led_index - 1, strip.Color(0, 15, 0, 0));  
    }
    if ( !pixel_status[led_index + 1])
    {
    strip.setPixelColor(led_index + 1, strip.Color(0, 5, 0, 0));  
    strip.setPixelColor(led_index + 2, strip.Color(0, 15, 0, 0));  
    }
  }
  strip.show();
}