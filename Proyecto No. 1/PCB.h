#include <stdio.h>

#include <dos.h>

#include <conio.h>

#include <time.h>

#include <iostream.h>


#define QT 1
//Estados:
//		0 - Inactivo
//		1 - Activo
//		2 - Pausado
class PCB {
   public:
      // Atributos del PCB
	  int id;
	  int Estado;
	  int CantidadPuntos;
	  int puntos[100][2];
	  int Quantum;
	  int realQuantum;
	  char caracter;
	  clock_t Time;
	  int indice;
	  PCB();
	  PCB(int);
};
PCB::PCB () {
  indice = -1;
  id = 0;
  Estado = 0;
  CantidadPuntos = 0;
  realQuantum = QT;
  Time = 0;
  puntos[100][2] = 0;
  caracter = '.';
  Quantum = QT;
}
PCB::PCB (int ProcessId) {
  indice = -1;
  id = ProcessId;
  Estado = 1;
  CantidadPuntos = 0;
  realQuantum = QT;
  Time = 0;
  puntos[100][2] = 0;
  caracter = '.';
  Quantum = QT;
}