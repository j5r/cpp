#include <iostream>
#include <iomanip>
#include <string>
#include <typeinfo>
#include "cmdformat.cpp"
using namespace std;


int min(int a, int b){return a<b?a:b;}
int max(int a, int b){return a>b?a:b;}



template <class Type>
class Matrix {  
  private:
    Type **myself;
    int nrows=0;
    int ncols=0;
    
  public: 
  int cols(){return this->ncols;}
  int rows(){return this->nrows;} 

  


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

  Matrix<Type>(){
    myself = nullptr;
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
    exit(11);
  }
  return myself[i][j];
}

void set(int i, int j, Type value){
  i = (this->nrows + i )% this->nrows;
  j = (this->ncols + j )% this->ncols;
  if(i<0 || j<0){
    exit(11);
  }
  myself[i][j] = (Type) value;
}
   
  




  void print(int precision=4){  
    string INDICES = _FYELLOW; 
    string MAINCOLOR = _FPINK;
    register int i, j;
    int setw_ = precision + 8;
    if(rows()+cols()==0){
      cout << endl << INDICES << "Matrix" << endl;
      cout << "|______ " << _BLINK <<"(0, 0)"
      << _RST <<INDICES <<" Type: " << typeid(*this).name();
      cout << _RST << endl;
      return;
    }

    // first line with indexes
    cout << endl << INDICES << left << setw(7) << "Matrix";
    for(j=0; j<min(cols(), 3); j++){
      cout << right << setprecision(precision) << setw(setw_) << setfill(' ') 
      << j << "  ";      
    }
    
    if(cols() > 3){
      if(cols() > 6){
        cout << scientific <<  setw(3) << setfill(' ') << "..."; 
      }

      for(j=max(3,cols()-3); j<cols(); j++){
        cout << right << setprecision(precision) << setw(setw_) << setfill(' ') 
        << j << "  ";   
      }
    }
    cout << endl;

    //the rest of lines, with indexes and numbers
    for(i=0; i<min(rows(), 3); i++){
      cout << INDICES << "|" << right << setw(6) << i;

      for(j=0; j<min(cols(), 3); j++){
        cout << MAINCOLOR << scientific << right << setprecision(precision) 
        << setw(setw_) << setfill(' ') << myself[i][j] << "  ";      
      }

      if(cols() > 3){
        if(cols() > 6){
          cout << scientific <<  setw(3) << setfill(' ') << "..."; 
        }

        for(j=max(3,cols()-3); j<cols(); j++){
          cout << MAINCOLOR << scientific << right << setprecision(precision) 
          << setw(setw_) << setfill(' ') << myself[i][j] << "  ";   
        }
      }
      cout << endl;
    }

    if(rows()>3){
      if(rows()>6){
        cout << INDICES << "|" << right << setw(6) << ":";
        cout << MAINCOLOR << setprecision(precision) << setw(setw_) 
        << setfill(' ') << right << ":" << endl;
      }

      for(i=max(rows()-3, 3); i<rows(); i++){
        cout << INDICES << "|" << right << setw(6) << i;

        for(j=0; j<min(cols(),3); j++){
          cout <<  MAINCOLOR << scientific << right << setprecision(precision) 
          << setw(setw_) << setfill(' ') << myself[i][j] << "  ";      
        }

        if(cols() > 3){
          if(cols() > 6){
            cout << scientific << setw(3) << setfill(' ') << "..."; 
          }
          
          for(j=max(cols()-3, 3); j<cols(); j++){
            cout << scientific << right << setprecision(precision) << setw(setw_) 
            << setfill(' ') << myself[i][j] << "  ";   
          }
        }    
        cout << endl;
      }
    }

    cout << INDICES << "|" << setfill('_') << setw(7) << " " << "(" << rows() 
    << ", " << cols() << ") Type: " << typeid(*this).name();
    cout << endl << setfill(' ') << _RST;
  }




void zeros(){
  register int i,j;
  for(i=0;i<rows();i++){
    for(j=0;j<cols();j++){
      set(i,j,0);
    }
  }
}

template <class R>
void operator=(Matrix<R>& m){
  this->_copy(m);
}



template <class R>
void _copy(Matrix<R>& m) {
  register int i,j;
  
  for(i=0; i<rows(); i++){
    delete[] myself[i];
  }
  myself = (Type **) calloc(m.rows(),sizeof(Type *));
  if(myself == nullptr){
      cout << _FRED << "Memory allocation error." << _RST << endl;
      exit(7);
    }
    for(i=0; i<m.rows(); i++){
      myself[i] = (Type *) calloc(m.cols(), sizeof(Type));
      if(myself[i] == nullptr){
        cout << _FRED <<"Memory allocation error." << _RST << endl;            
        exit(7);
      }
    }
    this->nrows = m.rows();
    this->ncols = m.cols();
    for(i=0; i<rows(); i++){
      for(j=0; j<cols(); j++){
        myself[i][j] = (Type) m.get(i,j);
      }
    }
  
    
}

void printsize(){
  cout << "(" << rows()<< ", " << cols() << ")" << endl;
}


Matrix<Type>& operator-(){
register int i,j;
for(i=0; i<rows(); i++){
    for(j=0; j<cols(); j++){
      set(i, j, -get(i,j));
    }
  }
  return *this;
}
};
//////////////////////////////////////////


template <class T, class R>
Matrix<T>& operator+(Matrix<T>& m1, Matrix<R>& m2){
  if(m1.rows() != m2.rows() || m1.cols() != m2.cols()){
    cout << _FRED <<"Dimention mismatch error in operator +." << _RST << endl;
    exit(13);
  }
  register int i,j;
  Matrix<T> *ans = new Matrix<T>(m1.rows(), m1.cols());
  ans->_copy(m2);
  for(i=0; i<m1.rows(); i++){
    for(j=0; j<m1.cols(); j++){
      ans->set(i, j, m1.get(i,j) + ans->get(i,j));
    }
  }
  return *ans;
}





template <class T, typename R>
Matrix<T>& operator+(Matrix<T>& m1, R m2){  
  register int i,j;
  Matrix<T> *ans = new Matrix<T>(m1.rows(), m1.cols());
  ans->_copy(m1);
  for(i=0; i<m1.rows(); i++){
    for(j=0; j<m1.cols(); j++){
      ans->set(i, j, m2 + ans->get(i,j));
    }
  }
  return *ans;
}

template <class T, typename R>
Matrix<T>& operator+(R m2, Matrix<T>& m1){  
  return (m1+m2);
}


template <class T, typename R>
Matrix<T>& operator*(Matrix<T>& m1, R m2){  
  register int i,j;
  Matrix<T> *ans = new Matrix<T>(m1.rows(), m1.cols());
  ans->_copy(m1);
  for(i=0; i<m1.rows(); i++){
    for(j=0; j<m1.cols(); j++){
      ans->set(i, j, m2 * ans->get(i,j));
    }
  }
  return *ans;
}

template <class T, typename R>
Matrix<T>& operator*(R m2, Matrix<T>& m1){  
  return (m1*m2);
}



template <class T, class R>
Matrix<T>& operator-(Matrix<T>& m1, Matrix<R>& m2){
  if(m1.rows() != m2.rows() || m1.cols() != m2.cols()){
    cout << _FRED <<"Dimention mismatch error in operator +." << _RST << endl;
    exit(13);
  }
  register int i,j;
  Matrix<T> *ans = new Matrix<T>(m1.rows(), m1.cols());
  ans->_copy(m2);
  for(i=0; i<m1.rows(); i++){
    for(j=0; j<m1.cols(); j++){
      ans->set(i, j, m1.get(i,j) - ans->get(i,j));
    }
  }
  return *ans;
}





template <class T, typename R>
Matrix<T>& operator-(Matrix<T>& m1, R m2){  
  register int i,j;
  Matrix<T> *ans = new Matrix<T>(m1.rows(), m1.cols());
  ans->_copy(m1);
  for(i=0; i<m1.rows(); i++){
    for(j=0; j<m1.cols(); j++){
      ans->set(i, j, ans->get(i,j) - m2);
    }
  }
  return *ans;
}

template <class T, typename R>
Matrix<T>& operator-(R m2, Matrix<T>& m1){  
  return (m2+(-m1));
}



template <class T, typename R>
Matrix<T>& operator/(Matrix<T>& m1, R m2){  
  register int i,j;
  Matrix<T> *ans = new Matrix<T>(m1.rows(), m1.cols());
  ans->_copy(m1);
  for(i=0; i<m1.rows(); i++){
    for(j=0; j<m1.cols(); j++){
      ans->set(i, j, ans->get(i,j)/(T) m2);
    }
  }
  return *ans;
}

template <class T, typename R>
Matrix<T>& operator/(R m2, Matrix<T>& m1){  
   register int i,j;
  Matrix<T> *ans = new Matrix<T>(m1.rows(), m1.cols());
  ans->_copy(m1);
  for(i=0; i<m1.rows(); i++){
    for(j=0; j<m1.cols(); j++){
      ans->set(i, j, (T) m2/ans->get(i,j));
    }
  }
  return *ans;
}


template <class T, class R>
void operator/(Matrix<T>& m1,Matrix<R>& m2){  
  cout << _FRED << "Division between matrices is not allowed!" << _RST <<endl;
  exit(23);
}