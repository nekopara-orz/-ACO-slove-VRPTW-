#include "bits/stdc++.h"
using namespace std;
//总点数
#define N 101
//蚂蚁数量
#define M 40 

//运算参数
#define att1 1
#define att2 3
#define att3 2
#define att4 3

double tij[N][N]; //信息素浓度
double nij[N][N]; //能见度
double dis[N][N] = {0};
bool vis[N]; //剩下的待服务的


default_random_engine random(time(NULL));
default_random_engine e;
uniform_real_distribution<double> u;
struct user
{
	int id;
	double pos_x,pos_y;
	int need;
	int ltime,rtime;
	int stime; //服务时间
}usr[N];
void genDis()
{
	for(int i = 0; i < N; i++)
	{
		for(int j = i + 1; j < N; j++)
		{
			double dx = usr[i].pos_x - usr[j].pos_x;
			double dy = usr[i].pos_y - usr[j].pos_y; 
			dis[i][j] = sqrt(dx * dx + dy * dy );
			dis[j][i] = dis[i][j];
			nij[i][j] = nij[j][i] = 1 / dis[i][j];
			tij[i][j] = tij[j][i] = 1;
			//printf("%lf \n", nij[i][j]);
		}
	}
	
}
inline double calfun(double tij,double nij,double width,double wait)
{
	//printf("%lf %lf %lf %lf\n", tij, nij, width, wait);
	//printf("%lf %lf %lf %.20lf\n", pow(tij,att1) , pow(nij, att2) , pow(1 / width,att3) , pow(1 / (wait + 0.1), att4));
	return pow(tij,att1) * pow(nij, att2) * pow(1 / width,att3) * pow(1 / (wait + 0.000001), att4);
	//加0.000001避免除0
}
bool canReach(int &from,int &to,int &onetime,int &carry)
{
	if(vis[to]) return false;

	if(dis[from][to] + onetime > usr[to].rtime) return false;

	if(carry < usr[to].need) return false;

	return true;
}
int genNext(int now,int onetime,vector<int> &nextList)
{
	double r = u(e);
	if (r < 0.5)
	{
		//printf("r < 0.5\n");
		double maxp = 0;
		int to = 0;
		//printf("gennext nextList %d\n", nextList.size());
		for(int &next : nextList)
		{
			 
			double val = calfun(tij[now][next],
			 	 			nij[now][next], 
			 	 			usr[next].rtime - usr[next].ltime, 
			 	 			max(usr[next].ltime - dis[now][next] - onetime, 0.0) ); //避免除0
			//printf("in gennext val = %.20lf \n", val);
			if(val > maxp)
			{
				maxp = val;
				to = next; 
			}
		}
		return to;
	}else
	{
		//printf("r > 0.5\n");
		vector<double> sumlist;
		double sum = 0;
		for(int &next : nextList)
		{
			double val = calfun(tij[now][next],
			 	 			nij[now][next], 
			 	 			usr[next].rtime - usr[next].ltime, 
			 	 			max(usr[next].ltime - dis[now][next] - onetime, 0.0) );
			sum += val;
			sumlist.push_back(sum);
			
			double rdm = u(e);
			double target = rdm * sum;
			
			for(int i = 0; i < sumlist.size(); i++)
			{
				if(target <= sumlist[i])
				{
					//返回第i个
					return nextList[i];
				}
			} 
		}
	}
	return -1;
}
void updateTij(vector<int> path,double len)
{
	//更新信息素
	for (int i = 0; i < path.size() - 1; ++i)
	{
		tij[ path[i] ][ path[i + 1] ] += 5 / len;
	}
}
int main(int argc, char const *argv[])
{
	e = *(new default_random_engine(time(0)));
    u = *(new uniform_real_distribution<double>(0,1));
	freopen("inputset.txt","r",stdin);
	freopen("outputset.txt","w",stdout);
	for(int i = 0; i < N; i++)
	{
		scanf("%d %lf %lf %d %d %d %d", &usr[i].id, &usr[i].pos_x, &usr[i].pos_y,
			&usr[i].need, &usr[i].ltime, &usr[i].rtime, &usr[i].stime);
	}
	genDis();

	memset(vis,0,sizeof(vis));

	vector<int> bestPath; 
	int bestCnt = 100; //最多不会比总点数多
	double bestLen =  -1;

	for (int iter = 0; iter < 100; ++iter)
	{
		cout<<"当前迭代次数："<<iter<<endl;
		 for (int i = 0; i < M; ++i)
		 {
		 	//对每一只蚂蚁
		 	memset(vis,0,sizeof(vis));

		 	vector<int> path = {0};
		 	int length = 0,cnt = 0; //cnt 车的数量
		 	
		 	vector<int> nextList;
			int onetime = 0;
			int alltime = 0;
			int carry = 200;
			int visited = 0;
		 	while(true)
		 	{
		 		if(visited == N - 1 ) break; //N次让其最终能返回
		 		nextList.clear();
		 		int now = path.back(); 
		 		vis[now] = true; //被访问
		 		for(int i = 1; i < N; i++)
		 		{
		 			if(canReach(now,i,onetime,carry))
		 			{
		 				nextList.push_back(i);
		 			}
		 		}
		 		
		 		if (nextList.empty())
		 		{
		 			cnt++; //车辆计数加一
		 			alltime += onetime;
		 			carry = 200;
		 			onetime = 0;
		 			path.push_back(0);
		 			continue;
		 		}
		 		// for(int &t : nextList)
		 		// {
		 		// 	printf("%d ", t);
		 		// }
		 		// printf("List End\n");

		 		int next = genNext(now,onetime,nextList);
		 		//printf("next = %d\n",next);
		 		onetime += dis[now][next] + max(usr[next].ltime - dis[now][next] - onetime, 0.0); // 长度 + wait time
		 		length += dis[now][next];
		 		carry -= usr[next].need;
		 		path.push_back(next);
		 		visited++;
		 	}
		 	if (cnt < bestCnt)
		 	{
		 		bestPath = path;
		 		bestLen = length;
		 		bestCnt = cnt;
		 	}else if(cnt == bestLen)
		 	{
		 		if(length < bestLen)
		 		{
		 			bestPath = path;
		 			bestLen = length;
		 			bestCnt = cnt;
		 		}
		 	}
		 // 	for (int &s : path)
			// {
			// 	printf("%d ", s);
			// }
			// printf("\nlen = %d endl\n", path.size());
		 		
		 }
		updateTij(bestPath,bestLen);
		//sort(bestPath.begin(),bestPath.end());
		for (int &s : bestPath)
		{
			printf("%d ", s);
		}

		printf("\n len = %d endl\n",bestPath.size());
	}

	return 0;
}
