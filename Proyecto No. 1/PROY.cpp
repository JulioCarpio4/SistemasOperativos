#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h> 
#include <time.h>
#include <iostream.h>
#include <pcb.h>
#include <string.h>

#define INTR 0x08    /* The clock tick

			interrupt */

#ifdef __cplusplus

    #define __CPPARGS ...

#else

    #define __CPPARGS

#endif

// Declaracion de todas las variables globales y prototipos de los metodos
PCB pcbs[6];
char cadEntrada[57];
int posCadena;
int bandera;
int idProceso;
int mouse_presionado = 0;
int miliSegundos = 0;
void interrupt (*oldhandler)(__CPPARGS);
int MirarPuntero(void);
int Verificaciones();
int MostrarPuntero(void);
int BotonPulsado(void);
int CorHTexto(void);
int CorVTexto(void);
int CalculoPantalla(void);
void MatarProceso(int);
void PausarProceso(int);
void LimpiarPantalla(int);
void ReanudarProceso(int);
void IniciarProceso(int);
void SetQuantum(int, int);
void PintarPunto();
int getNumber(int, int);
int validarEntrada();
double PorcentajeUso(int);
int mPow(int, int);

clock_t TGlobal; 
clock_t TIdle; 

void interrupt handler(__CPPARGS) // si C++, necesita el ellipsis
{
// deshabilita las interrupciones durante la interrupcion del handler
   disable();

// Incrementa el contador global	
	if(idProceso!=-1)
	{
		pcbs[idProceso].realQuantum--;
	}
	
// Rehabilita la interrupcion al fnal del handler
   enable();

// Llama la rutina antigua
   oldhandler();
}

// Metodo Principal
int main (void)
{
	//pcbs[0] = PCB(0);//pcbs[1] = PCB(1);//pcbs[2] = PCB(2);//pcbs[3] = PCB(3);//pcbs[4] = PCB(4);//pcbs[5] = PCB(5);
	idProceso = 0;
	bandera = 1;
	posCadena = 9;
	//clock_t Start = clock();//clock_t Temp = clock();
	
	//Limpia la pantalla, y muestra la pantalla del proyecto
	clrscr();
	_setcursortype(_NOCURSOR);
	MostrarPuntero();

	printf("%s","||================================================================||\n");
	printf("%s","||            PROYECTO NO. 1: SISTEMAS OPERATIVOS                 ||\n");
	printf("%s","||================================================================||\n");
	printf("%s","||                    ||                    ||                    ||\n");
	printf("%s","||                    ||                    ||                    ||\n");
	printf("%s","||                    ||                    ||                    ||\n");
	printf("%s","||                    ||                    ||                    ||\n");
	printf("%s","||                    ||                    ||                    ||\n");
	printf("%s","||                    ||                    ||                    ||\n");
	printf("%s","||====================||====================||====================||\n");
	printf("%s","||                    ||                    ||                    ||\n");
	printf("%s","||                    ||                    ||                    ||\n");
	printf("%s","||                    ||                    ||                    ||\n");
	printf("%s","||                    ||                    ||                    ||\n");
	printf("%s","||                    ||                    ||                    ||\n");
	printf("%s","||                    ||                    ||                    ||\n");
	printf("%s","||================================================================||\n");
	printf("%s","||                                                                ||\n");
	printf("%s","||INPUT:                                                          ||\n");
	printf("%s","||================================================================||\n");
	printf("%s","||                                                                ||\n");
	printf("%s","||OUTPUT:                                                         ||\n");
	printf("%s","||================================================================||\n");

	// guarda la vieja interrupcion del vetor

    oldhandler = getvect(INTR);

	// instala la nueva interrupcion del handler

    setvect(INTR, handler);
	
	// Limpia la cadena de entradas poniendo todos los valores en nulo
	for(int i = 0; i<57; i++)
	{
		cadEntrada[i] = 0;
	}

	TGlobal = clock();

	// Mientras que bandera sea verdadero estara pintando los puntos y verificando si esta escribiendo el usuario
	do
	{
		PintarPunto();		
		
		Verificaciones();			
		
	}while (bandera == 1);

	// Cuando termina regresa el cursos a la normalidad y resetea la antigua interrupcion del handler
	_setcursortype(_NORMALCURSOR);

	// resetea la antigua interrupcion del handler 
   setvect(INTR, oldhandler);

   getch();
   return 0;
}

// Metodo que eleva la base al exponente recibidos por parametro
int mPow(int base, int expon)
{
	int resp = 1;
	
	for(int i = 0; i < expon; i++)
	{
		resp *= base;
	}	
	return resp;
}

// Metodo que obtiene el numero de la cadena de entrada segun la posicion ingresada
// Retorna -2 si es incorrecto
int getNumber(int numInicial, int esAdd)
{
	int contNum = 0, i = 0;
	char numInstr[7];

	miliSegundos = 0;
	
	// Si la palabra es quantum
	if(numInicial == 8)
	{
		numInicial = 0x0A;
		
		for(i = numInicial; i < 28; i++)
		{
			if(cadEntrada[i] >= 0x30 && cadEntrada[i] <= 0x39)
			{
				contNum++;
				numInstr[i - numInicial] = cadEntrada[i];
			}
			else i = 28;
		}
		
		for(i = 0; i < contNum; i++)
		{
			miliSegundos += (numInstr[i] - 0x30)*mPow(10, contNum - i - 1);
		}
		
		if(miliSegundos < 50)
		{
			gotoxy(10,22); printf("Los milisegundos del quantum no deben ser menores a 50");
			return -2;
		}
		
		numInicial = 0x08;
		
		if(cadEntrada[numInicial + 1] != 0x20 && cadEntrada[numInicial + 1] != 0)
		{
			return -2;
		}		
	}
	
	else if(esAdd == 1)
	{
		if(cadEntrada[numInicial] == 0x2D)
		{
			numInicial++;
			if(cadEntrada[numInicial] == 0x31)
				return -1;			
			else 
				return -2;			
		}
	}
	
	if(cadEntrada[numInicial + 1] != 0x20 && cadEntrada[numInicial + 1] != 0)
	{
		return -2;
	}
	
	return (cadEntrada[numInicial] - 0x30);
}

// Metodo que verifica si la entrada ingresada por el usuario es la correcta
int validarEntrada()
{
	// Palabras posibles de entrada: add, pause, play, remove, clear, quantum, stats
	char instrEntrada[8];
	int num = 0;
	
	// Se guarda unicamente la palabra de la instruccion
	for(int i = 0; i < 8; i++)
	{
		if(cadEntrada[i] >= 0x61 && cadEntrada[i] <= 0x7A)
		{
			instrEntrada[i] = cadEntrada[i];
		}
		else
		{
			instrEntrada[i] = 0;
		}		
	}
	gotoxy(10,22);
	
	// Compara la instruccion con cada palabra reservada para ejecutar su respectivo procedimiento
	if(strcmp(instrEntrada, "add") == 0)
	{
		//CUIDADO
		//cadEntrada[3] += 0x10; 
		//gotoxy(10, 24); 
		//printf("%c", cadEntrada[2]);
		if(cadEntrada[3] != ' ') return 0;
		num = getNumber(4, 0);
		if(num < 1 || num > 6)
		{
			printf("Pantalla fuera de rango                                  ");
			return 0;
		}
		num = num - 1;
		
		//Si esta inactivo
		if(pcbs[num].Estado == 0)
		{
			IniciarProceso(num);
			gotoxy(10,22);
			printf("Proceso iniciado correctamente                           ");
		}else
		{
			printf("El proceso ingresado ya esta activo                      ");
		}
	}
	else if(strcmp(instrEntrada, "pause") == 0)
	{
		if(cadEntrada[5] != 0x20) return 0;
		num = getNumber(6, 0);
		if(num < 1 || num > 6)
		{
			printf("Pantalla fuera de rango                                  ");
			return 0;
		}
		num = num - 1;
		
		//Si esta activo
		if(pcbs[num].Estado==1)
		{
			PausarProceso(num);
			printf("Proceso pausado correctamente                            ");
		}else
		{
			printf("No es posible pausar el proceso [Proceso inactivo]       ");
		}
	}
	else if(strcmp(instrEntrada, "play") == 0)
	{
		if(cadEntrada[4] != 0x20) return 0;
		num = getNumber(5, 0);
		if(num < 1 || num > 6)
		{
			printf("Pantalla fuera de rango                                  ");
			return 0;
		}
		num = num - 1;
		
		//Si esta pausado
		if(pcbs[num].Estado==2)
		{
			ReanudarProceso(num);
			printf("Proceso reanudado correctamente                          ");
		}else
		{
			printf("No es posible reanudar el proceso [Proceso no pausado]   ");
		}
	}
	else if(strcmp(instrEntrada, "remove") == 0)
	{
		if(cadEntrada[6] != 0x20) return 0;
		num = getNumber(7, 0);
		if(num < 1 || num > 6)
		{
			printf("Pantalla fuera de rango                                  ");
			return 0;
		}
		num = num - 1;
		
		//Si esta pausado o activo
		if(pcbs[num].Estado == 1 || pcbs[num].Estado == 2)
		{
			MatarProceso(num);
			printf("Proceso terminado correctamente                          ");
		}else
		{
			printf("No es posible terminar el proceso [Proceso Inactivo]     ");
		}
	}
	else if(strcmp(instrEntrada, "clear") == 0)
	{
		if(cadEntrada[5] != 0x20) return 0;
		num = getNumber(6, 0);
		if(num < 1 || num > 6)
		{
			printf("Pantalla fuera de rango                                  ");
			return 0;
		}
		num = num - 1;
		
		//Si esta pausado o activo
		if(pcbs[num].Estado == 1 || pcbs[num].Estado == 2)
		{
			LimpiarPantalla(num);
			gotoxy(10,22);
			printf("Pantalla limpiada correctamente                          ");
		}else
		{
			printf("No es posible limpiar la pantalla del proceso            ");
		}
	}
	else if(strcmp(instrEntrada, "quantum") == 0)
	{
		if(cadEntrada[7] != 0x20) return 0;
		num = getNumber(8, 0);
		
		if(num < 1 || num > 6)
		{
			printf("Pantalla fuera de rango                                  ");
			return 0;
		}
		
		//Si esta activo
		if(pcbs[num - 1].Estado==1)
		{ // Guarda el tiempo ingresado en su respectivo pcb con su respectivo 
			pcbs[num - 1].Quantum = miliSegundos / 0x32;			
			printf("Quantum guardado correctamente                           ");
		}else
		{
			printf("No es posible asignar el quantum [Proceso inactivo]      ");
		}		
	}
	else if(strcmp(instrEntrada, "stats") == 0)
	{
		if(cadEntrada[5] != 0x20) return 0;
		num = getNumber(6, 1);
		if(num < -1 || num > 6)
		{
			printf("Pantalla fuera de rango                                  ");
			return 0;
		}	

		gotoxy(10,22);
	    printf("                                                         ");
		//Nuevo 
		switch (num)
		{
			
			case -1:
				gotoxy(10,22);
				printf(" Idle / %.1f%%                                       ", PorcentajeUso(num));
			break;
			case 0:
				gotoxy(10,22);
				int banderaX = -1; 
				for (int d = 1; d <= 6; d++)
				{
					//Mostrará stats solo de los activos. 
					if (pcbs[d - 1].Estado != 0)
					{
						printf("%2d/%.1f%%,", d, PorcentajeUso(d));
						banderaX = 1; 
					}
				}

				if (banderaX == -1)
				{
					printf("No hay ningun proceso activo.                     ");
				}
			break;
			default:
				gotoxy(10,22);

				//Únicamente mostrará Stats si el proceso está activo. 
				if (pcbs[num -1].Estado != 0)
				{
					printf(" %2d / %.1f%%                                   ", num, PorcentajeUso(num));
				}
				else 
				{
					printf("El proceso ingresado no esta activo               ");
				}
			break;
		}
		
	}
	else return 0;
	
	return 1;
}

// Metodo que verifica si ingresaron algo en el teclado o hicieron clic en la pantalla
int Verificaciones()
{
	// Se verifica si se presiono algun boton del mouse 
	switch(BotonPulsado())
		{
			case 0: // Si no presiono nada
				mouse_presionado = 0;
				
				if(kbhit())
				{
					// Se mueve a la posicion donde corresponde la escritura y esribe el caracter ingresado
					gotoxy(posCadena,19);
					char ch = getch();
					
					if(ch == 0x08)
					{ // Si ingresaron un backspace, imprime el backspace, imprime un espacio y otra vez el backspace
						if(posCadena > 9)
						{
							cadEntrada[posCadena - 9] = 0;
							posCadena--;
							printf("%c", ch);
							printf(" ");
							printf("%c", ch);
							gotoxy(posCadena,19);
						}
					}
					else if(ch == 0x0D)
					{// Si se presiona un enter
						if(validarEntrada() != 1)
						{							
							gotoxy(10,22);printf("No se ingreso una instruccion correcta                  ");
						}
						gotoxy(9, 19); printf("                                                          ");
						
						// Limpia la cadena de entrada para que puedan volver a ingresar una instruccion
						for(int i = 0; i < 57; i++)
						{
							cadEntrada[i] = 0;
						}
						posCadena = 9;
					}
					else if(ch == 0x1B)
					{
						// Si se presiona "esc" termina el programa
						bandera = 0;
						return 0;
					}
					else
					{ // Si se ingresa una tecla normal 
						if(posCadena < 0x39)
						{
							// Guarda la entrada en la cadena de entrada e imprime el caracter en pantalla
							cadEntrada[posCadena - 9] = ch;
							printf("%c", ch);
							posCadena++;
						}
					}
				}
				break;
			case 1:
				{						
					if(mouse_presionado==0)
					{
						int calculo = CalculoPantalla();
						mouse_presionado = 1;
						//gotoxy(10,22);printf("Pantalla:   : %2d puntos %d",calculo, pcbs[calculo].CantidadPuntos);
					}
					
					//mouse_presionado = BotonPulsado();
				break;
				}
			case 2:
				{
				if(mouse_presionado==0)
					{
						int calculo = CalculoPantalla();
						mouse_presionado = 1;
						//gotoxy(10,22);printf("Pantalla:   : %2d puntos %d",calculo, pcbs[calculo].CantidadPuntos);
					}
				break;
				}
		}
		if(idProceso==-1)
		{
			int found = 0;
			int count=0;
			while(found==0)
			{
				count++;
				idProceso++;
				if(idProceso>5)
				{
					idProceso=0;
				}				
				if(pcbs[idProceso].Estado == 1)
				{
					found =1;
				}
				if(count>=7)
				{
					idProceso = -1;
					found = 1;
				}
			}
			if(idProceso!=-1)
			{
				pcbs[idProceso].realQuantum = pcbs[idProceso].Quantum;
			}
		}else
		if(pcbs[idProceso].realQuantum<=0)
		{
			int found = 0;
			int count=0;
			while(found==0)
			{
				count++;
				idProceso++;
				if(idProceso>5)
				{
					idProceso=0;
				}				
				if(pcbs[idProceso].Estado == 1)
				{
					found =1;
				}
				if(count>=7)
				{
					idProceso = -1;
					found = 1;
				}
			}
			if(idProceso!=-1)
			{
				pcbs[idProceso].realQuantum = pcbs[idProceso].Quantum;
			}
		}
		//mouse_presionado = BotonPulsado();
		return 0;
}

int MirarPuntero(void)
	{
	asm xor ax, ax
	asm int 33h
	asm cmp ax, -1
	asm je  Existe_Raton
	return 0;
	Existe_Raton:
	return 1;
	}

int MostrarPuntero(void)
	{
	asm mov ax, 01h
	asm int 33h
	return 1;
	}

int BotonPulsado()
	{
	asm mov ax, 03h
	asm int 33h
	return _BX;
	}

int CorHTexto(void)
	{
	asm mov ax, 03h
	asm int 33h
	return (_CX/8)+1;
	}

int CorVTexto(void)
	{
	asm mov ax, 03h
	asm int 33h
	return (_DX/8)+1;
	}

int CalculoPantalla(void)
{
	// a->x	
	// b->y
	int a = CorVTexto();
	int b = CorHTexto();
	int noPantalla = 0;
	//Primeras 3 pantallas
	if (a <= 9 && a > 3)
	{

		if (b <= 22 && b >= 3)
		{						
			noPantalla = 0;
		}

		else if (b <= 44 && b >= 25)
		{
			noPantalla = 1;
			//return 2; 
		}

		else if (b <= 66 && b >= 47)
		{
			noPantalla = 2;
			//return 3;
		}

		else 
		{
			noPantalla = -1;
			//return -1;
		}


	}

	//3 Pantallas de abajo
	else if (a <= 16 && a >= 11)
	{

		if (b <= 22 && b >= 3)
		{
			noPantalla = 3;
			//return 4;
		}

		else if (b <= 44 && b >= 25)
		{
			noPantalla = 4;
			//return 5; 
		}

		else if (b <= 66 && b >= 47)
		{
			noPantalla = 5;
			//return 6;
		}

		else 
		{
			noPantalla = -1;
			//return -1;
		}
	}

	else 
	{
		noPantalla = -1;
		return -1;
	}
	if (noPantalla>=0)
	{			
		if(pcbs[noPantalla].Estado==1)
		{
			int puntos = pcbs[noPantalla].CantidadPuntos;
			pcbs[noPantalla].puntos[puntos][0] = b;
			pcbs[noPantalla].puntos[pcbs[noPantalla].CantidadPuntos][1] = a;
			pcbs[noPantalla].CantidadPuntos++;
		}
	}
	return noPantalla;
}

void PintarPunto()
{
	if(idProceso>=0)
		{
			//Nuevo 
			clock_t Inicio = clock(); 

			if(pcbs[idProceso].CantidadPuntos>0)
			{
				pcbs[idProceso].indice++;
				
				
				if(idProceso>=0)
				{
					int pos = pcbs[idProceso].indice;
					gotoxy(pcbs[idProceso].puntos[pos][0],pcbs[idProceso].puntos[pos][1]);
					printf("%c", pcbs[idProceso].caracter);
				}

				if(pcbs[idProceso].indice==(pcbs[idProceso].CantidadPuntos-1))
				{
					pcbs[idProceso].indice = -1;
					if(pcbs[idProceso].caracter == '.')
					{
						pcbs[idProceso].caracter = ' ';
					}
					else
					{
						pcbs[idProceso].caracter = '.';
					}
				}
			}

			//Nuevo
			clock_t Fin = clock(); 
			pcbs[idProceso].Time += Fin - Inicio; 
			TIdle += Fin - Inicio; 

		}
}

void MatarProceso(int id)
{
	pcbs[id].Estado=0;
	pcbs[id].Time = 0; 
}

void PausarProceso(int id)
{
	pcbs[id].Estado=2;
}

void LimpiarPantalla(int id)
{
	//Cambiar las variables para que se vea como nuevo
	pcbs[id].CantidadPuntos = 0;
	pcbs[id].caracter= '.';
	pcbs[id].indice = -1;

	int posx=0;
	int posy=0;
	switch (id)
    {
	case 0:
		posx = 3;
		posy = 4;
		break;
	case 1:
		posx = 25;
		posy = 4;
	    break;
	case 2:
		posx = 47;
		posy = 4;
	    break;
	case 3:
		posx = 3;
		posy = 11;
		break;
	case 4:
		posx = 25;
		posy = 11;
	    break;
	case 5:
		posx = 47;
		posy = 11;
	 default:
	 break;

      }
	
	for( int index = posy; index < (posy + 6); index = index + 1 ) {
		gotoxy(posx, index);
		printf("                    ");
	}	  
}

void ReanudarProceso(int id)
{
	pcbs[id].Estado=1;
}

void IniciarProceso(int id)
{
	LimpiarPantalla(id);
	pcbs[id] = PCB(id);
}

void SetQuantum(int id, int lquantum)
{
	//Hay que hacer antes la conversion a quantum 1=50ms
	pcbs[id].Quantum = lquantum;
}

double PorcentajeUso(int id)
{
	double TUso; 
	clock_t Cronometro = clock(); 
	long TiempoTotal = Cronometro - TGlobal;
	
	if (id > 0)
	{
		//Restar 1 por si viene en base 1
		id = id - 1; 
		long TiempoProceso = pcbs[id].Time; 

		TUso = ((double)TiempoProceso) / ((double)TiempoTotal);
		return TUso * 100; 
	}

	else 
	{
		long TiempoTodos = 0; 
		int Activo = 0; 
		for (int i = 0; i < 6; i++)
		{
			if (pcbs[i].Estado != 0)
			{
				Activo = 1; 
				TiempoTodos = TiempoTodos + pcbs[i].Time;
			}
		}

		if (Activo == 1)
		{
			TiempoTodos = TiempoTotal - TiempoTodos; 
			TUso = ((double)TiempoTodos) / ((double)TiempoTotal) * 100;
		}

		else 
		{
			TiempoTodos = TiempoTotal - TIdle; 
			TUso = ((double)TiempoTodos) / ((double)TiempoTotal) * 100;
		}
		

		return TUso; 
	}

}