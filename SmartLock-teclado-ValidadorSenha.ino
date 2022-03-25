/* 28-02-22
 *  Projeto Market Self
 *  Empresa: Start704
 *  Autor: Heghbertho G Costa - 85-992850808
 *  Melhorias no código, identificação de leds, uso de portas digitais
 *  Introdução de leitura pelo SD
 *########################### 02-03-22 
 * Incluir módulo Relógio - falha
   *########################### 24-03-22 
 * Retirada de leitura via sd, usando formula de leutura de código
 * 
*/

#include <Keypad.h> //INCLUSÃO DE BIBLIOTECA
#include <SD.h>
#include <SPI.h>


///##################################################################################################### Variáveis e constantes 

const byte qtdLinhas = 4; //QUANTIDADE DE LINHAS DO TECLADO
const byte qtdColunas = 3; //QUANTIDADE DE COLUNAS DO TECLADO
//CONSTRUÇÃO DA MATRIZ DE CARACTERES
char matriz_teclas[qtdLinhas][qtdColunas] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte PinosqtdLinhas[qtdLinhas] = {3, 4, 5, 6}; //PINOS UTILIZADOS PELAS LINHAS
byte PinosqtdColunas[qtdColunas] = {8, 9, 7}; //PINOS UTILIZADOS PELAS COLUNAS (original pino 7 seria 10, porém arduino com porta defeituosa)
//INICIALIZAÇÃO DO TECLADO
Keypad meuteclado = Keypad( makeKeymap(matriz_teclas), PinosqtdLinhas, PinosqtdColunas, qtdLinhas, qtdColunas); 

const int buzzer = A3;
const int RelePin = A4;
const int ledAmarelo = A0;
const int ledVermelho = A1;
const int ledVerde = A2;
const int pinoSS = 2;


String digitada;
int estado=0; 
String codigo; 
byte byteRead;
File myFile;
String buffer;
String listaArquivoSD = "id.txt";
String fileName = "registro.txt";
int portaAberta = 5; // Aproximadamente 15 segundos (1000 + 700) miselimos
bool resultado;
bool retorno;

//##################################################################################################### Funções 

void (*funcReset)() = 0;

  // Testa SD
 bool testaSD(){
    bool resultado=false;
    if (SD.begin()) {
      resultado=true;
    } else{
      resultado=false;
    }
   return resultado;
  }
  

  void ListaSD(String arquivoSD){
    myFile = SD.open(arquivoSD); // Abre o Arquivo
  if (myFile) {
     // Serial.println("Conteúdo do SD");
      while (myFile.available()) {
        buffer = myFile.readStringUntil('\n');
        Serial.println(buffer);
      }
      
      myFile.close();
    } else {
      Serial.println("error opening .txt");
    }
  }

  void gravaRegistro(String txt){
    SD.begin();
    
    myFile = SD.open(fileName, FILE_WRITE); // Cria / Abre arquivo .txt
    if(!SD.begin() or !myFile){
      Serial.print("Erro do SD, reiniciando sistema");
      funcReset();
    }
    
    if (myFile) { // Se o Arquivo abrir imprime:
      myFile.println("Registro Card: " + txt); // Escreve no Arquivo
      delay(50);
      myFile.close(); // Fecha o Arquivo após escrever
      Serial.println("Gravando: Registro Card "+ txt); // Imprime na tela
      delay(100);
      return;
    }else {     // Se o Arquivo não abrir
      Serial.println("Erro ao Abrir Arquivo ----" + fileName); // Imprime na tela
    }
  }






  


 
void setup(){
  Serial.begin(9600); 
  pinMode(buzzer, OUTPUT);
  pinMode(pinoSS, OUTPUT);
  pinMode(RelePin, OUTPUT); 
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);

  Serial.println(F("Aperte uma tecla...")); 
  Serial.println(); 
  digitalWrite(ledAmarelo, HIGH);
  beep1();


  if(testaSD()){
    Serial.println(F("SD Card pronto para uso.")); // Imprime na tela
    Serial.println(F("Lista de IDs usados com Sucesso")); // Imprime na tela
    ListaSD("registro.txt");
    beep0();
  }else{
    Serial.println(F("Falha na inicialização do SD Card."));
    digitalWrite(ledVermelho, HIGH);
    digitalWrite(ledAmarelo, HIGH);
    digitalWrite(ledVerde, HIGH);
    beep3();
    digitalWrite(ledVermelho, LOW);
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVerde, LOW);
    delay(100);
    beep3();    
    digitalWrite(ledAmarelo, HIGH);
  }

  

 
}


void loop(){

  
  char tecla = meuteclado.getKey(); //VERIFICA SE ALGUMA DAS TECLAS FOI PRESSIONADA - teclado 
  if(tecla != NO_KEY){
  digitalWrite(ledAmarelo, HIGH);
    
  //Serial.println(tecla);
    estado=1;
    if(tecla=='#'){
      beep0();
      digitalWrite(ledAmarelo, LOW);     
      Serial.println(digitada);
      
      if(Verifica(digitada)){
        gravaRegistro("Gravando o id:" + digitada );
        estado=3;
        leds(estado);
        estado=0;
      }else{
        estado=2;
        leds(estado);
        delay(500);
        estado=0;
      }
      digitada="";
    }else{
      digitada+=tecla;
    }
    leds(estado);
  }
  
}

void leds(int e){ //0=Aguardando, 1=Digitando, 2=Negado, 3=Aceito
  if(e==0){
    Serial.println(F("Aguardando"));
    digitalWrite(ledAmarelo, HIGH);
  }else if(e==1){
    //Serial.println(F("Digitando "));
    tone(buzzer,449,100);
    //delay(100);
  }else if(e==2){
    Serial.println(F("Negado"));
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVermelho, HIGH);
    digitalWrite(RelePin,LOW);
    beep2();
    delay(2000);
    digitalWrite(ledVermelho, LOW);
  }else if(e==3){
    Serial.println(F("Aceito"));
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVerde, HIGH);
    digitalWrite(RelePin,HIGH);
    beep4();
    digitalWrite(ledVerde, LOW);
    digitalWrite(RelePin,LOW);
    
  }
}



void beep0()
{
     tone(buzzer,500,100);  
}
void beep1()
{
     tone(buzzer,400,100);  
}

void beep2()
{
     tone(buzzer,1000,500); 
     delay(100);
     tone(buzzer,1000,500);  
     delay(100);
     tone(buzzer,1000,500);  
}


void beep3()
{
     tone(buzzer,800,300);  
     delay(100);
     tone(buzzer,800,300); 
     delay(100);
     tone(buzzer,800,300);  
}

void beep4(){
  for(int i=0;i<portaAberta;i++){
    tone(buzzer,3500,700);  
    delay(1000); 
  }
   for(int i=0;i<portaAberta-1;i++){
    tone(buzzer,3500,500);  
    delay(700); 
  }
   for(int i=0;i<portaAberta;i++){
    tone(buzzer,3500,500);  
    delay(200); 
  }
}


bool Verifica(String id){
   /*
   * cadeia exata de 6 dígitos
   * divide-se cada número (tipo split)
   * cálculo:
   *    (a) = coma-se o priemiro ao quinto número
   *    (b) = multiplica-se o primeiro numero pelo sexto número
   *    (c) = soma-se (A) + (B)  
   *    Faz-se a prova dos nove
   *      (C) - 9 = resultado, repete-se até dar zero ou menor que zero.
   *      caso de zero esta validado a operação com resultado true, qualquer outro número é resultado falso
   */
  int soma = 0;
  retorno = false;

  if(id.length() != 6 or id == "147258"){
   Serial.println("Maior ou Menor que 6 ou difitado número bloqueado");
   return retorno;
  }

  soma = id.substring(0,1).toInt() + id.substring(1,2).toInt() + id.substring(2,3).toInt() + id.substring(3,4).toInt() + id.substring(4,5).toInt() ;
  soma += id.substring(5,6).toInt() * id.substring(0,1).toInt();
 
 // Serial.println("Soma: "+String(soma));

  while(1){
     // Serial.print("Soma :" +String(soma));
      soma = soma - 9;
     // Serial.println(" -9 = "+String(soma));    
      
      if(soma <= 0){
        if(soma == 0){
          retorno = true;
        }
        return retorno;
        break;
      }
     
  }
  
}



  
