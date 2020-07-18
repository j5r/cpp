#include <iostream>
#include <iomanip>
#include "cmdformat.cpp"
using namespace std;

template <class Type>
class Matrix {  
  private:
    Type **myself;
    int nrows;
    int ncols;
    
  public:     
  Matrix<Type>(int m, int n){
    myself = (Type **) calloc(m,sizeof(Type *));
    if(myself == nullptr){
      cout << _FRED << "Memory allocation error." << _RST << endl;
      throw 7;
      exit(7);
    }
    register int i;
    for(i=0; i<m; i++){
      myself[i] = (Type *) calloc(n, sizeof(Type));
      if(myself[i] == nullptr){
        cout << _FRED <<"Memory allocation error." << _RST << endl;            
        throw 7;
        exit(7);
      }
    }
    this->nrows = m;
    this->ncols = n;   
  }

  ~Matrix(){
    register int i;
    for(i=0; i<this->nrows; i++){
        delete[] myself[i];
        }
        delete[] myself;
    }

void print(int precision=4){
  register int i, j;
  bool dotsrows, dotscols;
  int setw_ = precision + 8;

  cout << _FYELLOW << setw(7) << setfill(' ') << right  << "Matrix";  
  dotscols = (this->ncols > 6);
  for(j=0; j<this->ncols; j++){
    if((this->ncols>6) && (j>2 && j<this->ncols-3)){
      if(dotscols){          
        cout << left << setw(5) << "...";
        dotscols = false;
      }
        continue;
    }
      cout << right << setprecision(precision) << setw(setw_) << setfill(' ');
      cout << j + 1 << "  ";
  }
  cout << endl << _FBLUE;
  
  dotsrows = (this->nrows > 6);
  for(i=0; i<this->nrows; i++){
    dotscols = (this->ncols >6);    
    if(this->nrows>6 && (i>2 && i<this->nrows-3)){
      if(dotsrows){
        cout << _FYELLOW <<setprecision(precision) << setw(7) << setfill(' ')
            << right << ":";
        cout << _FBLUE << setprecision(precision) << setw(setw_+1) << setfill(' ')
            << right << ":\n";
        dotsrows = false;
      }
      continue;
    }
    
    cout << _FYELLOW << setw(7) << setfill(' ') << right  << i + 1 << _FBLUE;

    for(j=0; j<this->ncols; j++){      
      if((this->ncols>6) && (j>2 && j<this->ncols-3)){
        if(dotscols){          
          cout << left << setw(5) << "...";
          dotscols = false;
        }
        continue;
      }
      cout << scientific << right << setprecision(precision) << setw(setw_) << setfill(' ');
      cout << myself[i][j] << "  ";
    }
    cout << endl;
  }
  cout << _RST << endl;
}

Type get(int i, int j){
  i = (this->nrows + i )% this->nrows;
  j = (this->ncols + j )% this->ncols;
  if(i<0 || j<0){
    throw 11;
    exit(11);
  }
  return myself[i][j];
}

void set(int i, int j, Type value){
  i = (this->nrows + i )% this->nrows;
  j = (this->ncols + j )% this->ncols;
  if(i<0 || j<0){
    throw 11;
    exit(11);
  }
  myself[i][j] = (Type) value;
}
   
  
};