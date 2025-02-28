#include <iostream>
using namespace std;

class lmis{
private:
    int * sequence;
    int * length;
    int size;
    int * ans;
public:
    lmis(int * s,int si):size(si){
        sequence=s;
        length=new int[size]();
        ans=new int[size]();
    }
    int findLMIS();
};
int lmis::findLMIS(){
    int max=0;
    for(int i=0;i<size;i++){
        for(int j=0;j<i;j++){
            if(sequence[j]<sequence[i]&&ans[j]>=max) max=ans[j];
        }
        ans[i]=max+1;
    }
    for(int i=0;i<size;i++) if(ans[i]>max) max=ans[i];
    return max;
}