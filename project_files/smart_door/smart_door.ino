
#include <Adafruit_DHT.h>
#define DHTPIN D1    // what pin we're connected to
#define DHTTYPE DHT11		// DHT 22 (AM2302)3

/* humidity function*/
DHT dht(DHTPIN, DHTTYPE);

/********************************************************PIN CONFIGURATIONS********************************************************/
//temparature
const int temperatureInputPin = A2;
//light level
const int photoresistorInputPin = A0;
const int photoresistorOutputPin = A5;

//magnet sensoure
const int magnetInputPin = D3; // magnet part is connected to D3

//led light
const int led1 = D0;
//sound
const int speakerOutputPin = D2;

/********************************************************PARTICLE CLOUD VARIABLE********************************************************/

double temperature = 0.0; //temperature
double humidity = 0.0; //humidity
int lightLevel=0; //light Level
String doorLock = "UNKNOWN"; //door lock

/********************************************************PARTICLE CLOUD FUNCTIONS********************************************************/

/********************************************************PARTICLE CLOUD EVENTS********************************************************/

/********************************************************Local VARIABLE********************************************************/
// magnet sensor
int magnetValState=0; 
//melody
const int songLength = 18;
char notes[] = "cdfda ag cdfdg gf ";
int beats[] = {1,1,1,1,1,1,4,4,2,1,1,1,1,1,1,4,4,2};
int tempo = 150;

void setup() {
    
    //temparature
    pinMode(temperatureInputPin,INPUT);
    
    //light level
    pinMode(photoresistorInputPin,INPUT);
    pinMode(photoresistorOutputPin,OUTPUT);
    digitalWrite(photoresistorOutputPin,HIGH);
    
    //magnetic switch door lock
    pinMode(magnetInputPin, INPUT_PULLUP);
    
    //led light
    pinMode(led1, OUTPUT);
   
   //sound
    pinMode(speakerOutputPin, OUTPUT);
    
    //start reading temparature & humidity
    dht.begin();
    
    //particle cloud variable
    Particle.variable("temparature", &temperature, DOUBLE);
    Particle.variable("humidity", &humidity, DOUBLE);
    Particle.variable("lightLevel", &lightLevel, INT);
    Particle.variable("doorLock", &doorLock, STRING);
    
    //particle cloud function
    Particle.function("led",ledToggle); //on or off led light
    Particle.function("playMelody",playMelody); //play sound
    
    //switch off led
    digitalWrite(led1, LOW);

}

void loop() {
    
    //read & publish temperature & humidity sensor data
    float h = dht.getHumidity();
    humidity=(double)h;
    
    float t = dht.getTempCelcius();
    temperature=(double)t;
    
    Particle.publish("temperature",String(temperature),60,PRIVATE);
    Particle.publish("humidity",String(humidity),60,PRIVATE);
    
    //read & publish light level from photoresistor
    lightLevel = analogRead(photoresistorInputPin);
    Particle.publish("lightLevel",String(lightLevel)+" Flux",60,PRIVATE);
    
    //read & publish magnetic door sensor data
    magnetValState = digitalRead(magnetInputPin);
    
    if(magnetValState == HIGH){// Was motion detected
        
        doorLock="OPEN";
        Particle.publish("doorLock","OPEN",60,PRIVATE);
        playSound();
     }
    else
    {
        doorLock="CLOSE";
        Particle.publish("doorLock","CLOSE",60,PRIVATE);
    }
    
    //flash & delay
    flashLight(led1);
    delay(6000);

}

//particle function to play melody
int playMelody(String command){
    playSound();
    return 0;
}

//particle cloud function ,retur 1 if light ON, return 0 if light OFF
int ledToggle(String command) {
    if (command=="on") {
        digitalWrite(led1,HIGH);
        return 1;
    }
    else if (command=="off") {
        digitalWrite(led1,LOW);
        return 0;
    }
    else {
        return -1;
    }
}

//falsh light
void flashLight(int boardLed){
  digitalWrite(boardLed,HIGH);
  delay(100);
  digitalWrite(boardLed,LOW);
  delay(100);
  digitalWrite(boardLed,HIGH);
  delay(100);
  digitalWrite(boardLed,LOW);
  delay(100);
    
}

//play sound
void playSound(){
    // We only want to play the song once, so we'll put it in the setup loop
  int i, duration;

  for (i = 0; i < songLength; i++) // step through the song arrays
  {
    duration = beats[i] * tempo;  // length of note/rest in ms

    if (notes[i] == ' ')          // is this a rest? 
    {
      delay(duration);            // then pause for a moment
    }
    else                          // otherwise, play the note
    {
      tone(speakerOutputPin, frequency(notes[i]), duration);
      delay(duration);            // wait for tone to finish
    }
    delay(tempo/10);              // brief pause between notes
  }
}

//frequency for melody
int frequency(char note) 
{
  // This function takes a note character (a-g), and returns the
  // corresponding frequency in Hz for the tone() function.

  int i;
  const int numNotes = 8;  // number of notes we're storing

  // The following arrays hold the note characters and their
  // corresponding frequencies. The last "C" note is uppercase
  // to separate it from the first lowercase "c". If you want to
  // add more notes, you'll need to use unique characters.

  // For the "char" (character) type, we put single characters
  // in single quotes.

  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int frequencies[] = {262, 294, 330, 349, 392, 440, 494, 523};

  // Now we'll search through the letters in the array, and if
  // we find it, we'll return the frequency for that note.

  for (i = 0; i < numNotes; i++)  // Step through the notes
  {
    if (names[i] == note)         // Is this the one?
    {
      return(frequencies[i]);     // Yes! Return the frequency
    }
  }
  return(0);  // We looked through everything and didn't find it,
              // but we still need to return a value, so return 0.
}
