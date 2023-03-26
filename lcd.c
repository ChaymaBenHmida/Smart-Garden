sbit LCD_RS at RD0_bit;
sbit LCD_EN at RD1_bit;
sbit LCD_D7 at RD2_bit;
sbit LCD_D6 at RD3_bit;
sbit LCD_D5 at RD4_bit;
sbit LCD_D4 at RD5_bit;

sbit LCD_RS_Direction at TRISD0_bit;
sbit LCD_EN_Direction at TRISD1_bit;
sbit LCD_D7_Direction at TRISD2_bit;
sbit LCD_D6_Direction at TRISD3_bit;
sbit LCD_D5_Direction at TRISD4_bit;
sbit LCD_D4_Direction at TRISD5_bit;

char panneStr[15];
int hum;
int testArrosage=0;
int finArrosageClrCount=0;
int arrosageClrCount=0;
int timerBippeur=4577;
int timerConsulter=915;
unsigned int nb_panne=0;
int startPanne=0;
int pluieClrCount=0;
int consulterTest=0;
int consulterClrCount=0;
int testFinArrosage=0;
      int pluie=0;

void arrosage(){
     finArrosageClrCount=0;

     if(arrosageClrCount==0){
        Lcd_Cmd(_LCD_CLEAR);
     }
     Lcd_out(1,2,"Arrosage gazon");
     portd.rd6=1;
     arrosageClrCount=1;
}

void finArrosage(){
     arrosageClrCount=0;

     if(finArrosageClrCount==0){
        Lcd_Cmd(_LCD_CLEAR);
     }
     Lcd_out(1,3,"Fin arrosage");
     Lcd_out(2,6,"gazon");
     portd.rd6=0;
     finArrosageClrCount=1;
}

void panne(){
     Lcd_out(1,1,"Systeme en panne");
     portd.rd6=0;
}

void bipeur(){
     portd.rd7=1;
     delay_ms(500);
     portd.rd7=0;
     delay_ms(500);
     portd.rd6=0;
}

void pluieFn(){
     if(pluieClrCount==0){
        Lcd_Cmd(_LCD_CLEAR);
     }
     Lcd_out(1,5,"PLUIE");
     portd.rd6=0;
     pluieClrCount=1;
}

void consulter(){
     nb_panne = EEPROM_Read(0x02);
     if(consulterClrCount==0){
        Lcd_Cmd(_LCD_CLEAR);
     }
     IntToStr(nb_panne, panneStr);
     Lcd_out(1,3,"nb_panne:");
     Lcd_out(2,3,panneStr);
     consulterClrCount=1;
     portd.rd6=0;
}

void readHumidity()
{
    hum= adc_read(0);
    if(hum>=23 && hum<=66){
       hum=hum-1;
    }else if(hum>=67){
       hum=hum-2;
    }
}

void interrupt()
{
    if((INTCON.RBIF)&&(INTCON.RBIE)){
        if(portb.RB4==1){
           testArrosage=1;
           portd.rd6=1;
        }
        
        if(portb.rb4==0){
           testArrosage=0;
           portd.rd6=0;
           //finArrosageClrCount=1;
        }
        
        if(portb.RB5==1){
           consulterTest=1;
           TMR0= 0;
           timerConsulter=915; //1 minute
        }
        
        if(portb.RB5==0){
           consulterTest=0;
        }
        
        intcon.RBIF=0;

    }

    if((INTCON.INTF)&&(INTCON.INTE))
    {
        if(portb.rb0==1){
             portd.rd6=0;
        }else{
               intcon.INTF=0;
        }

    }


    if(INTCON.T0IF){
        TMR0= 0;
        if(timerBippeur>=0){
           timerBippeur--;
        }
        if(timerConsulter>=0){
           timerConsulter--;
        }
        INTCON.T0IF=0;
    }
}

void main(){
     trisD=0;
     portD=0;
     TRISB.RB0=1;
     TRISB.RB4=1;
     TRISB.RB5=1;


     INTCON.GIE=1; //registre global des interruptions  flagbit7
     INTCON.RBIE=1;                    //registre du portb   flagbit3
     INTCON.INTE=1;           //rb0            flagbit4
     OPTION_REG.INTEDG=1;          //rb0 montant mode compteur  flagbit6
     //timer0config
     TMR0= 0;
     INTCON.T0IE=1;            //config interruption global       bit 5
     OPTION_REG.T0CS=0;        //config interruption timer  bit 5
     OPTION_REG.T0SE=0;         //config interrup timer mode compteur   bit 5
     OPTION_REG.PSA=0;                  //prédivisuer
     OPTION_REG.PS2=1;
     OPTION_REG.PS1=1;
     OPTION_REG.PS0=1;
     //
     EEPROM_Write(0x02,nb_panne);
     Lcd_Init();
     Lcd_Cmd(_LCD_CURSOR_OFF);
     Lcd_out(1,5,"Welcome");
     delay_ms(900);
     Lcd_Cmd(_LCD_CLEAR);
     Lcd_out(1,3,"SMART GARDEN");
     Lcd_out(2,3,"By 2A5");
     delay_ms(1200);
     Lcd_Cmd(_LCD_CLEAR);
     ADC_init();

     do{
                readHumidity();

                if(finArrosageClrCount==0 && hum>28){
                   arrosage();
                }

                //
                if(testArrosage==1){
                    arrosage();
                }else{
                   if(testFinArrosage == 1){
                       finArrosage();
                   }
                
                   if(consulterTest == 1){
                       while(timerConsulter>0){
                           consulter();
                           if(timerConsulter<=0){
                              Lcd_Cmd(_LCD_CLEAR);
                           }
                       }
                   }else{
                      consulterClrCount=0;

                      if(hum<=30 && hum>28){
                          arrosage();
                          testFinArrosage=0;
                      }else if(hum>=50){
                          finArrosage();
                          testFinArrosage=1;
                      }

                      if(hum<29){
                          panne();
                          if(startPanne==0){
                             timerBippeur=4577; //5 minute
                             nb_panne++;
                             EEPROM_Write(0x02,nb_panne);
                          }
                          startPanne=1;
                      }else{
                          startPanne=0;
                      }

                      while(hum<29 && timerBippeur>0){
                          readHumidity();
                          bipeur();

                          if(hum>=29 || timerBippeur<=0){
                              Lcd_Cmd(_LCD_CLEAR);
                          }
                      }

                    }
                }

     }while(1);

}