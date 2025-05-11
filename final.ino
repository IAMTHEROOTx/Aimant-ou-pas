#include "Wire.h"

const int pinCapteur = 36; //voie1
const int pinCapteur2 = 39; //voie2

int donneeRecue;
int valeurParDefaultV1;
int valeurParDefaultV2;
const byte addr = 200;

#define addr 0xC8
unsigned char H, B;

void ecouteDuReseauRS485(){
  if (Serial1.available()) 
  {
    int inByte = Serial1.read();

    H=inByte/16 +48;
    if(H>0x39) H+=7;

    B=inByte%16 +48;
    if(B>0x39) B+=7;

    if(inByte==0xFE) Serial.write('\n');

    Serial.write(H);    
    Serial.write(B);    
    Serial.write(' ');
    
    /*
    if (H == addr && B == 0x01) { // Si je reçoit une trame avec l'adresse 200
      envoyerTrame(addr, voie1);  // Envoi de la trame pour voie 1
    } 
    //if (inByte == 0x31)
    else if (H == addr && B == 0x02) {  // Adresse 200, voie 2
      envoyerTrame(addr, voie2);  // Envoi de la trame pour voie 2
    }
  }
  */
}

void capteurHall(){
  Wire.beginTransmission(0x3D);//0x3D adresse
  Wire.write(3);//mettre sous tension tous les capteurs
  Wire.endTransmission();
}

bool aimantOuPas(int valeurCapteur, int valeurCapteur2) {
  if (valeurCapteur > valeurParDefaultV1 || valeurCapteur < valeurParDefaultV1) {
    return true; 
  } else if (valeurCapteur2 > valeurParDefaultV2 || valeurCapteur2 < valeurParDefaultV2) {
    return true; 
  }
  return false; // Aucun aimant détecté
}

void envoyerTrame(byte adresse, byte voie, byte donneeRecue) {
  byte trame[4];
  trame[0] = adresse;
  trame[1] = voie;
  trame[2] = donneeRecue;
  trame[3] = (trame[0] + trame[1] + trame[2]) & 0xFF; 

  Serial1.write(trame, 4); 
  Serial.printf("Envoi trame : Addr=%d Voie=%d Statut=0x%X Checksum=0x%X\n", adresse, voie, donneeRecue, trame[3]);
}
void setup() {
  Serial.begin(9600);
  Serial.printf("\n\n visu en Hexa des trames sur reseau vannes:\n\n");
  Wire.begin();
  valeurParDefaultV1 = analogRead(pinCapteur);
  valeurParDefaultV2 = analogRead(pinCapteur2);
  capteurHall();

  Serial.printf("Valeurs par défaut : V1=%d, V2=%d\n", valeurParDefaultV1, valeurParDefaultV2);
}

void loop() {
  ecouteDuReseauRS485();

  int valeurCapteur = analogRead(pinCapteur);
  int valeurCapteur2 = analogRead(pinCapteur2);
  Serial.printf("V1= %d V2=%d\n", valeurCapteur, valeurCapteur2);

  bool aimantDetecte = aimantOuPas(valeurCapteur, valeurCapteur2);

  if (aimantDetecte) {
    Serial.println("Aimant détecté !");
    envoyerTrame(addr, 1, 0x31);
  } else {
    Serial.println("Aucun aimant détecté.");
    envoyerTrame(addr, 1, 0x30);
  }

  delay(1000);
}
