#include <iostream>
#include <algorithm>
#include <climits>
#include <iomanip>

using namespace std;

class Expression{
private:
	string exp;
	int * nums;
	char * opes;
	int **dp;
	int **path;
	int num;
	void display(){
		for(int i=0;i<num;i++){
			for(int j=0;j<num;j++){
				cout<<setw(6)<<dp[i][j]<<" ";
			}
			cout<<endl;
		}
	}
public:
	Expression(string exp):exp(exp){
		int * numsTmp=new int[exp.size()];
		char * opesTmp=new char[exp.size()];	
		int cch,tmp=0;	
		num=0;
		for(char c:exp){
			if(c==' ') continue;
			cch=c-'0';
			if(cch<0||cch>9){
				numsTmp[num]=tmp;
				opesTmp[num++]=c;
				tmp=0;
			}
			else{
				tmp*=10;
				tmp+=cch;
			}
		}
		numsTmp[num++]=tmp;
		nums=new int[num];
		opes=new char[num-1];
		copy(numsTmp,numsTmp+num,nums);
		copy(opesTmp,opesTmp+num-1,opes);
		dp=new int* [num];
		path=new int* [num];
		for(int i=0;i<num;i++) dp[i]=new int[num]();
		for(int i=0;i<num;i++){
			path[i]=new int[num];
			for(int j=i+1;j<num;j++){
				dp[i][j]=-INT_MAX;
				dp[j][i]=INT_MAX;
			}
			dp[i][i]=nums[i];
		}
		delete [] numsTmp;
		delete [] opesTmp;	
	}
	int calculateMax();
	~Expression(){
		for(int i=0;i<num;i++){
			delete[] dp[i];	
			delete[] path[i];
		}
		delete[] dp;
		delete[] path;
		delete[] nums;
		delete[] opes;
	}
	//void show();
};
int Expression::calculateMax(){
	auto cal=[](const int &a,const int &b,const char &c){
		if(c=='+') return a+b;
		if(c=='-') return a-b;
		if(c=='*'||c=='x') return a*b;
		if(c=='/') return a/b;
		cout<<" don't u input shit'<<endl";
		return -1;
	};
	int cache[4];
	for(int i=1;i<num;i++){
		for(int j=0;j<num-i;j++){
			//dp[j][j+i]
			for(int k=j;k<j+i;k++){
				if(opes[k]=='/'||opes[k]=='*'){
					cache[0]=cal(dp[j][k],dp[k+1][j+i],opes[k]);
					cache[1]=cal(dp[k][j],dp[j+i][k+1],opes[k]);
					cache[2]=cal(dp[j][k],dp[j+i][k+1],opes[k]);
					cache[3]=cal(dp[k][j],dp[k+1][j+i],opes[k]);
					for(int l=0;l<4;l++){
						if(cache[l]>dp[j][j+i]){
							dp[j][j+i]=cache[l];
							path[j][j+i]=k;
						}
						if(cache[l]<dp[j+i][j]){
							dp[j+i][j]=cache[l];
							path[j][j+i]=k;
						}
					}
				}
				else if(opes[k]=='-'){
					cache[2]=cal(dp[j][k],dp[j+i][k+1],opes[k]);
					if(cache[2]>dp[j][j+i]){
						dp[j][j+i]=cache[2];
						path[j][j+i]=k;
					}
					if(dp[j+i][j]>-dp[j][j+i]){
						dp[j+i][j]=-dp[j][j+i];
						path[j][j+i]=k;
					}
				}
				else{
					cache[0]=cal(dp[j][k],dp[k+1][j+i],opes[k]);
					cache[1]=cal(dp[k][j],dp[j+i][k+1],opes[k]);
					if(cache[0]>dp[j][j+i]){
						dp[j][j+i]=cache[0];
						path[j][j+i]=k;
					}
					if(cache[1]<dp[j+i][j]){
						dp[j+i][j]=cache[1];
						path[j][j+i]=k;
					}
				}
			}
		}
	}
	display();
	return dp[0][num-1];
}
/*
void Expression::show(){
	auto parse=[](string &exp,const int &left,const int &right){
		int count=0;
		int n=exp.size();
		for(int i=0;i<n;i++){
			if(exp[i]<0||exp[i]>9){
				count++;
				if(count==left-1){
					exp.insert(i+1,"(");
					n++;
				}
				else if(count==right){
					exp.insert(i,")");
					break;
				}
			}
		}
	};
	int **tmp=new int*[num];
	for(int i=0;i<num;i++) tmp[i]=new int[2]();
	int i=0,j=1;
	tmp[0][0]=0;
	tmp[0][1]=num-1;
	int x,y;
	while(i<j){
		x=tmp[i][0];
		y=tmp[i++][1];
		if(x==y) continue;
		if(x-y==-1){
			parse(exp,x,y);
			continue;
		}
		tmp[j][0]=x;
		tmp[j++][1]=dp[y][x];
		tmp[j][0]=dp[y][x]+1;
		tmp[j][1]=y;
	}
	
	for(int i=0;i<num;i++) delete[] tmp[i];
	delete[] tmp;
	cout<<exp<<endl;
}
*/
int main(int argc,char** argv){
	string exp="5-8+7*4-8+9";
	if(argc>1){
		exp=argv[1];
	}
	Expression e(exp);
	cout<<e.calculateMax()<<endl;
	return 0;
}
