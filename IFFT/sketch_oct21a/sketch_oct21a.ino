const int ledPin = 12;  
 
int freq;
int ledChannel = 0;
int resolution = 8;
 
void setup()
{
  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin, ledChannel);
}
 
void loop()
{
  pwm_set(2000);
}

void pwm_set(int freq_s)
{
  ledcSetup(ledChannel,freq_s,resolution);
  ledcWrite(ledPin,128);
}