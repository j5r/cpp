#include <iostream>
#include <iomanip>
#include <string>
#include "cmdformat.cpp"
using namespace std;


int min(int a, int b){return a<b?a:b;}
int max(int a, int b){return a>b?a:b;}



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
   
  




void print(int precision=4){  
  string INDICES = _FYELLOW; 
  string MAINCOLOR = _FPINK;
  register int i, j;
  int setw_ = precision + 8;

  // first line with indexes
  cout << endl << INDICES << left << setw(7) << "Matrix";
  for(j=0; j<min(this->ncols, 3); j++){
    cout << right << setprecision(precision) << setw(setw_) << setfill(' ') 
    << j << "  ";      
  }
  
  if(this->ncols > 3){
    if(this->ncols > 6){
      cout << scientific <<  setw(3) << setfill(' ') << "..."; 
    }

    for(j=max(3,this->ncols-3); j<this->ncols; j++){
      cout << right << setprecision(precision) << setw(setw_) << setfill(' ') 
      << j << "  ";   
    }
  }
  cout << endl;

  //the rest of lines, with indexes and numbers
  for(i=0; i<min(this->nrows, 3); i++){
    cout << INDICES << "|" << right << setw(6) << i;

    for(j=0; j<min(this->ncols, 3); j++){
      cout << MAINCOLOR << scientific << right << setprecision(precision) 
      << setw(setw_) << setfill(' ') << myself[i][j] << "  ";      
    }

    if(this->ncols > 3){
      if(this->ncols > 6){
        cout << scientific <<  setw(3) << setfill(' ') << "..."; 
      }

      for(j=max(3,this->ncols-3); j<this->ncols; j++){
        cout << MAINCOLOR << scientific << right << setprecision(precision) 
        << setw(setw_) << setfill(' ') << myself[i][j] << "  ";   
      }
    }
    cout << endl;
  }

  if(this->nrows>3){
    if(this->nrows>6){
      cout << INDICES << "|" << right << setw(6) << ":";
      cout << MAINCOLOR << setprecision(precision) << setw(setw_) 
      << setfill(' ') << right << ":" << endl;
    }

    for(i=max(this->nrows-3, 3); i<this->nrows; i++){
      cout << INDICES << "|" << right << setw(6) << i;

      for(j=0; j<min(this->ncols,3); j++){
        cout <<  MAINCOLOR << scientific << right << setprecision(precision) 
        << setw(setw_) << setfill(' ') << myself[i][j] << "  ";      
      }

      if(this->ncols > 3){
        if(this->ncols > 6){
          cout << scientific << setw(3) << setfill(' ') << "..."; 
        }
        
        for(j=max(this->ncols-3, 3); j<this->ncols; j++){
          cout << scientific << right << setprecision(precision) << setw(setw_) 
          << setfill(' ') << myself[i][j] << "  ";   
        }
      }    
      cout << endl;
    }
  }

  cout << INDICES << "|" <<setfill('_') << setw(7) << " " << "(" << rows() 
  << ", " << cols() << ")";
  cout << endl << setfill(' ') << _RST;
}

int cols(){return this->ncols;}
int rows(){return this->nrows;}

};