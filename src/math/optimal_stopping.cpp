#include <iostream>
using namespace std;
class CalFormula{
private:
    int num;
    double calculate(int x);
public:
    CalFormula(int n):num(n){}
    int getMax();
};
int CalFormula::getMax(){
    int idx=0;
    double tmp,max=0;
    for(int i=1;i<=num;i++){
        tmp=calculate(i);
        if(tmp>max){
            max=tmp;
            idx=i;
        }
    }
    cout<<max<<endl;
    return idx;
}
double CalFormula::calculate(int x){
    double ans=0;
    for(int i=0;i<num-x;i++) ans+=1/(double)(i+x);
    return ans*(double)x/(double)num;
}
int main(){
    CalFormula *c=new CalFormula(200);
    cout<<c->getMax();
}