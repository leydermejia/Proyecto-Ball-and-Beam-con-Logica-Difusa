/*
 Código Obtenido del repositorio de github.com de Hugo Marquez.  https://github.com/hugomarquez

 Modificado por Leyder Londoño
 
 */

/*se importan las librerias para trabajar con lógica difusa (FuzzyIO.h, FuzzyRule.h, FuzzyOutput.h, 
  FuzzyComposition.h, FuzzySet.h, FuzzyRuleAntecedent.h, Fuzzy.h, FuzzyRuleConsequent.h, FuzzyInput.h) estas librerias
  obtener las entradas y salida para el sistema difuso en construcción, tambien nos permite realizar fusificación y desuficación
  con relación a las reglas de antecedentes y de consecuencias.

  También se incluye la libreria de Servo.h para crear objetos de tipo servo y trabjar con ellos
*/

#include <FuzzyIO.h>
#include <FuzzyRule.h>
#include <FuzzyOutput.h>
#include <FuzzyComposition.h>
#include <FuzzySet.h>
#include <FuzzyRuleAntecedent.h>
#include <Fuzzy.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzyInput.h>
#include <Servo.h>


//Se crea objeto servo
Servo servo;

// Se establece la longitud de la viga
int const L = 47;
// Se establece el radio de la bola
int const R = 2;
// Se establece el Angulo minimo del dispositivo
int const MIN = 50;
// Se establece el Angulo maximo del dispositivo
int const MAX = 160;
// Se establece en milisegundos la reación del dispositivo
int const velocidadReacion = 50;
// Se establece la posición inicial del servo motor
int const posicionInicialMotor = 100;
// Varible para indicar si es necesario (o no) mover la viga, sirve para hacer mantenimiento al programa
bool moveMotor = false;

// Se establece el Punto de referencia (set point)
float referencia = 23;

//se definen los pines usados en el sensor ultrasonico
# define trigPin 13
# define echoPin 12

//se define el pin usado para el servo motor
# define servoPin 9

//Se crea un nuevo objeto de la clase Fuzzy()
Fuzzy* fuzzyObj = new Fuzzy();

//Variables linguisticas usadas en el sistema difuso:
// NM - Negativo Mediano
// NP - Negativo Pequeño
// CO - Cero
// PP - Positivo Pequeño
// PM - Positivo Mediano


// Se crean los conjuntos de pertenencia para las entradas y salida de sistema difuso
//donde "posicion_<variable linguistica>" va a ser el conjunto de pertenencia de entrada al sistema disfuso para la posicion real de la bola,
//"referencia_<variable linguistica>" va a ser el conjunto de pertenencia de entrada al sistema disfuso para la posicion deseada de la bola
// y "angulo_<variable linguistica>" va a ser el conjunto de pertenencia de salida del sistema disfuso para el angulo al cual se posicionara la viga

FuzzySet* posicion_nm = new FuzzySet(0, 0, 3, 6);
FuzzySet* posicion_np = new FuzzySet(3, 6, 9, 12);
FuzzySet* posicion_co = new FuzzySet(9, 12, 15, 18);
FuzzySet* posicion_pp = new FuzzySet(15, 18, 21, 24);
FuzzySet* posicion_pm = new FuzzySet(21, 24, 26, L);

FuzzySet* referencia_nm = new FuzzySet(0, 0, 3, 6);
FuzzySet* referencia_np = new FuzzySet(3, 6, 9, 12);
FuzzySet* referencia_co = new FuzzySet(9, 12, 15, 18);
FuzzySet* referencia_pp = new FuzzySet(15, 18, 21, 24);
FuzzySet* referencia_pm = new FuzzySet(21, 24, 27, L);

FuzzySet* angulo_nm = new FuzzySet(MIN, 50, 60, 80);
FuzzySet* angulo_np = new FuzzySet(60, 80, 90, 100);
FuzzySet* angulo_co = new FuzzySet(90, 100, 110, 120);
FuzzySet* angulo_pp = new FuzzySet(110, 120, 120, 130);
FuzzySet* angulo_pm = new FuzzySet(130, 140, 160, MAX);


//se crea la funcion iniciarlogicaDifusa() para configurar el sistema disufo
void iniciarlogicaDifusa(){
  //Se definen en el sistema difuso que los conjuntos "posicion_<variable linguistica>" y "referencia_<variable_linguistica>" son de entrada
  // y que el conjunto "angulo_<variable linguistica>" representara la salida del sistema
  FuzzyInput* posicionBola = new FuzzyInput(1);
  
  posicionBola->addFuzzySet(posicion_nm);
  posicionBola->addFuzzySet(posicion_np);
  posicionBola->addFuzzySet(posicion_co);
  posicionBola->addFuzzySet(posicion_pp);
  posicionBola->addFuzzySet(posicion_pm);
  
  fuzzyObj->addFuzzyInput(posicionBola);

  FuzzyInput* puntoReferencia = new FuzzyInput(2);
  
  puntoReferencia->addFuzzySet(referencia_nm);
  puntoReferencia->addFuzzySet(referencia_np);
  puntoReferencia->addFuzzySet(referencia_co);
  puntoReferencia->addFuzzySet(referencia_pp);
  puntoReferencia->addFuzzySet(referencia_pm);
  
  fuzzyObj->addFuzzyInput(puntoReferencia); 

  FuzzyOutput* angulo = new FuzzyOutput(1);
  
  angulo->addFuzzySet(angulo_nm);
  angulo->addFuzzySet(angulo_np);
  angulo->addFuzzySet(angulo_co);
  angulo->addFuzzySet(angulo_pp);
  angulo->addFuzzySet(angulo_pm);
  
  fuzzyObj->addFuzzyOutput(angulo);

  // Se crean las reglas de consecuencias
  FuzzyRuleConsequent* entonces_anguloPP = new FuzzyRuleConsequent();
  entonces_anguloPP->addOutput(angulo_pp);

  FuzzyRuleConsequent* entonces_anguloPM = new FuzzyRuleConsequent();
  entonces_anguloPM->addOutput(angulo_pm);

  FuzzyRuleConsequent* entonces_anguloNM = new FuzzyRuleConsequent();
  entonces_anguloNM->addOutput(angulo_nm);

  FuzzyRuleConsequent* entonces_anguloNP = new FuzzyRuleConsequent();
  entonces_anguloNP->addOutput(angulo_np);

  FuzzyRuleConsequent* entonces_anguloCO = new FuzzyRuleConsequent();
  entonces_anguloCO->addOutput(angulo_co);

  // Se Crean las reglas de antecedentes

  // Si la posicion es NM
  FuzzyRuleAntecedent* posicionNM_puntoReferenciaNM = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionNM_puntoReferenciaNP = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionNM_puntoReferenciaCO = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionNM_puntoReferenciaPP = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionNM_puntoReferenciaPM = new FuzzyRuleAntecedent();
      
      //Como son dos entradas al sistema, se hace la unión a través del operador lógico AND del conjunto de entrada al sistema "posicion_nm" y cada uno de los conjuntos de entrada "referencia_" 
  posicionNM_puntoReferenciaNM->joinWithAND(posicion_nm, referencia_nm);
  posicionNM_puntoReferenciaNP->joinWithAND(posicion_nm, referencia_np);
  posicionNM_puntoReferenciaCO->joinWithAND(posicion_nm, referencia_co);
  posicionNM_puntoReferenciaPP->joinWithAND(posicion_nm, referencia_pp);
  posicionNM_puntoReferenciaPM->joinWithAND(posicion_nm, referencia_pm);
      
      //se crea la regla difusa, indicando la consecuencia de cada unión de los conjuntos de entrada "posicion_nm" y los conjuntos de entrada de "referencia_"
  FuzzyRule* Rule1 = new FuzzyRule(1, posicionNM_puntoReferenciaNM, entonces_anguloCO);
  FuzzyRule* Rule2 = new FuzzyRule(2, posicionNM_puntoReferenciaNP, entonces_anguloNP);
  FuzzyRule* Rule3 = new FuzzyRule(3, posicionNM_puntoReferenciaCO, entonces_anguloNP);
  FuzzyRule* Rule4 = new FuzzyRule(4, posicionNM_puntoReferenciaPP, entonces_anguloNP);
  FuzzyRule* Rule5 = new FuzzyRule(5, posicionNM_puntoReferenciaPM, entonces_anguloNM);

      //se agregan las reglas difusas
  fuzzyObj->addFuzzyRule(Rule1);
  fuzzyObj->addFuzzyRule(Rule2);
  fuzzyObj->addFuzzyRule(Rule3);
  fuzzyObj->addFuzzyRule(Rule4);
  fuzzyObj->addFuzzyRule(Rule5);

  // Si la posicion es NP
  FuzzyRuleAntecedent* posicionNP_puntoReferenciaNP = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionNP_puntoReferenciaNM = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionNP_puntoReferenciaCO = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionNP_puntoReferenciaPP = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionNP_puntoReferenciaPM = new FuzzyRuleAntecedent();
  
  posicionNP_puntoReferenciaNP->joinWithAND(posicion_np, referencia_np);
  posicionNP_puntoReferenciaNM->joinWithAND(posicion_np, referencia_nm);
  posicionNP_puntoReferenciaCO->joinWithAND(posicion_np, referencia_co);
  posicionNP_puntoReferenciaPP->joinWithAND(posicion_np, referencia_pp);
  posicionNP_puntoReferenciaPM->joinWithAND(posicion_np, referencia_pm);
  
  FuzzyRule* Rule6 = new FuzzyRule(6, posicionNP_puntoReferenciaNP, entonces_anguloCO);
  FuzzyRule* Rule7 = new FuzzyRule(7, posicionNP_puntoReferenciaNM, entonces_anguloCO);
  FuzzyRule* Rule8 = new FuzzyRule(8, posicionNP_puntoReferenciaCO, entonces_anguloNP);
  FuzzyRule* Rule9 = new FuzzyRule(9, posicionNP_puntoReferenciaPP, entonces_anguloNP);
  FuzzyRule* Rule10 = new FuzzyRule(10, posicionNP_puntoReferenciaPM, entonces_anguloNM);

  fuzzyObj->addFuzzyRule(Rule6);
  fuzzyObj->addFuzzyRule(Rule7);
  fuzzyObj->addFuzzyRule(Rule8);
  fuzzyObj->addFuzzyRule(Rule9);
  fuzzyObj->addFuzzyRule(Rule10);

  // Si la posicion es CO
  FuzzyRuleAntecedent* posicionCO_puntoReferenciaCO = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionCO_puntoReferenciaNM = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionCO_puntoReferenciaNP = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionCO_puntoReferenciaPP = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionCO_puntoReferenciaPM = new FuzzyRuleAntecedent();
  
  posicionCO_puntoReferenciaCO->joinWithAND(posicion_co, referencia_co);
  posicionCO_puntoReferenciaNM->joinWithAND(posicion_co, referencia_nm);
  posicionCO_puntoReferenciaNP->joinWithAND(posicion_co, referencia_np);
  posicionCO_puntoReferenciaPP->joinWithAND(posicion_co, referencia_pp);
  posicionCO_puntoReferenciaPM->joinWithAND(posicion_co, referencia_pm);
  
  FuzzyRule* Rule11 = new FuzzyRule(11, posicionCO_puntoReferenciaCO, entonces_anguloCO);
  FuzzyRule* Rule12 = new FuzzyRule(12, posicionCO_puntoReferenciaNM, entonces_anguloPP);
  FuzzyRule* Rule13 = new FuzzyRule(13, posicionCO_puntoReferenciaNP, entonces_anguloPP);
  FuzzyRule* Rule14 = new FuzzyRule(14, posicionCO_puntoReferenciaPP, entonces_anguloCO);
  FuzzyRule* Rule15 = new FuzzyRule(15, posicionCO_puntoReferenciaPM, entonces_anguloNP);

  fuzzyObj->addFuzzyRule(Rule11);
  fuzzyObj->addFuzzyRule(Rule12);
  fuzzyObj->addFuzzyRule(Rule13);
  fuzzyObj->addFuzzyRule(Rule14);
  fuzzyObj->addFuzzyRule(Rule15);

  // Si la posicion es PP
  FuzzyRuleAntecedent* posicionPP_puntoReferenciaPP = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionPP_puntoReferenciaNM = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionPP_puntoReferenciaNP = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionPP_puntoReferenciaCO = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionPP_puntoReferenciaPM = new FuzzyRuleAntecedent();

  posicionPP_puntoReferenciaPP->joinWithAND(posicion_pp, referencia_pp);
  posicionPP_puntoReferenciaNM->joinWithAND(posicion_pp, referencia_nm);
  posicionPP_puntoReferenciaNP->joinWithAND(posicion_pp, referencia_np);
  posicionPP_puntoReferenciaCO->joinWithAND(posicion_pp, referencia_co);
  posicionPP_puntoReferenciaPM->joinWithAND(posicion_pp, referencia_pm);

  FuzzyRule* Rule16 = new FuzzyRule(16, posicionPP_puntoReferenciaPP, entonces_anguloCO);
  FuzzyRule* Rule17 = new FuzzyRule(17, posicionPP_puntoReferenciaNM, entonces_anguloPM);
  FuzzyRule* Rule18 = new FuzzyRule(18, posicionPP_puntoReferenciaNP, entonces_anguloPP);
  FuzzyRule* Rule19 = new FuzzyRule(19, posicionPP_puntoReferenciaCO, entonces_anguloCO);
  FuzzyRule* Rule20 = new FuzzyRule(20, posicionPP_puntoReferenciaPM, entonces_anguloCO);

  fuzzyObj->addFuzzyRule(Rule16);
  fuzzyObj->addFuzzyRule(Rule17);
  fuzzyObj->addFuzzyRule(Rule18);
  fuzzyObj->addFuzzyRule(Rule19);
  fuzzyObj->addFuzzyRule(Rule20);

  // posicion PM
  FuzzyRuleAntecedent* posicionPM_puntoReferenciaPM = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionPM_puntoReferenciaNM = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionPM_puntoReferenciaNP = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionPM_puntoReferenciaCO = new FuzzyRuleAntecedent();
  FuzzyRuleAntecedent* posicionPM_puntoReferenciaPP = new FuzzyRuleAntecedent();

  posicionPM_puntoReferenciaPM->joinWithAND(posicion_pm, referencia_pm);
  posicionPM_puntoReferenciaNM->joinWithAND(posicion_pm, referencia_nm);
  posicionPM_puntoReferenciaNP->joinWithAND(posicion_pm, referencia_np);
  posicionPM_puntoReferenciaCO->joinWithAND(posicion_pm, referencia_co);
  posicionPM_puntoReferenciaPP->joinWithAND(posicion_pm, referencia_pp);

  FuzzyRule* Rule21 = new FuzzyRule(21, posicionPM_puntoReferenciaPM, entonces_anguloCO);
  FuzzyRule* Rule22 = new FuzzyRule(22, posicionPM_puntoReferenciaNM, entonces_anguloPM);
  FuzzyRule* Rule23 = new FuzzyRule(23, posicionPM_puntoReferenciaNP, entonces_anguloPM);
  FuzzyRule* Rule24 = new FuzzyRule(24, posicionPM_puntoReferenciaCO, entonces_anguloPP);
  FuzzyRule* Rule25 = new FuzzyRule(25, posicionPM_puntoReferenciaPP, entonces_anguloCO);

  fuzzyObj->addFuzzyRule(Rule21);
  fuzzyObj->addFuzzyRule(Rule22);
  fuzzyObj->addFuzzyRule(Rule23);
  fuzzyObj->addFuzzyRule(Rule24);
  fuzzyObj->addFuzzyRule(Rule25);
}

//la funcion registro() imprime a traves del serial información relevante del sistema difuso
void registro(float anguloSalida){
  reg_posicion();
  reg_puntoReferencia();
  reg_angulo(anguloSalida);
}

//funcion para imprimir la pertenencia de la posicion actual de la bola a cada conjunto de entrada "posicion_"
void reg_posicion(){
  Serial.println("");
  Serial.print("Posicion de la Bola: ");  
  Serial.print(posicion_nm->getPertinence());
  Serial.print(",");
  Serial.print(posicion_np->getPertinence());
  Serial.print(",");
  Serial.print(posicion_co->getPertinence());
  Serial.print(",");
  Serial.print(posicion_pp->getPertinence());
  Serial.print(",");
  Serial.println(posicion_pm->getPertinence());
}

//funcion para imprimir la pertenencia de la posicion deseada de la bola a cada conjunto de entrada "referencia_"
void reg_puntoReferencia(){
  Serial.print("Punto de Referencia: ");
  Serial.print(referencia_nm->getPertinence());
  Serial.print(",");
  Serial.print(referencia_np->getPertinence());
  Serial.print(",");
  Serial.print(referencia_co->getPertinence());
  Serial.print(",");
  Serial.print(referencia_pp->getPertinence());
  Serial.print(",");
  Serial.println(referencia_pm->getPertinence());
}

//funcion para imprimir la pertenencia del angulo a cada cunjunto de salida "angulo_"
void reg_angulo(float anguloSalida){
  Serial.print("Angulo: ");
  Serial.println(anguloSalida);

  Serial.print("Pertenencia de Salida: ");
  Serial.print(angulo_nm->getPertinence());
  Serial.print(",");
  Serial.print(angulo_np->getPertinence());
  Serial.print(",");
  Serial.print(angulo_co->getPertinence());
  Serial.print(",");
  Serial.print(angulo_pp->getPertinence());
  Serial.print(",");
  Serial.println(angulo_pm->getPertinence());
}

//funcion para iniciar el servo motor
void iniciarServoMotor(){
  servo.attach(servoPin);
  servo.write(posicionInicialMotor);
}

//funcion para evitar que el servo gire a más de 165 grados y menos de 20 grados
void moveServo(float angulo){
  if(angulo <= 20){angulo = MIN;}
  if(angulo >= 165){angulo = MAX;}
  
  servo.write(angulo);
}

//funcion para iniciar el sensor ultrasonico e indicando cual es el pin de eco y de recepcion
void iniciarSensorUltraSonico(){
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

//funcion para calcular la distancia a la cual se encuentra la bola
float calcularDistancia(){
  float distancia, duracion;
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  distancia = (calcularDuracion()/2) * 0.0344;
  distancia = distancia - R;
  if(distancia > L){distancia = L;}
  if(distancia <= 0){distancia = 0;}

  Serial.print("Distancia ajustada= ");
  Serial.println(distancia);
  
  return distancia;
}

//funcion para calcular la duración en tiempo en la que el sensor ultrasonico localiza la bola
float calcularDuracion(){
  float duracion;
  duracion = pulseIn(echoPin, HIGH);
  
  return duracion;
}

//función setup() donde se hace la configuración inicial de la placa de arduino
void setup() {
  Serial.begin(9600);
  iniciarSensorUltraSonico();
  iniciarServoMotor();
  iniciarlogicaDifusa();
}

// funcion loop() ciclo infinito que repite las instrucciones necesarias para ejecutar el programa
void loop() {
  //se crea la variable "posicion_bola" en la que se almacena la distancia en la que esta la bola del sensor ultrasonico
  float posicion_bola = calcularDistancia();

  //se agregan las entradas al sistema difuso (la posicion de la bola y el punto de referencia)
  fuzzyObj->setInput(1,posicion_bola);
  fuzzyObj->setInput(2,referencia);

  //se realiza la fusificación
  fuzzyObj->fuzzify();

  //se hace la defusificación y se asigna el valor a la variable "anguloSalida"
  float anguloSalida = fuzzyObj->defuzzify(1);

  //se hace uso de la funcion registro() para imprimir a través del serial
  registro(anguloSalida);

  if(moveMotor){
    moveServo(anguloSalida);
  }

  delay(velocidadReacion);
}
