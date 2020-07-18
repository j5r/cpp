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
      exit(7);
    }
    register int i;
    for(i=0; i<m; i++){
      myself[i] = (Type *) calloc(n, sizeof(Type));
      if(myself[i] == nullptr){
        cout << _FRED <<"Memory allocation error." << _RST << endl;            
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
  int setw_ = precision+3;

  cout << _FYELLOW;
  for(j=0;j<this->ncols;j++){
      cout << fixed << setprecision(precision) << setw(setw_) << setfill(' ')
      << j+1 << "  ";
    }
  cout << _RST << endl;


  cout << _FBLUE;
  for(i=0;i<this->nrows;i++){
    for(j=0;j<this->ncols;j++){
      cout << fixed << setprecision(precision) << setw(setw_) << setfill(' ')
      << myself[i][j] << "  ";
    }
    cout << endl;
  }
  cout << _RST << endl;
}

Type get(int i, int j){
  return myself[i][j];
}

void set(int i, int j, Type value){
  myself[i][j] = (Type) value;
}
   
  //void print();
};






//
    /*
    Type get(int,int);
    void set(int,int,int);
    void set(int,int,float);
    void set(int,int,double);


    //
    void _add(int);
    void _add(float);
    void _add(double);
    void _add(Matrix);

  */









