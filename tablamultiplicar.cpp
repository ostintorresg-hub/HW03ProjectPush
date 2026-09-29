#include <iostream>

using namespace std;

void tablaMulti(int);

int main (){
	
	int numero;
	
	do{
		cout<<"Ingrese un numero entero positivo del 1 al 12: ";
		cin>>numero;
	}while(numero<0 || numero>12);
	
	tablaMulti(numero);
	
	
	return 0;
}

void tablaMulti (int x){
	
	int i,resultado;
	
	for (i=1;i<=12;i++){
		
		resultado = x * i;
		
		cout << x << " x " << i << " = " << resultado << endl;

	}
}