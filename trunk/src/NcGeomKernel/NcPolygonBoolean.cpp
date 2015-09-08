#include <cassert>
#include <math.h>
#include <vector>
#include "NcUtility\NcGlobalDefinitions.h"
#include "NcGeomKernel\NcPolygonBoolean.h"
#include "NcUtility\vector2d.h"



namespace DiscreteSimulator
{


	class line
	{
	public:
		vector2d		*start_pt;
		vector2d		*end_pt;
		
		line			*next;
		line			*prev;
	};



	class polygon
	{
	public:
		int			num_vertex;
		vector2d	*vertex;
		line		*start;
		line		*edge;

	};

	/*STATUS	isSame(const vector2d& v1, const vector2d& v2) const;
	double	signedAngle(const vector2d& a, const line& l) const;*/

}

using namespace DiscreteSimulator;


vector2d midPoint(line& l)
{	
	vector2d m;
	m.v[0] = (l.start_pt->v[0] + l.end_pt->v[0]) / 2.0;
	m.v[1] = (l.start_pt->v[1] + l.end_pt->v[1]) / 2.0;
	return m;
} 


line* new_edge(vector2d *v1, vector2d *v2)
{
	line *edge;
	edge = new line ;
	assert(edge);
	edge->next = edge;
	edge->prev = edge;
	edge->start_pt = v1;
	edge->end_pt = v2;
	return edge;	
}


//NcPolygonBoolean::NcPolygonBoolean()
//{}
//
//NcPolygonBoolean::~NcPolygonBoolean()
//{}


template <class T>
STATUS NcPolygonBoolean::construct_polygon(polygon &poly, std::vector<T>dInputPoly, int iPoint)
{
	int i;
	poly.num_vertex = iPoint;
	poly.vertex = new vector2d[poly.num_vertex + ARRAY_MAX];
	assert( poly.vertex );

	for(i=0; i < poly.num_vertex; i++)
	{
		poly.vertex[i].v[0] = dInputPoly[i][0];
		poly.vertex[i].v[1] = dInputPoly[i][1];
	}
	
	for(i=0; i < poly.num_vertex - 1; i++)
	{
		if(i == 0)
		{
			poly.start = poly.edge = new_edge(&poly.vertex[i], &poly.vertex[i+1]);
		}
		else
		{ 
			poly.edge->next = new_edge(&poly.vertex[i], &poly.vertex[i+1]);
			poly.edge->next->prev = poly.edge;

			//this will excute once to make list circular
			if(poly.edge->prev == poly.edge)
				poly.edge->prev = poly.edge->next;
			
			poly.edge->next->next = poly.start;	
			poly.start->prev = poly.edge->next;
		
		#ifdef debug
			fprintf(stderr,"prev edge %u  prev %u next %u start %u\n",
				poly.edge,poly.edge->prev,poly.edge->next,poly.start);
		#endif
			poly.edge = poly.edge->next;
		}
		#ifdef debug
			fprintf(stderr,"edge %u  prev %u next %u start %u start->prev %u\n",
				poly.edge,poly.edge->prev,poly.edge->next,poly.start,poly.start->prev);
		#endif
		
		//poly.edge->next=NULL;//this make it linear
	}
	poly.edge = poly.start;
	return OK;
}


STATUS NcPolygonBoolean::boolean_main(std::vector<NcVector> dTargetPoly, int iTargetPoints,
									  std::vector<vector2d> dToolPoly, int iToolPoints,
                                      double dModiTargetPoly[][2], int &iMoadiTargetPoints)
{
	polygon target, tool;
	iMoadiTargetPoints = 0;

#ifdef debug
	debug_boolean=fopen("boolean.debug", "w+");
#endif

	construct_polygon(target, dTargetPoly, iTargetPoints);
	construct_polygon(tool, dToolPoly, iToolPoints);

#ifdef debug
	display_polygon(target,tool,output_handle);
#endif

	intersect_polygon(target, tool);

#ifdef debug
	display_polygon(target,tool,output_handle);
#endif

	polgon_boolean(target, tool);

#ifdef debug
	display_polygon(target,tool,output_handle);
#endif
		
	dModiTargetPoly[iMoadiTargetPoints][0] = target.start->start_pt->v[0];
	dModiTargetPoly[iMoadiTargetPoints][1] = target.start->start_pt->v[1];
	iMoadiTargetPoints++;

	for(target.edge = target.start; target.edge != NULL; target.edge = target.edge->next)
	{
		dModiTargetPoly[iMoadiTargetPoints][0] = target.edge->end_pt->v[0];
		dModiTargetPoly[iMoadiTargetPoints][1] = target.edge->end_pt->v[1];
		iMoadiTargetPoints++;
		if(target.start->prev == target.edge)
			break;
	}
	delete [] target.vertex;
	delete [] tool.vertex;
//	fclose(debug_boolean);

#ifdef debug
        fclose(boolean.debug);
#endif
        //free_list(target)
	//free_list(tool)
	return OK;
}



INT_STATUS NcPolygonBoolean::intersect_line(line *p, line *q, double t[2])
{	
	double a1 = 0, a2 = 0;
	double b1 = 0, b2 = 0;
	double c1 = 0, c2 = 0;

	double sqr_det, det_temp, det, Q_t1, Q_t2, P_t1, P_t2;

	a1 = p->end_pt->v[0] - p->start_pt->v[0];
	b1 = q->end_pt->v[0] - q->start_pt->v[0];
	a2 = p->end_pt->v[1] - p->start_pt->v[1];
	b2 = q->end_pt->v[1] - q->start_pt->v[1];
	c1 = -p->start_pt->v[0] + q->start_pt->v[0];
	c2 = -p->start_pt->v[1] + q->start_pt->v[1];

	det = (b1 * a2) - (b2 * a1);
	sqr_det = ((b1 * a2) - (b2 * a1)) * ((b1 * a2) - (b2 * a1));
	det_temp = (a1 * c2 - a2 * c1) * (a1 * c2 - a2 * c1);
	
	if(sqr_det <= TOL)
	{
		if(det_temp > TOL)
		{	
		#ifdef debug
			fprintf(stderr,"parallel lines\n");
		#endif
			return I_FAIL;
		}
		if(det_temp <= TOL)
		{
			if(fabs(p->end_pt->v[0] - p->start_pt->v[0]) <= TOL)
			{	
				// check if lines are vertical
				Q_t1 = ((q->start_pt->v[1] - p->start_pt->v[1])/
					   (p->end_pt->v[1] - p->start_pt->v[1]));

				Q_t2 = ((q->end_pt->v[1] - p->start_pt->v[1])/
					   (p->end_pt->v[1]  - p->start_pt->v[1]));

				P_t1 = ((p->start_pt->v[1] - q->start_pt->v[1])/
					   (q->end_pt->v[1] - q->start_pt->v[1]));

				P_t2 = ((p->end_pt->v[1] - q->start_pt->v[1])/
					   (q->end_pt->v[1]  - q->start_pt->v[1]));
			
			}
			else
			{
				Q_t1 = ((q->start_pt->v[0] - p->start_pt->v[0])/
					    (p->end_pt->v[0] - p->start_pt->v[0]));

				Q_t2 = ((q->end_pt->v[0] - p->start_pt->v[0])/
					    (p->end_pt->v[0] - p->start_pt->v[0]));

				P_t1 = ((p->start_pt->v[0] - q->start_pt->v[0])/
					    (q->end_pt->v[0] - q->start_pt->v[0]));

				P_t2 = ((p->end_pt->v[0] - q->start_pt->v[0])/
					    (q->end_pt->v[0] - q->start_pt->v[0]));
			}

			#ifdef debug
				fprintf(stderr,"Q_t1 %lf Q_t2 %lf P_t1 %lf P_t2 %lf\n",Q_t1,Q_t2,P_t1,P_t2);
			#endif		
			
			if((((Q_t1 + PARA_TOL) > 0.0) && ((Q_t1 + PARA_TOL) < 1.0))
			&&(((Q_t2 + PARA_TOL) > 0.0) && ((Q_t2 + PARA_TOL) < 1.0)))
			{
			#ifdef debug
				fprintf(stderr,"Q0Q1 is lying inside P0P1-- Not Occur\n");
			#endif	
				t[0] = Q_t1;
				t[1] = Q_t2;
				return BOTH_ON_TARGET;	
			}
			else if(!(((Q_t1 + PARA_TOL) > 0.0) && ((Q_t1 + PARA_TOL) < 1.0))
					 &&(((Q_t2 + PARA_TOL) > 0.0) && ((Q_t2 + PARA_TOL) < 1.00)))
			{
			#ifdef debug
				fprintf(stderr,"only Q1 lies on P0P1 and not Q0\n");
			#endif	
				t[0] = Q_t2;
				t[1] = P_t1;
			}
			else if((((Q_t1 + PARA_TOL) > 0.0) && ((Q_t1 + PARA_TOL) < 1.0))
				   && !(((Q_t2 + PARA_TOL) > 0.0) && ((Q_t2 + PARA_TOL) < 1.0)))
			{
			#ifdef debug
				fprintf(stderr,"only Q0 lies on P0P1 and not Q1\n");
			#endif	
				t[0] = Q_t1;
				t[1] = P_t2;
			}
			else if( (!(((Q_t1 + PARA_TOL) > 0.0) && ((Q_t1 + PARA_TOL) < 1.0))
				        && !(((Q_t2 + PARA_TOL) > 0.0) && ((Q_t2+PARA_TOL) < 1.0)))
						&&((((P_t1 + PARA_TOL) > 0.0) && ((P_t1+PARA_TOL) < 1.0))
						&&(((P_t2 + PARA_TOL) > 0.0) && ((P_t2+PARA_TOL) < 1.0))))
			{
				#ifdef debug
					fprintf(stderr,"seg P lies complete inside Q\n");
				#endif	
				t[0] = P_t1;
				t[1] = P_t2;
				return BOTH_ON_TOOL;	
			}
			else if((fabs(Q_t1 - 0.0) < PARA_TOL) && (fabs(Q_t2 - 1.00) < PARA_TOL))
			{
				#ifdef debug
				 fprintf(stderr,"P0P1 and Q0Q1 are coincident \n");
				#endif	
				return I_FAIL;
			}
			else if((!(((Q_t1+PARA_TOL)>0.0) && ((Q_t1-PARA_TOL)<1.0))
					|| !(((Q_t2+PARA_TOL)>0.0) && ((Q_t2-PARA_TOL)<1.0)))
					|| (!(((P_t1+PARA_TOL)>0.0) && ((P_t1-PARA_TOL)<1.0))
					|| !(((P_t2+PARA_TOL)>0.0) && ((P_t2-PARA_TOL)<1.0))))
			{
				#ifdef debug
					fprintf(stderr,"Lines coincident but segment not intersecting\n");
				#endif	
				return I_FAIL;
			}
			#ifdef debug
				fprintf(stderr,"coincident lines Q_t1 %lf Q_t2 %lf P_t1 %lf P_t2 %lf\n",
				Q_t1, Q_t2, P_t1, P_t2);
			#endif	
			return I_OK;
		}
	}
	else
	{
		t[0] = (c2 * b1 - c1 * b2) / det;
		t[1] = (a1 * c2 - a2 * c1) / det;
		#ifdef debug
			fprintf(stdout,"t[0]=%24.20lf t[1]=%24.20lf\n",t[0],t[1]);
		#endif	
/*		if((t[0]>=0.0 && t[0]<=1.0) && (t[1]>0.0 && t[1]<1.0))
		{
			fprintf(stdout,"BOTH_ON_TARGET t[0]=%lf t[1]=%lf\n",t[0],t[1]);
			return I_OK;
		}
		else if((t[0]>0.0 && t[0]<1.0) && (t[1]>=0.0 && t[1]<=1.0))
		{
			fprintf(stdout,"BOTH_ON_TOOL t[0]=%lf t[1]=%lf\n",t[0],t[1]);
			return I_OK;
		}
		else if((t[0]>=0.0 && t[0]<=1.0) && (t[1]>=0.0 && t[1]<=1.0))
		{
			fprintf(stdout,"UNHANDLED case t[0]=%lf t[1]=%lf\n",t[0],t[1]);
			getchar();
			return I_OK;
		}
		else */
		if((t[0]>0.0 && t[0]<=1.0) && (t[1]>0.0 && t[1]<1.0))
		{
			//fprintf(stdout,"crossing t[0]=%lf t[1]=%lf\n",t[0],t[1]);
			return I_OK;
		}
		else
		{
			#ifdef debug
				fprintf(stderr,"Intersections are not real\n");
			#endif	
			return I_FAIL;
		}
		
	}
	return I_FAIL;
}



STATUS isSame(const vector2d& v1, const vector2d& v2)
{
	if((fabs(v1.v[0] - v2.v[0]) < TOL && fabs(v1.v[1] - v2.v[1]) <TOL))	
		return OK;
	else 
		return FAIL;

}




int NcPolygonBoolean::add_vertex(polygon &target, vector2d v)
{
	for(int i=0; i<target.num_vertex; i++)
	{
		if(isSame(target.vertex[i], v)==OK)
			return i;
	}	
	#ifdef debug
		fprintf(stderr,"add_vertex num_vertex %d\n",target.num_vertex);	
	#endif	
	target.vertex[target.num_vertex] = v;
	target.num_vertex++;
	return target.num_vertex - 1;
}


int  NcPolygonBoolean::add_vertex(polygon &target, line edge, double t)
{
	vector2d p1, p2, p;
	
	p1 = *edge.start_pt;
	p2 = *edge.end_pt;
	
	p.v[0] = p1.v[0] + t * (p2.v[0] - p1.v[0]);
	p.v[1] = p1.v[1] + t * (p2.v[1]-p1.v[1]);
#ifdef debug	
	fprintf(stderr,"Add vertex %d\n",target.num_vertex);
	fprintf(stderr,"new p %lf %lf\n",p.v[0],p.v[1]);
#endif
	for(int i = 0; i < target.num_vertex; i++)
	{
		if(isSame(target.vertex[i],p)==OK)
			return -1;
	}	
	target.vertex[target.num_vertex] = p;
	target.num_vertex++;
	//printf("num_vertex %d\n",target.num_vertex);
	return target.num_vertex - 1;
}




STATUS NcPolygonBoolean::intersect_polygon(polygon &target, polygon &tool)
{	
	double t[2];
	int iVertex;
#ifdef debug
	fprintf(stderr,"original target %d\n",target.num_vertex);
//	debug_list(target);
#endif	
	for(target.edge = target.start; target.edge != NULL; target.edge = target.edge->next)
	{
#ifdef debug
	print_edge("new target",*target.edge);
#endif	
		for(tool.edge = tool.start; tool.edge != NULL; tool.edge = tool.edge->next)
		{
		#ifdef debug
			print_edge("target",*target.edge);
			print_edge("tool",*tool.edge);
		#endif	
			//before going for intersection,check whether target edge vertex is matching 
			// with any of tool edge vertex, then skip this tool edge
			INT_STATUS status;	
			status = intersect_line(target.edge, tool.edge, t);
			if(status == I_FAIL)
			{	
			#ifdef debug
				fprintf(stderr,"not intersecting\n");
			#endif	
			}	
			else if(status==I_OK)
			{
			#ifdef debug
				fprintf(stderr,"I am in I_OK\n");	
			#endif	
				iVertex = add_vertex(target, *target.edge, t[0]);
				if(iVertex != -1)
				{
					line *l = new_edge(&target.vertex[iVertex], target.edge->end_pt); 
					
					//creating new edge with iVertex v2
					target.edge->end_pt = &target.vertex[iVertex]; //change existing v2 to new v
					l->next = target.edge->next;
					l->next->prev = l;
					l->prev = target.edge;
					target.edge->next = l;
				}
			#ifdef debug
				fprintf(stderr,"target num_vertex %d\n",target.num_vertex);	
			#endif	
				iVertex=add_vertex(tool,*tool.edge,t[1]);
                if(iVertex != -1)
				{
                    line *l = new_edge(&tool.vertex[iVertex], tool.edge->end_pt); 
					//creating new edge with iVertex v2
                    tool.edge->end_pt = &tool.vertex[iVertex]; //change existing v2 to new v
                    l->next = tool.edge->next;
					l->next->prev = l;
					l->prev = tool.edge;
                    tool.edge->next = l;
                 }
			#ifdef debug
				fprintf(stderr,"tool num_vertex %d\n",tool.num_vertex);	
			#endif	
				//break;
			}
			else if(status == BOTH_ON_TARGET)
			{
			#ifdef debug
				fprintf(stderr,"I am in BOTH_ON_TARGET\n");	
			#endif	
				if(t[0] < t[1])
					iVertex = add_vertex(target, *target.edge, t[0]);
				else 
					iVertex = add_vertex(target, *target.edge, t[1]);
				if(iVertex != -1)
				{
					line *l = new_edge(&target.vertex[iVertex], target.edge->end_pt); 
					target.edge->end_pt = &target.vertex[iVertex]; //change existing v2 to new v
					l->next = target.edge->next;
					l->next->prev = l;
					l->prev = target.edge;
					target.edge->next = l;
				}
			#ifdef debug
				fprintf(stderr,"target num_vertex %d\n",target.num_vertex);	
			#endif	
				target.edge = target.edge->next;
				if(t[1]>t[0])
					iVertex = add_vertex(target,*target.edge,t[1]);
				else 
					iVertex = add_vertex(target,*target.edge,t[0]);
				if(iVertex != -1)
				{
					line *l = new_edge(&target.vertex[iVertex], target.edge->end_pt); 
					target.edge->end_pt = &target.vertex[iVertex]; //change existing v2 to new v
					l->next = target.edge->next;
					l->next->prev = l;
					l->prev = target.edge;
					target.edge->next = l;
				}
			#ifdef debug
				fprintf(stderr,"target num_vertex %d\n",target.num_vertex);	
			#endif	
				break;
			}
			else if(status == BOTH_ON_TOOL)
			{
			#ifdef debug
				fprintf(stderr,"I am in BOTH_ON_TOOL\n");	
			#endif	
				if(t[0] < t[1])
					iVertex = add_vertex(tool,*tool.edge,t[0]);
				else 
					iVertex = add_vertex(tool,*tool.edge,t[1]);
				if(iVertex != -1)
				{
					line *l = new_edge(&tool.vertex[iVertex], tool.edge->end_pt); 
					tool.edge->end_pt = &tool.vertex[iVertex]; //change existing v2 to new v
					l->next = tool.edge->next;
					l->next->prev = l;
					l->prev = tool.edge;
					tool.edge->next = l;
				}
			#ifdef debug
				fprintf(stderr,"tool num_vertex %d\n",tool.num_vertex);	
			#endif	
				tool.edge = tool.edge->next;
				if(t[1] > t[0])
					iVertex = add_vertex(tool, *tool.edge, t[1]);
				else 
					iVertex = add_vertex(tool, *tool.edge, t[0]);
				if(iVertex != -1)
				{
					line *l = new_edge(&tool.vertex[iVertex], tool.edge->end_pt); 
					tool.edge->end_pt=&tool.vertex[iVertex]; //change existing v2 to new v
					l->next = tool.edge->next;
					l->next->prev = l;
					l->prev = tool.edge;
					tool.edge->next = l;
				}
			#ifdef debug
				fprintf(stderr,"tool num_vertex %d\n",tool.num_vertex);	
			#endif	
				break;
			}
			if(tool.start->prev == tool.edge)
				break;
		}
	#ifdef debug
		fprintf(stderr,"after intersection target %d\n",target.num_vertex);
		//debug_list(target);
		fprintf(stderr,"\nafter intersection tool is:%d\n",tool.num_vertex);
		//debug_list(tool);
	#endif	
		if(target.start->prev == target.edge)
			break;
	}
	return OK;
}


STATUS NcPolygonBoolean::polgon_boolean(polygon &target, polygon tool)
{
	int count_d=0,count_a=0,i=0;
	int found_end=0;
	int sandip=0;
	/*char tp_mesg[80];*/
	line *tobe_deleted[ARRAY_MAX];
	line *tobe_added[ARRAY_MAX];
	for(target.edge=target.start;target.edge!=NULL;target.edge=target.edge->next)
	{
		if(pointInPolygon(tool,*target.edge->start_pt)==OUTSIDE){
			target.start=target.edge;
			break;
		}
	}

	for(target.edge=target.start;target.edge!=NULL;target.edge=target.edge->next)
	{
		/*sprintf(tp_mesg,"target%d",sandip++);*/
		//print_edge(tp_mesg,*target.edge);
		if(pointInPolygon(tool,*target.edge->end_pt)==INSIDE &&
		  pointInPolygon(tool,midPoint(*target.edge))==INSIDE)
		{
			#ifdef debug
			fprintf(stderr,"case 1\n");
			#endif
			tobe_deleted[count_d++]=target.edge;
			assert(count_d<ARRAY_MAX);
			if(pointInPolygon(tool,midPoint(*target.edge->next))==OUTSIDE)
				found_end=1;
		}
		else if(pointInPolygon(tool,*target.edge->start_pt)==BOUNDARY&&
			pointInPolygon(tool,*target.edge->end_pt)==BOUNDARY)
		{
			#ifdef debug
			fprintf(stderr,"case 2\n");
			#endif
			tobe_deleted[count_d++]=target.edge;
			assert(count_d<ARRAY_MAX);
			if(pointInPolygon(tool,*target.edge->next->end_pt)==OUTSIDE)
				found_end=1;
		}
		else if(pointInPolygon(tool,midPoint(*target.edge))==INSIDE)
		{
			#ifdef debug
			fprintf(stderr,"case 3\n");
			#endif
			tobe_deleted[count_d++]=target.edge;
			assert(count_d<ARRAY_MAX);
			if(pointInPolygon(tool,*target.edge->next->end_pt)==OUTSIDE)
				found_end=1;
		}
		if(found_end==1||
			(pointInPolygon(tool,*target.edge->start_pt)==INSIDE&&
			pointInPolygon(tool,*target.edge->end_pt)==BOUNDARY)) 
		{
			#ifdef debug
			fprintf(stderr,"case end\n");
			#endif
			if(!found_end)
				tobe_deleted[count_d++]=target.edge;
			assert(count_d<ARRAY_MAX);
			found_end=0;
			//print_vertex("v1",*tobe_deleted[0]->start_pt);	
			//print_vertex("v2",*tobe_deleted[count_d-1]->end_pt);	
			find_tool_edge_tobeadded(tool,target,
				*tobe_deleted[0]->start_pt,*tobe_deleted[count_d-1]->end_pt,tobe_added,count_a);
			//start stiching 
			for(i=0,target.edge=tobe_deleted[0]->prev;i<count_a;i++)
			{
				int iVertex1=add_vertex(target,*tobe_added[i]->end_pt);
				int iVertex2=add_vertex(target,*tobe_added[i]->start_pt);
				target.edge->next=new_edge(&target.vertex[iVertex1],&target.vertex[iVertex2]); 
				target.edge->next->prev=target.edge;
				target.edge=target.edge->next;
			}
			target.edge->next=tobe_deleted[count_d-1]->next;
			tobe_deleted[count_d-1]->next->prev=target.edge;
			#ifdef debug
			fprintf(stderr,"Done stiching\n");
			#endif
		}
		if(target.start->prev==target.edge)
			break;

		#ifdef debug
		fprintf(stderr,"count_d %d\n",count_d);
		#endif
	}
	return OK;
}


double polarAngle(vector2d p)
{
	if((p.v[0]==0.0) &&(p.v[1]==0.0))
		return -1.0;
	if(p.v[0]==0.0)
		return((p.v[1]>0.0)?90:270);
	double theta = atan(p.v[1]/p.v[0]);	//in radians
	
	theta *=360.0/(2.0 * NC_PI);		//convert to degrees	
	if(p.v[0]>0.0)			//quadrants 1 and 4
		return((p.v[1]>=0.0)? theta : 360 + theta);
	else			// quadrants 2 and 3
		return(180 + theta);
}



double signedAngle(const vector2d& a, const line& l)
{
	vector2d u,w;
	u.v[0] = l.start_pt->v[0] - a.v[0];
	u.v[1] = l.start_pt->v[1] - a.v[1];

	w.v[0] = l.end_pt->v[0] - a.v[0];
	w.v[1] = l.end_pt->v[1] - a.v[1];

	double ua = polarAngle(u);
	double wa = polarAngle(w);
	
	if((ua==-1.0)||(wa==-1.0))
		return 180.0;

	double x = wa-ua;
	if((x == 180.0) || (x == -180.0))
		return 180.0;
	else if(x<-180.0)
		return (x+360.0);
	else if(x>180.0)
		return (x-360.0);
	else 
		return x;
}




int NcPolygonBoolean::pointInPolygon(polygon& target, vector2d& p)
{
	double total=0.0;
	for(target.edge = target.start; target.edge != NULL; target.edge = target.edge->next)
	{
		double x = signedAngle(p,*target.edge);
		if((fabs(x - 180.0) < 0.0001))
		{
			#ifdef debug
			fprintf(stderr,"BOUNDARY\n");
			#endif
			return BOUNDARY;
		}
		total += x;
		if(target.start->prev == target.edge)
			break;
	}
	if(fabs(total - 360.0) < 0.0001)
	{
	#ifdef debug
		fprintf(stderr,"INSIDE\n");
	#endif
		return INSIDE;
	}
	else
	{
	#ifdef debug
		fprintf(stderr,"OUTSIDE\n");
	#endif
		return OUTSIDE;
	}
}






STATUS NcPolygonBoolean::find_tool_edge_tobeadded(polygon tool, polygon target, vector2d v1, vector2d v2,
												  line *tobe_added[], int &count_a)
{
	count_a=0;	

	//find start edge
	for(tool.edge=tool.start;tool.edge!=NULL;tool.edge=tool.edge->next)
	{
		if(isSame(*tool.edge->end_pt,v1)==OK)
			break;	
		if(tool.start->prev==tool.edge)
		{
			#ifdef 	debug	
			fprintf(stderr,"Error This should not happen\n");
			getchar();
			#endif
			break;
		}
	}
	
	for(tool.edge;tool.edge!=NULL;tool.edge=tool.edge->prev)
	{
		tobe_added[count_a++]=tool.edge;
		if(count_a>(ARRAY_MAX/2))
		{	
			#ifdef 	debug	
			fprintf(stderr,"Error in find_tool_edge_tobeadded %d\n",count_a);
			#endif
			return FAIL;
			
		}
		//assert(count_a<ARRAY_MAX);
		if(isSame(*tool.edge->start_pt,v2)==OK)
		{
			#ifdef 	debug	
			fprintf(stderr,"Found %d edge to be added\n",count_a);
			#endif
			return OK;
		}
	}
	return FAIL;
}
//double& vector2d::operator [](const int index)
//{
//	if(index == 0) return v[0];
//	else if(index == 1) return v[1];
//	
//}
//
//const double& vector2d::operator[](const int index) const
//{
//	return (*this)[index];
//	/*if(index == 0) return vx;
//	else if(index == 1) return vy;
//	else if(index == 2) return vz;*/
//}