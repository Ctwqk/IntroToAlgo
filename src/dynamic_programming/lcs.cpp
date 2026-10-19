#include <iostream>

using namespace std;

class lcs{
private:
    int **tabulate;
    int *list;
    string str1,str2;
    int size1,size2;
public:
    lcs(string s1,string s2):str1(s1),str2(s2){
        size1=s1.size();
        size2=s2.size();
        tabulate=new int*[size2+1];
        for(int i=0;i<=size2;i++) tabulate[i]=new int[size1+1]();
    }
    string getLCS();
    int getLCSLength();
};
string lcs::getLCS(){
    for(int i=1;i<=size2;i++){
        for(int j=1;j<=size1;j++){
            if(str1[j-1]==str2[i-1])tabulate[i][j]=tabulate[i-1][j-1]+1;
            else if(tabulate[i][j-1]>tabulate[i-1][j]) tabulate[i][j]=tabulate[i][j-1];
            else tabulate[i][j]=tabulate[i-1][j];
        }
    }
    /*for(int i=0;i<=size2;i++){
        for(int j=0;j<=size1;j++)cout<<tabulate[i][j];
        cout<<endl;
    }*/
    string ans;
    int index1=size1,index2=size2;
    while(index1>0&&index2>0){
        if(str1[index1-1]==str2[index2-1]) {
            ans=str1[--index1]+ans;
            index2--;
        }
        else tabulate[index1-1][index2]>tabulate[index1][index2-1]?index1--:index2--;
    }
    return ans;
}
int lcs::getLCSLength(){
    list=new int[size1+1]();
    int tmp=0,swa;
    for(int i=1;i<=size2;i++){
        tmp=0;
        for(int j=1;j<=size1;j++){
            swa=tmp;
            if(str2[i-1]==str1[j-1])tmp=list[j-1]+1;
            else if(tmp<list[j]) tmp=list[j];
            list[j-1]=swa;
        }
        list[size1]=tmp;
    }
    return list[size1];
}
int main(){
    string str1="10010101";
    string str2="010110110";
    lcs *l=new lcs(str1,str2);
    cout<<l->getLCSLength()<<endl;
    cout<<l->getLCS();
}