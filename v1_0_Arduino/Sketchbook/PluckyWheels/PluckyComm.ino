// **************************
// Remote control code
// **************************

//#define COMM_SIMPLE
#define COMM_ELEMENT

#ifdef COMM_ELEMENT

int j = 0; // sensor "randomizer" for debugging

void control()
{
  while(Serial.available())
  {
    String str = Serial.readStringUntil('\r');
    if(str == "reset")
    {
        Serial.print("Arduino firmware Plucky Wheels\r\n>");  // code phrase, checked by C# side
    }
    else
    {
      //Serial.println("read: '" + str + "'");
      char chars[100];
      char* tokens[5];
      int channels[5];
      int pwms[5];
      str.toCharArray(chars, 100);
      int i=0; 

      // strtok replaces " " with '\0'
      const char sep[2] = " ";
      tokens[i] = strtok(chars, sep);
      if(tokens[i] == NULL)
      {
        return;
      }
      
      while (i < 5 && tokens[i] != NULL)
      {
        //Serial.print("token: ");
        //Serial.println(tokens[i]);
        tokens[++i] = strtok(NULL, sep);
      }

      //Serial.print(i);

      if(!strcmp(tokens[0], "cfg"))
      {
        //Serial.println("OK: cfg");
        Serial.print("\r\nACK\r\n>");
      }
      else if(!strcmp(tokens[0], "psonar"))
      {
            receiveI2cPacket();
            Serial.print("\r\n");
            Serial.print(rangeFRcm);
            Serial.print(" ");
            Serial.print(rangeFLcm);
            Serial.print(" ");
            Serial.print(rangeBRcm);
            Serial.print(" ");
            Serial.print(rangeBLcm);
            Serial.print("\r\n>");
      }
      else if(!strcmp(tokens[0], "sensor"))
      {
        //Serial.println("OK: sensor");
        int pin = atoi(tokens[1]);
        switch(pin)
        {
          case 5: // battery
            Serial.print("\r\n");
            //Serial.print(800);    // "800" here relates to battery voltage 11.72V = 3.91V per cell
            Serial.print(analogRead(batteryInPin));
            Serial.print("\r\n>");
            break;

          default:
            Serial.print("\r\n");
            Serial.print(analogRead(pin));
            Serial.print("\r\n>");
            break;            
        }
      }
      else if(!strcmp(tokens[0], "odomreset"))
      {
        odometry->Reset();
        X = Y = Theta = 0.0;
        Ldistance = Rdistance = 0;
        Serial.print("\r\nACK\r\n>");
      }
      else if(!strcmp(tokens[0], "odom"))
      {
        Serial.print("\r\n");
        Serial.print(Ldistance);
        Serial.print(" ");
        Serial.print(Rdistance);
        Serial.print(" ");
        Serial.print(X);
        Serial.print(" ");
        Serial.print(Y);
        Serial.print(" ");
        Serial.print(Theta);
        Serial.print("\r\n>");
      }
      else if(!strcmp(tokens[0], "srf04"))
      {
        //Serial.println("OK: srf04");
        Serial.print("\r\n"); Serial.print(200 + j); Serial.print("\r\n>");
      }
      else if(!strcmp(tokens[0], "getenc"))
      {
        //Serial.println("OK: getenc");
        switch(atoi(tokens[1]))
        {
          case 1:
            Serial.print("\r\n");
            Serial.print(Rdistance);
            Serial.print("\r\n>");
            break;
          case 2:
            Serial.print("\r\n");
            Serial.print(Ldistance);
            Serial.print("\r\n>");
            break;
          default:
            Serial.print("\r\n0\r\n>");
            break;            
        }
      }
      else if(!strcmp(tokens[0], "pwm"))
      {
        // 'pwm 1:35 2:-65'
        //Serial.println("OK: pwm");
        for(int k=1; k < i ;k++)
        {
          channels[k-1] = atoi(tokens[k]);
          pwms[k-1] = atoi(tokens[k] + 2);

//          Serial.print("   ch=");
//          Serial.print(channels[k-1]);
//          Serial.print("   pwm=");
//          Serial.println(pwms[k-1]);
        }
        Serial.print("\r\nACK\r\n>");

        for(int k=0; k < i-1 ;k++)
        {
          // "pwm" comes in the range -100...100 - it has a meaning of "percent of max speed"
          switch(channels[k])
          {
            case 1:
              desiredSpeedR = pwms[k];
              break;
            case 2:
              desiredSpeedL = pwms[k];
              break;
          }
        }        
      }
      else if(!strcmp(tokens[0], "vel"))
      {
        //Serial.println("OK: vel");
        Serial.print("\r\n0 0 \r\n>");
      }
      else
      {
        Serial.println("****************** unknown read: '" + str + "'");
      }
    }
  }
  if(++j > 50)   // sensor "randomizer" for debugging
    j = 0;
}
#endif // COMM_ELEMENT

#ifdef COMM_SIMPLE
void control()
{
  if(Serial.available())
  {
    int val;
    val=Serial.read();
    switch (val)
    {
    case 'a':   // forward
         if(error_a < 25)         
           { error_a += 3;  }
           break;
    case 'b':   // back
         if(error_a > -25)
           { error_a -= 3;  }
           break;
    case 's':   // stop
      error_a=0;
      turn_flag=0;
      break;
    case 'r':  // right, CW (increase Theta)
      turn_flag=1;
      break;
    case 'l':  // left, CCW (decrease Theta)
      turn_flag=-1;
      break;
    case 'x':  // reset odometry
      Ldistance = 0;
      Rdistance = 0;
      odometry->Reset();
      break;
    case 'z':  // reset position
      X = 0.0;
      Y = 0.0;
      Theta = 0.0;
      break;
      
    default:
      break;
      
    }
  }
}
#endif // COMM_SIMPLE
