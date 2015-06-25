/*
Date: 25/06/2015 - 17:43
Professor: Cesar Alberto Bravo Pariente
Student: Giovanne Almeida Messias

References:
-> Wikipedia at <https://en.wikipedia.org/?title=P-code_machine>
-> Black Mesa Tech at <http://blackmesatech.com/2011/12/pl0/pl0.xhtml>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STACK_SIZE 500
#define MAX_LEVELS 3 //For this implementation, only one level will be used

// Types
typedef struct{
	int f;
	int l;
	int a;

} INSTRUCTION;


//Variables
int p=0, b=0, t=0; //program-register, base-register and topstack-register
INSTRUCTION i; //Current intruction
int s[STACK_SIZE]; //Stack
INSTRUCTION code[STACK_SIZE]; //Do not declared on reference. Section "Example Machine" at <https://en.wikipedia.org/?title=P-code_machine>

FILE *sample, *out;
int operandLeft, operandRight, op;
int i_line=0; //Line counter
int is_binary = 1; //Whether operation is binary (most operations are binary, that's why ut has 1 as default value

enum FCT {LIT, OPR, LOD, STO, CAL, INT, JMP, JPC};
enum OPERATORS {RET, NEG, ADD, SUB, MUL, DIV, ODD, MOD, EQL, NEQ, LSS, LEQ, GTR, GEQ};

//Functions prototypes
int load_sample(char[]); //Loads the sample file
int create_out_file(char[]); //Creates the out file with results
void start_machine(void); //Starts the P-Code Machine
void execute(void); //Starts the execution of a sample
void execute_instruction(void); //Executes the current instruction
void print_stack(void); //Prints the relevant part of the stack

//Main
int main (int argc, char *argv []){

	if(!load_sample(argv[1])){
		printf("\nSample file not found.\nDid you type the correct name of the file? (e.g. \"sample-logic.txt\")\n\n");
		exit(EXIT_FAILURE);
	}
	
	if(!create_out_file(argv[1])){
		printf("Could not create the out file.\n\n");
		exit(EXIT_FAILURE);
	}
	
	start_machine();
	
	printf("\nDone!\n");

	return 0;
}

//Functions declarations
int load_sample(char *file){
	if(sample = fopen(file,"r")){
		return 1;
	}
	return 0;
}

int create_out_file(char *file){
	char out_name[100] = {"out-"};
	strcat(out_name, file);
	
	if(out = fopen(out_name,"w")){
		return 1;
	}
	return 0;
}

void start_machine(){
	char polishAux, opAux, opAux2='\0';
	int is_polish;
	
	//Setting values
	while (fscanf(sample, "%c", &polishAux) != EOF){
	
		//Reseting variables		
		t = 0; b = 1; p = 0;
		s[1] = 0; s[2] = 0; s[3] = 0;
		is_polish = 0;
		is_binary = 1;
		i_line++;
		
		if(polishAux == '\n'){ //Skipping line break
			fscanf(sample, "%c", &polishAux);
		}
		
		if(!isdigit(polishAux)){ //Polish notation detection (+ a b)
			is_polish = 1;
			switch(polishAux){
				case '+':
					op = ADD;
					break;
				case '-':
					op = SUB;
					break;
				case '*':
					op = MUL;
					break;
				case '/':
					op = DIV;
					break;
			}
			
			fscanf(sample, "%c", &opAux); //Blank space
			fscanf(sample, "%d", &operandLeft);
			fscanf(sample, "%c", &opAux); //Blank space
			fscanf(sample, "%d", &operandRight);
		} else {
			operandLeft = polishAux - '0'; //Converts the number in polishAux to int and sets into operandLeft
			fscanf(sample,  "%c", &opAux);
			switch(opAux){
				case '+':
					op = ADD;
					break;
				case '-':
					op = SUB;
					break;
				case '*':
					op = MUL;
					break;
				case '/':
					op = DIV;
					break;
				case '>':
					fscanf(sample, "%c", &opAux2);
					if(opAux2 == '='){ //Detecting >= (GEQ) operator
						op = GEQ;
					} else {
						op = GTR;
					}
					break;
				case '<':
					fscanf(sample, "%c", &opAux2);
					if(opAux2 == '='){ //Detecting '<=' (LEQ) operator
						op = LEQ;
					} else {
						if(opAux2 == '>'){ //Detecting '<>' (NEQ) operator
							op = NEQ;
						} else {
							if(isdigit(opAux2)){
								op = LSS;
							} else {
								printf("\nSyntax error at line %d column 3!\n\nDid you mean one of these?\n a<=b (Less than or equal to)\n a<>b (Not equal to)\n",i_line);
								exit(EXIT_FAILURE);
							}
						}
					}
					break;
				case '=':
					op = EQL;
					break;
				case 'O':
					fscanf(sample, "%c", &opAux2);
					if(opAux2 == 'D'){
						fscanf(sample, "%c", &opAux2);
						if(opAux2 == 'D'){
							op = ODD;
							is_binary = 0;
						} else {
							printf("\nSyntax error at line %d column 4!\n\nDid you mean aODD (is a ODD?)?\n",i_line);
							exit(EXIT_FAILURE);
						}
					} else {
						printf("\nSyntax error at line %d column 3!\n\nDid you mean aODD (is a ODD?)?\n",i_line);
						exit(EXIT_FAILURE);
					}
					break;
			}
			
			if((op == LSS)||(op == GTR)){ //If op is LSS or GTR, opAux2 must be a number
				operandRight = opAux2 - '0'; //Converts the number in opAux2 to int and sets into operandRight
			} else {
				if((op != ODD)){ //If op is not LSS, GTR or ODD, the right operand is unknown...
					fscanf(sample, "%d", &operandRight);
				}
			}
		}
			
			//Setting how the result must be written depending on the analysis type
			if(is_polish){
				fprintf(out,"Analyzing: %c %d %d --------------------- \n\n",polishAux, operandLeft, operandRight);
			} else {
				switch(opAux){
					case 'O': //Analyzing whether is ODD
						fprintf(out, "Analyzing: %dODD --------------------- \n\n", operandLeft);
						break;
					
					case '<': 
						if(opAux2 == '=' || opAux2 == '>'){
							fprintf(out,"Analyzing: %d%c%c%d --------------------- \n\n", operandLeft, opAux, opAux2, operandRight);
						} else {
							fprintf(out,"Analyzing: %d%c%d --------------------- \n\n", operandLeft, opAux, operandRight);
						}
						break;
						
					case '>':
						if(opAux2 == '='){
							fprintf(out,"Analyzing: %d%c%c%d --------------------- \n\n", operandLeft, opAux, opAux2, operandRight);
						} else {
							fprintf(out,"Analyzing: %d%c%d --------------------- \n\n", operandLeft, opAux, operandRight);
						}
						break;
						
					default:
						fprintf(out,"Analyzing: %d%c%d --------------------- \n\n", operandLeft, opAux, operandRight);
				}
			}
		execute();
   }
}

void execute(){
	//Push the operandLeft into the stack
	code[0].f = LIT;
	code[0].l = 0;
	code[0].a = operandLeft;
	
	execute_instruction();
	
	code[1].f = STO;
	code[1].l = 0;
	code[1].a = 3;
	
	execute_instruction();
	
	if(is_binary){
		code[2].f = LIT;
		code[2].l = 0;
		code[2].a = operandRight;
		
		execute_instruction();
		
		code[3].f = STO;
		code[3].l = 0;
		code[3].a = 4;
		
		execute_instruction();
		
		code[4].f = LOD;
		code[4].l = 0;
		code[4].a = 3;
		
		execute_instruction();
		
		code[5].f = LOD;
		code[5].l = 0;
		code[5].a = 4;
		
		execute_instruction();
		
		code[6].f = OPR;
		code[6].l = 0;
		switch(op){
			case ADD:
				code[6].a = ADD;
				break;
			
			case SUB:
				code[6].a = SUB;
				break;
			
			case MUL:
				code[6].a = MUL;
				break;
			
			case DIV:
				code[6].a = DIV;
				break;
				
			case EQL:
				code[6].a = EQL;
				break;
			
			case NEQ:
				code[6].a = NEQ;
				break;
			
			case LSS:
				code[6].a = LSS;
				break;
				
			case LEQ:
				code[6].a = LEQ;
				break;
				
			case GTR:
				code[6].a = GTR;
				break;
			
			case GEQ:
				code[6].a = GEQ;
				break;
		}
		
	} else {
		code[2].f = LOD;
		code[2].l = 0;
		code[2].a = 3;
		
		execute_instruction();
		
		code[3].f = OPR;
		code[3].l = 0;
		switch(op){
			case ODD:
				code[3].a = ODD;
				break;
				
			case MOD:
				code[3].a = MOD;
				break;
		}
	}
	
	execute_instruction();
	print_stack();
}

void write_operation(){
	
	fprintf(out,"%d.",p+1);
	switch(i.f){
		case LIT:
			fprintf(out," LIT ");
			break;
		
		case OPR:
			fprintf(out," OPR ");
			break;
		
		case LOD:
			fprintf(out," LOD ");
			break;
			
		case STO:
			fprintf(out," STO ");
			break;
	}
	fprintf(out,"%d %d\n",i.l,i.a);
}

void execute_instruction(){
	
	
	i = code[p];
	
	write_operation();
	
	p++;
	switch(i.f){
		case LIT: //t := t + 1; s[t] := a
			t++;
			s[t] = i.a;
			break;
		
		case OPR: //If i.op is an operatar, we must find what operation is about to happen
			switch(i.a){
				case RET: //t := b - 1; p := s[t + 3]; b := s[t + 2];
					t = b-1;
					p = s[t + 3];
					b = s[t + 2];
					break;
				
				case NEG: //s[t] := -s[t]
					s[t] = -s[t];
					break;
				
				case ADD: //t := t - 1; s[t] := s[t] + s[t + 1]
					t--;
					s[t] = s[t] + s[t + 1];
					break;
				
				case SUB: //t := t - 1; s[t] := s[t] - s[t + 1]
					t--;
					s[t] = s[t] - s[t + 1];
					break;
				
				case MUL: //t := t - 1; s[t] := s[t] * s[t + 1]
					t--;
					s[t] = s[t] * s[t + 1];
					break;
				
				case DIV: //t := t - 1; s[t] := s[t] div s[t + 1]
					t--;
					s[t] = s[t] / s[t + 1];
					break;
				
				case ODD: //s[t] := ord(odd(s[t])) ???				
					s[t] = ((s[t] % 2) ? 1 : 0);
					break;
				
				case MOD: //Not implemented on reference at <https://en.wikipedia.org/?title=P-code_machine>
					s[t] = abs(s[t]);
					break;
				
				case EQL: //t := t - 1; s[t] := ord(s[t] = s[t + 1])
					t--;
					s[t] = (s[t] == s[t + 1]);
					break;
				
				case NEQ: //t := t - 1; s[t] := ord(s[t] <> s[t + 1])
					t--;
					s[t] = (s[t] != s[t + 1]);
					break;
				
				case LSS: //t := t - 1; s[t] := ord(s[t] < s[t + 1])
					t--;
					s[t] = (s[t] < s[t + 1]);
					break;
				
				case LEQ: //t := t - 1; s[t] := ord(s[t] <= s[t + 1])
					t--;
					s[t] = (s[t] <= s[t + 1]);
					break;
				
				case GTR: //t := t - 1; s[t] := ord(s[t] > s[t + 1])
					t--;
					s[t] = (s[t] > s[t + 1]);
					break;
				
				case GEQ: //t := t - 1; s[t] := ord(s[t] >= s[t + 1])
					t--;
					s[t] = (s[t] >= s[t + 1]);
					break;
			}
			break;
		
		case LOD: //t := t + 1; s[t] := s[base(l) + a]
			t++;
			s[t] = s[base(i.l) + i.a];
			break;
		
		case STO: //s[base(l)+a] := s[t]; writeln(s[t]); t := t - 1
			s[base(i.l) + i.a] = s[t];
			t--;
			break;
	} //TODO: {CAL, INT, JMP, JPC};
}

int base(int l) {
   int b1 = b;
   while(l > 0){
      b1 = s[b1];
      l--;
   }
   return b1;
}

void print_stack(){
	int i;
	fprintf(out,"\ns|");
	for(i=1;i<p;i++){
		fprintf(out,"%d|",s[i]);
	}
	fprintf(out,"\n_______________________________________________\n\n");
}
