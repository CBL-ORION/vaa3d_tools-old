
#include <v3d_interface.h>
#include "v3d_message.h"
#include "stackutil.h"
#include "meanshift_func.h"
#include "node.h"
#include "D:\Vaa3D\v3d_main\neurontracing_vn21\app2/heap.h"
#include "D:\Vaa3D\v3d_main\neuron_editing\v_neuronswc.h"
#include "D:\Vaa3D/sort_neuron_swc/sort_swc.h"
#include "D:\Vaa3D\v3d_main\basic_c_fun\basic_surf_objs.h"
#include "D:\Vaa3D/neurontracing_vn21/app2/my_surf_objs.h"
#include <vector>
#include <iostream>
#include "stackutil.h"

 void construct_tree(QList<Node*> &seeds)
{
	
	//for(QMap<int,QList<Node*>>::iterator iter=finalclass_node.begin();iter!=finalclass_node.end();iter++)
	{
		//if(iter==finalclass_node.begin())
		{
			QList<Node*> seeds=iter.value();
			V3DLONG marknum = seeds.size();

			double** markEdge = new double*[marknum];
			for(int i = 0; i < marknum; i++)
			{
				markEdge[i] = new double[marknum];
				//fprintf(debug_fp,"markEdge[i]:%lf\n",markEdge[i]);
			}

			double x1,y1,z1;
			for (int i=0;i<marknum;i++)
			{
				x1 = seeds.at(i)->x;
				y1 = seeds.at(i)->y;
				z1 = seeds.at(i)->z;
				for (int j=0;j<marknum;j++)
				{
					markEdge[i][j] = sqrt(double(x1-seeds.at(j)->x)*double(x1-seeds.at(j)->x) + double(y1-seeds.at(j)->y)*double(y1-seeds.at(j)->y) + double(z1-seeds.at(j)->z)*double(z1-seeds.at(j)->z));
					//fprintf(debug_fp,"markEdge[i][j]:%lf\n",markEdge[i][j]);
				}
			}//����Ǽ������ӵ�֮��ľ��룬Ӧ�����ڹ���ߣ�������ߵĳ���

			//NeutronTree structure
			NeuronTree marker_MST;
			QList <NeuronSWC> listNeuron;//NeuronSWCӦ���ǽڵ����˼������ڵ㹹��һ����
			QHash <int, int>  hashNeuron;
			listNeuron.clear();
			hashNeuron.clear();

			//set node

			NeuronSWC S;
			S.n 	= 1;
			S.type 	= 3;
			S.x 	= seeds.at(0)->x;
			S.y 	= seeds.at(0)->y;
			S.z 	= seeds.at(0)->z;
			S.r 	= 1;
			S.pn 	= -1;
			listNeuron.append(S);
			hashNeuron.insert(S.n, listNeuron.size()-1);

			int* pi = new int[marknum];
			for(int i = 0; i< marknum;i++)
				pi[i] = 0;
			pi[0] = 1;
			int indexi,indexj;
			for(int loop = 0; loop<marknum;loop++)//���ѭ��ò���������·�����ȴӵ�1���㿪ʼ�������������һ���㣬Ȼ�������㿪ʼ��������㣬�Դ����ơ����Ӧ������С��������ʵ�ִ���
			{
				double min = INF;
				for(int i = 0; i<marknum; i++)
				{
					if (pi[i] == 1)
					{
						for(int j = 0;j<marknum; j++)
						{
							if(pi[j] == 0 && min > markEdge[i][j])
							{
								min = markEdge[i][j];
								indexi = i;
								indexj = j;//������ֵӦ�ñ�ʾ������̵Ķ�������
							}
						}
					}

				}
				if(indexi>=0)//�����if����о��γ���һ�������ڵ�ĸ��ڵ���������������Ľڵ㣬Ҳ������֮ǰ�����һ���ڵ�
				{
					S.n 	= indexj+1;
					S.type 	= 7;
					S.x 	= seeds.at(indexj)->x;
					S.y 	= seeds.at(indexj)->y;
					S.z 	= seeds.at(indexj)->z;
					S.r 	= 1;
					S.pn 	= indexi+1;
					listNeuron.append(S);
					hashNeuron.insert(S.n, listNeuron.size()-1);

				}else
				{
					break;
				}
				pi[indexj] = 1;
				indexi = -1;
				indexj = -1;
			}
			marker_MST.n = -1;
			marker_MST.on = true;
			marker_MST.listNeuron = listNeuron;
			marker_MST.hashNeuron = hashNeuron;

			if(markEdge) {delete []markEdge, markEdge = 0;}
			//writeSWC_file("mst.swc",marker_MST);
			QList<NeuronSWC> marker_MST_sorted;
			if (SortSWC(marker_MST.listNeuron, marker_MST_sorted ,1, 0))//�������Ӧ���ǲ���Ӱ�츸�ڵ����ӽڵ�֮��Ĺ�ϵ��Ӧ��ֻ�ǽ���Щ�ڵ㰴��һ����˳��д��SWC�ļ���
			//return marker_MST_sorted;


		}

	}

}