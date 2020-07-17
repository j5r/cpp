#include <iostream>
#include "cmdformat.cpp"

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
      std::cout << "Allocation error.";
      exit(-7);
    }
    register int i;
    for(i=0; i<m; i++){
      myself[i] = (Type *) calloc(n, sizeof(Type));
      if(myself[i] == nullptr){
        std::cout << _FRED <<"Allocation error." << _RST;            
        exit(-7);
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

void print(){
  int i, j;
  std::cout << _FBLUE;
  for(i=0;i<this->nrows;i++){
    for(j=0;j<this->nrows;j++){
       std::cout << myself[i][j] << "__";
    }
    std::cout << "\b\b  \n";
  }
  std::cout << _RST;
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









