#include <reg51.h>
#include <stdio.h>
//DIP LSB is Pin1, 
//---------------------------------------
//---------------------------------------
//Libraries//////////////////////////////////////
void BCD(void); //Display to BCD
void display_RPM(void); //Determining if the speed is 1 digit 2 digit 3digits etc
void switchdisplay1(void); //Only Left display is on
void switchdisplay2(void); //Only Middle display is on
void switchdisplay3(void); //Only Right display is on
extern void Timer1(unsigned int x);

//Libraries End//////////////////////////////////
//7 segment display Config
sbit BCDPin0 = P1^0; //7 segment display Config
sbit BCDPin1 = P1^1;
sbit BCDPin2 = P1^2;
sbit BCDPin3 = P1^3;
sbit BCDPin4 = P1^4;
sbit BCDPin5 = P1^5;
sbit BCDPin6 = P1^6;
sbit BCDPin7 = P1^7;//This turns on if the number is >999, off if <999, this is for indicating if the number is 4 digits or 3 digits

sbit speedcheck = P2^7;

sbit inputBit1 = P2^0;//Input from DIP
sbit inputBit2 = P2^1;//Input from DIP
sbit inputBit3 = P2^2;//Input from DIP

sbit BCDControl1 = P2^3; //These are for Controlling the BCD displays sequence,Left to right
sbit BCDControl2 = P2^4;
sbit BCDControl3 = P2^5;

sbit feedback = P3^2; //Input for bluecable feedback, timer 1 external input, int0, set ex0 or ie0 =1
sbit PWMPin = P2^6;
//--------------------------------------------
//staticc vairables
static int BCDnumber;
static int BCDnum1;
static int BCDnum2;
static int BCDnum3;
static int Ton;
static int TonTemp;
static int Toff;
static int speed; //speed is calculated as the RPM, 0<RPM<9999
static int wanted_speed;
static int edge_count;
static int array[3];
static int n;
static int t; //Time of each interruptt to calculate no. of edges
static int loop_count;
static int error;
static int k = 10; //P control coefficient, larger the better but it is trial and error, the best k value can be found by determining the response time


//Timer/Counter/Interrupt Libraries------------------------------------------------
void ISR_ex0(void) interrupt 0{//Basic External interruptt function, counts up the edge count while feedback edge hits
	edge_count++;
}

void Timer0(void) interrupt 1{ //Timer 0 for calculating no. of edges	
	     //Freqeuncy : 1154Hz, 866ms
					if(loop_count < 11){//So its timer0 interruptt*11 = period of interruptt is called
						loop_count++;
					}
					else{
					//speedcheck=~speedcheck;
					TH0 = 0xFD; //50ms*11
					TL0 = 0x4B;
					TR0 = 1;
					//TF0 = 0; // Reset overflow flag
					t = 60*(1/2)/(866*1000); //250ms
					speed = edge_count*37;
					edge_count = 0;
					array[0] = array[1];
					array[1] = array[2];
					array[2] = speed;
					speed = (array[0]+array[1]+array[2])/3;
					//TR0 = 0 ;
					TF0 = 0;
					loop_count = 0;
					}
}//function end

 void Timer1(unsigned int x) {//Timer 1** 8bit mode, for PWM delay
		int i=0;
	  //x=0;
	 
		for(i=0;i<x;i++){
		//FFFF-FFF0 = F, = 16 decimal.
		//(16+1)*1.085us = us delay
		TH1 = 0xFF;
		TR1 = 1; //Turn on timer 
		while (TF1 == 0);//Wait for timer to run til overflow flag is 1
		TF1 = 0; // Reset overflow flag
		TR1 = 0; //Turn off timer
		}
}

void Proportional_control(){
		if(error<1000 && error > -1000 ){
		//Do nothing
		//10RPM of threshold
		}
		else if(wanted_speed > speed){
		Ton = Ton + (error/2400)*k;
		Toff = Toff - (error/2400)*k;
		}
		else{//wanted_speed 
		Ton = Ton - (error/2400)*k;
		Toff = Toff + (error/2400)*k;
		}
}

//Timer Libraries End------------------------------------------------
//---------------------------------------
void main(void){
	//External Interrupt
	EA = 1; //Enable global interruptt flag
	EX0 = 1; //Enalbe the EX0 interruptt
	IT0 = 1; //Configure interrupt 0 for falling edge on INT0
	//Interrupt-------------------------------------------------------------
	TMOD = 0x21; //Set Timer 1 to be mode 2, Timer 0 mode 1
	TH0 = 0xFD; //
	TL0 = 0x4B;
	//t = 2*1.085/1000;
	TR0 = 1; //Turn on timer 0
	TR1 = 1; //Turn on timer 1
	ET0 = 1; //Enable Interrupt for timer 0
	//Variables Declaration
	n=0;
	while(1){
		//speedcheck = 0 ;
					//DIP input 
					if (inputBit1 == 0 && inputBit2 == 0 && inputBit3 == 0){
					Ton = 100;
					Toff = 0;
					
					}
					else if(inputBit1 == 0 && inputBit2 == 0 && inputBit3 == 1){
					Ton = 90;
					Toff = 10; 
					}
					else if(inputBit1 == 0 && inputBit2 == 1 && inputBit3 == 0){
					Ton = 70;
					Toff = 30;
					}
					else if(inputBit1 == 0 && inputBit2 == 1 && inputBit3 == 1){
					Ton = 60;
					Toff = 40;
					}
					else if(inputBit1 == 1 && inputBit2 == 0 && inputBit3 == 0){
					Ton = 50;
					Toff = 50;
					}
					else if(inputBit1 == 1 && inputBit2 == 0 && inputBit3 == 1){
					Ton = 30;
					Toff = 70;
					}
					else if(inputBit1 == 1 && inputBit2 == 1 && inputBit3 == 0){
					Ton = 20;
					Toff = 80;
					}
					else {
					Ton = 0;
					Toff = 100;
					}
					wanted_speed = 2400*Toff/100;
					error = wanted_speed - speed;
					Proportional_control();
					
					if(Ton >= 100){
					PWMPin = 1;
					}
					else if(Ton <= 0){
					PWMPin = 0;
					}
					else {
					PWMPin=1;
					Timer1(Ton);
					PWMPin=0;
					Timer1(Toff);
					}
					display_RPM();
					
					//Just for checking the feedback speed
					
}//While end
}//Main end


void BCD(){
	
	//BCD is Common  anode so negative logic, * is 1
	if(BCDnumber == 0){
		BCDPin0 = 0;
		BCDPin1 = 0;
		BCDPin2 = 0;
		BCDPin3 = 0;
		BCDPin4 = 0;
		BCDPin5 = 0;
		BCDPin6 = 1;
	}
	else if(BCDnumber == 1){
		BCDPin0 = 1;
		BCDPin1 = 0;
		BCDPin2 = 0;
		BCDPin3 = 1;
		BCDPin4 = 1;
		BCDPin5 = 1;
		BCDPin6 = 1;
	}
 	else if(BCDnumber == 2){
		BCDPin0 = 0;
		BCDPin1 = 0;
		BCDPin2 = 1;
		BCDPin3 = 0;
		BCDPin4 = 0;
		BCDPin5 = 1;
		BCDPin6 = 0;
	}
	else if(BCDnumber == 3){
		BCDPin0 = 0;
		BCDPin1 = 0;
		BCDPin2 = 0;
		BCDPin3 = 0;
		BCDPin4 = 1;
		BCDPin5 = 1;
		BCDPin6 = 0;
	}
	else if(BCDnumber == 4){
		BCDPin0 = 1;
		BCDPin1 = 0;
		BCDPin2 = 0;
		BCDPin3 = 1;
		BCDPin4 = 1;
		BCDPin5 = 0;
		BCDPin6 = 0;
	}
	else if(BCDnumber == 5){
		BCDPin0 = 0;
		BCDPin1 = 1;
		BCDPin2 = 0;
		BCDPin3 = 0;
		BCDPin4 = 1;
		BCDPin5 = 0;
		BCDPin6 = 0;
	}
	else if(BCDnumber == 6){
		BCDPin0 = 0;
		BCDPin1 = 1;
		BCDPin2 = 0;
		BCDPin3 = 0;
		BCDPin4 = 0;
		BCDPin5 = 0;
		BCDPin6 = 0;
	}
	else if(BCDnumber == 7){
		BCDPin0 = 0;
		BCDPin1 = 0;
		BCDPin2 = 0;
		BCDPin3 = 1;
		BCDPin4 = 1;
		BCDPin5 = 1;
		BCDPin6 = 1;
	}
	else if(BCDnumber == 8){
		BCDPin0 = 0;
		BCDPin1 = 0;
		BCDPin2 = 0;
		BCDPin3 = 0;
		BCDPin4 = 0;
		BCDPin5 = 0;
		BCDPin6 = 0;
	}
	else{
		BCDPin0 = 0;
		BCDPin1 = 0;
		BCDPin2 = 0;
		BCDPin3 = 0;
		BCDPin4 = 1;
		BCDPin5 = 0;
		BCDPin6 = 0;
	}
//genericdelay(50);
}

void display_RPM (){	
int temp;
	if(speed >999){
	BCDPin7 = 0;
	}
	else{
	BCDPin7 = 1;
	}
	if (speed>=0 && speed<10){ 
		BCDnumber = 0;
		switchdisplay1();
		BCDnumber = 0;
		switchdisplay2();
		BCDnumber = speed;
		switchdisplay3();
	}

	else if (speed>=10 && speed<100){ //10<RPM<100
		BCDnumber = 0;
		switchdisplay1();
		BCDnumber = speed/10;
		switchdisplay2();
		BCDnumber = speed%10;
		switchdisplay3();
	}
	else if (speed>=100 && speed<1000){ //100<RPM<1000
		temp = speed/10;
		BCDnumber = temp/10;
		switchdisplay1();
		BCDnumber = temp%10;
		switchdisplay2();
		BCDnumber = speed%10;
		switchdisplay3();
	}
	else if(speed>1000 && speed<9999){
		BCDnumber = speed/1000;
		switchdisplay1();
		BCDnumber = (speed%1000)/100;
		switchdisplay2();
		BCDnumber = (speed%100)/10;
		switchdisplay3();
	}
	else{
		BCDPin0 = 1;
		BCDPin1 = 1;
		BCDPin2 = 1;
		BCDPin3 = 1;
		BCDPin4 = 1;
		BCDPin5 = 1;
		BCDPin6 = 1;
		switchdisplay1();
		switchdisplay2();
		switchdisplay3();
	}
}

void switchdisplay1(){
	BCDControl1 = 1;
	BCDControl2 = 0;
	BCDControl3 = 0;
	BCD();
	Timer1(5);
}

void switchdisplay2(){
	BCDControl1 = 0;
	BCDControl2 = 1;
	BCDControl3 = 0;
	BCD();
	Timer1(5);
}

void switchdisplay3(){
	BCDControl1 = 0;
	BCDControl2 = 0;
	BCDControl3 = 1;
	BCD();
	Timer1(5);
}
