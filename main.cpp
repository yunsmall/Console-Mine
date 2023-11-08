#include <iostream>
#include <algorithm>
#include <cstring>
#include <ctime>


using namespace std;

#define CEIL_SPACE 3
#define TOSTRING1(s) #s
#define TOSTRING(s) TOSTRING1(s)
#define CEIL_SPACE_STR TOSTRING(CEIL_SPACE)

typedef enum {
	NOT_OPEN,
	OPENED,
	FLAG,
	DOUBT
}StatValue;

typedef enum {
	OPEN,
	PLACE_FLAG,
	SET_DOUBT,
	CANCEL,
	DOUBLE_CLK
}ActValue;



int length,height;
int lei_count;

int total;

int* lei=nullptr;
int* status=nullptr;

int count_lei(int x,int y){
	int tmp_leis=0;
	for(int i=-1;i<=1;i++){
		for(int j=-1;j<=1;j++){
			//if(i==0&&j==0){
			//	continue;
			//}
			if(x+i>=0&&x+i<length&&y+j>=0&&y+j<height){
				tmp_leis+=lei[x+i+(y+j)*length];
			}
		}
	}
	return tmp_leis;
}

int count_flag(int x,int y){
	int tmp_flags=0;
	for(int i=-1;i<=1;i++){
		for(int j=-1;j<=1;j++){
			//if(i==0&&j==0){
			//	continue;
			//}
			if(!(i==0&&j==0)&&x+i>=0&&x+i<length&&y+j>=0&&y+j<height&&status[x+i+(y+j)*length]==FLAG){
				tmp_flags++;
			}
		}
	}
	return tmp_flags;
}

void gen_map(){
	if(lei!=nullptr){
		delete[] lei;
	}
	lei=new int[total];

	for(int i=0;i<total;i++){
		if(i<lei_count){
			lei[i]=1;
		}
		else{
			lei[i]=0;
		}
	}
	random_shuffle(lei,lei+total);

	if(status!=nullptr){
		delete[] status;
        }
	status=new int[total];
	memset(status,0,sizeof(int)*total);
}

void show_map(){
	printf("%-" CEIL_SPACE_STR "c",' ');
	for(int i=0;i<length;i++){
		printf("%-" CEIL_SPACE_STR "d",i);
	}
	putchar('\n');
	for(int j=0;j<height;j++){
		printf("%-" CEIL_SPACE_STR "d",j);
		for(int i=0;i<length;i++){
			switch(status[i+j*length]){
				case NOT_OPEN:
					//putchar('■);
					//putchar('N');
					printf("%-" CEIL_SPACE_STR "c",'N');
					break;
				case OPENED:
					if(lei[i+j*length]){
						printf("%-" CEIL_SPACE_STR "c",'@');
						break;
					}
					
					{
						int leis=count_lei(i,j);
						if(leis==0){
							//putchar('□');
							//putchar('O');
							printf("%-" CEIL_SPACE_STR "c",'O');
						}
						else{
							//putchar('0'+leis);
							printf("%-" CEIL_SPACE_STR "d",leis);
							// printf("%c  ",'0'+leis);
						}
						
					}
					break;
				case FLAG:
					//putchar('?');
					//putchar('P');
					printf("%-" CEIL_SPACE_STR "c",'P');
					break;
				case DOUBT:
					//putchar('?');
					printf("%-" CEIL_SPACE_STR "c",'?');
					break;
			}
		}
		putchar('\n');
	}

}

int* found=nullptr;


int dfs(int x,int y){

	if(x<0||x>=length||y<0||y>=height){
		return true;
	}
	if(found[x+y*length]){
		return true;
	}
	found[x+y*length]=1;

	status[x+y*length]=OPENED;

	if(lei[x+y*length]){
		return false;
	}

	if(count_lei(x,y)){
		return true;
	}

	//status[x+y*length]=1;

	dfs(x,y-1);
	dfs(x+1,y);
	dfs(x,y+1);
	dfs(x-1,y);
	return true;
}


typedef enum{
	DBCLK_NOT_OPENED=-2,
	DBCLK_LEI_NEQU_FLAGS=-3,
	DBCLK_FAILED=-1,
	DBCLK_SUCC=0

} DoubleClkRetValue;

int double_clk(int x,int y){
	if(status[x+y*length]!=OPENED){
		return DBCLK_NOT_OPENED;
	}
	int leis=count_lei(x,y);

	if(leis==count_flag(x,y)){
		for(int i=-1;i<=1;i++){
			for(int j=-1;j<=1;j++){
				bool failed=false;
				if(!(i==0&&j==0)&&x+i>=0&&x+i<length&&y+j>=0&&y+j<height&&status[x+i+(y+j)*length]==NOT_OPEN){
					failed=!dfs(x+i,y+j);
				}
				if(failed){
					return DBCLK_FAILED;
				}
				
			}
		}
	}
	else{
		return DBCLK_LEI_NEQU_FLAGS;
	}

	return DBCLK_SUCC;
}


bool check_end(){
	int not_found_count=0;
	int flag_count=0;
	bool all_found=true;
	for(int i=0;i<total;i++){
		if(status[i]!=OPENED){
			not_found_count++;
		}
		if(status[i]==FLAG){
			if(!lei[i]){
				all_found=false;
			}
			flag_count++;
		}
	}
	if(not_found_count==lei_count||(all_found&&flag_count==lei_count)){
		return true;
	}
	else{
		return false;
	}
}

int main(){
	srand(time(NULL));
	while(1){
		cout<<"请输入雷区长宽[5,40]:";
		cin>>length>>height;
		if(length<5||length>40||height<5||height>40){
			cout<<"范围错误"<<endl;
			continue;
		}
		total=length*height;
		cout<<"请输入雷数量[0,0.6*length*height]";
		cin>>lei_count;
		if(lei_count<0||lei_count>=total*0.6){
			cout<<"数量错误"<<endl;
			continue;
		}
		putchar('\n');
		gen_map();
		if(found!=nullptr){
			delete[] found;
		}
		found=new int[total];
		bool is_failed=false;
		while(1){
			show_map();

			if(check_end()){
		                cout<<"你找到了所有雷，你赢了"<<endl;
				break;
			}

			if(is_failed){
				cout<<"你被雷炸死了，你输了"<<endl;
				break;
			}

			int x,y,act;
			
			while(1){
				//int x,y;
                cout<<"输入你想点开的格子，和操作码（0打开，1插旗，2问号，3取消，4双击）";
				cin>>x>>y>>act;
				if(0<=x&&x<length&&0<=y&&y<height&&(act==OPEN&&status[x+y*length]==NOT_OPEN||act!=OPEN&&status[x+y*length]!=OPENED||act==DOUBLE_CLK&&status[x+y*length]==OPENED&&count_lei(x,y)!=0)){
					break;
				}
				cout<<"输入的格子有问题"<<endl;	
			}
			if(act==OPEN){
				memset(found,0,sizeof(int)*total);
				is_failed=!dfs(x,y);
				
			}
			else if(act<3){
				status[x+y*length]=act+1;
			}
			else if(act==DOUBLE_CLK){
				int dbclk_ret=double_clk(x,y);

				if(dbclk_ret==DBCLK_FAILED){
					is_failed=true;
				}
				else if(dbclk_ret==DBCLK_LEI_NEQU_FLAGS){
					cout<<"标记数不等于旗子数，无法打开"<<endl;
				}
				// else if(dbclk_ret==DBCLK_)
			}
			else{
				status[x+y*length]=NOT_OPEN;
			}

			
				

		}
	}


    return 0;
}
