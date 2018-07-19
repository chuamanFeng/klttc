#include "StdAfx.h"
#include "datatrans.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv/highgui.h"
#include "math.h"
#include "objectFind.h"

int numelli =5;

/*----------------------------------------------------------------------------*/
bool comp(const pair<int,float>& a,const pair<int,float>& b)
{
	return a.second>b.second;
}


datatrans::datatrans(bool svm,double minarea,double maxarea,double c1,double T,Mat camMatrix,Mat distCoeff,int he,int wi)
{
	minArea=minarea;
	maxArea=maxarea;
	circleli=c1;
	Tr=T;
	ok=no=0;
	h=he;
	w=wi;
	cam=camMatrix;
	disc=distCoeff;
	svmFLAG=svm;
	if(!OBj.empty())
	{
		OBj.clear();
	}	

	OBj.push_back(Point3f(0.f,20.f,0.f));
	OBj.push_back(Point3f(0.f,0.f,0.f));
	OBj.push_back(Point3f(0.f,-40.f,0.f));
	OBj.push_back(Point3f(30.f,0.f,0.f));
	OBj.push_back(Point3f(60.f,0.f,0.f));
	
	if(svmFLAG)
	{
		objectFind svmserve(7);
		if(svmserve.built)
		{
			/*svmserve.make_pos_neg();
			svmserve.build_vocab();
			svmserve.train_classifiers();
			AfxMessageBox(_T("SHADIAO12"));*/
			categories=svmserve.categories;
			svmserve.category_names.swap(category_names);
			svmserve.svms.swap(svms);
			AfxMessageBox(_T("SHADIAO13"));
			featureDetector=svmserve.featureDetector;
			bowDescriptorExtractor =svmserve.bowDescriptorExtractor;
			AfxMessageBox(_T("SHADIAO14"));
			count4sq=0;
		}
		else
		{
			AfxMessageBox(_T("No Data For SVM"));
			svmFLAG=false;
		}
	}
}
	

datatrans::~datatrans(void)
{
}

void datatrans::imgPros(Mat res,Rect roi)
{
	
	int flagsqu=0,repeatflag=0;
	Mat gray;
	Rect ROI;
	cvtColor(res, gray, CV_BGR2GRAY);
	ROI=roi;		
	contours.clear();
	vector<Mat>::iterator itA= contours.begin();
	
	/***********************square searching&fillting contours*************************/
	Mat* square=NULL;	     
	vector<Mat> tempcon;
	

	while(flagsqu==0&&repeatflag<3)
	{				
		Mat INI=performThreshold(Mat(gray,ROI));		
		tempcon.clear();
		findContours(INI,contours,CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);

		int siz=contours.size();
		for(int i=0;i<siz;i++)
		{			

			double area=contourArea(contours[i]);
			double biggist=0.0;


			if(area<=maxArea&&area>=minArea)
			{	
				tempcon.push_back(contours[i]);
				if(area>biggist)
				{
					biggist=area;
					square=&contours[i];					
				}
			}
			else
			{
				continue;						
			}

		}

		if(square!=NULL)
		{				
			vector<Point> approxCurve;		   
			approxPolyDP(mat2vec(*square),approxCurve,15,true);

			if(approxCurve.size()<=6&&approxCurve.size()>=4)
			{
				flagsqu=1;
				break;
			}
		}
		else
		{			
			ROI.x=(int)(ROI.x-0.5*ROI.width);
			ROI.y=(int)(ROI.y-0.5*ROI.height);
			ROI.width=(int)(1.5*ROI.width);
			ROI.height=(int)(1.5*ROI.height);//SHOULD HAVE BEEN MORE RESONABLE

			if(ROI.x<0)
			{
				ROI.x=0;
			}
			if(ROI.y<0)
			{
				ROI.y=0;
			}
			if(ROI.y-ROI.height<0)
			{
				ROI.height=ROI.y;
			}
			if(ROI.x-ROI.width<0)
			{
				ROI.width=ROI.x;
			}
		}


		square=NULL;
		contours.clear();			
		repeatflag+=1;

	}		

	if(flagsqu==0)
	{
		//AfxMessageBox(_T("target missing"));	
		no++;		
		return;


	}
	/*******************************extract ellipses********************************/
	double areaplus=0.0;	
	vector<double> areas;
	for(itA=contours.begin();itA!=contours.end();itA++)
	{		
		Point2f center;
		float R;
		minEnclosingCircle(*itA,center,R);		
		double area=contourArea(*itA);
		if(circlelike(area,R))//COME HERE
		{
			//AfxMessageBox(_T("SHADIAO12"));
			tempcon.push_back(*itA);
			areas.push_back(area);
			areaplus+=area;
			//itA=contours.erase(itA);
			//itA-1;
		}		
	}

	contours.clear();
	contours.insert(contours.end(),tempcon.begin(),tempcon.end());

	tempcon.clear();

	int csz=contours.size();
	if(csz<numelli)//this judge is for debug
	{		
		no++;
		return;
	}
	if(csz>numelli)//this judge is for debug
	{	
		double areaave=areaplus/csz;
		vec4cal(areaave,areas);
		AfxMessageBox(_T("GOOD"));
		vector<pair<int,double>> contpairs;
		pair<int,double> tempp;
		for(int i=0;i<csz;i++)
		{
			tempp.first=i;
			tempp.second=areas[i];
			contpairs.push_back(tempp);
		}
		sort(contpairs.begin(),contpairs.end(),compar1);
		vector<Mat> CONTOURSTEMP;
		for(int i=0;i<numelli;i++)
		{
			CONTOURSTEMP.push_back(contours[contpairs[i].first]);
		}
		CONTOURSTEMP.swap(contours);

	}
	/************************************&centers*********************************/	
	centers.clear();
	Mat INI1(gray,ROI);
	for(itA=contours.begin();itA!=contours.end();itA++)
	{		
		AfxMessageBox(_T("SHADIAO22"));
		Point ct=getcenter(INI1,ROI.tl(),(*itA));	
		centers.push_back(ct);
	}
	int sz=centers.size();
	if(sz!=numelli)
	{
		no++;
		//AfxMessageBox(_T("center group with wrong number"));
		return;
	}
	/*************************************pose-handle****************************/
	int rl=relats_bt_centers(centers);
	AfxMessageBox(_T("SHADIAO23"));
	if(rl==1)
	{
		Mat_<float> temp;
		temp=solve_pnp(centers);
		AfxMessageBox(_T("SHADIAO24"));
		pos_res.push_back(temp);
		ok++;
		return;
	}
	else
	{
		no++;
		return;
	}	

}
	
/**********************************---------------with svm--------------------*******************************************/
void datatrans::imgProssvm(Mat res,Rect roi)
{
	AfxMessageBox(_T("SHADIAO15"));
	int flagsqu=0,repeatflag=0;
	Mat gray;
	Rect ROI;
	cvtColor(res, gray, CV_BGR2GRAY);
	ROI=roi;		
	vector<Mat>::iterator itA= contours.begin();
	/***********************square searching&fillting contours*************************/
	Mat* square=NULL;	     
	vector<Mat> tempcon;
	contours.clear();
	while(flagsqu==0&&repeatflag<3)
	{				
		Mat INI=performThreshold(Mat(gray,ROI));		
		tempcon.clear();
		findContours(INI,contours,CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);

		int siz=contours.size();
		for(int i=0;i<siz;i++)
		{			

			double area=contourArea(contours[i]);
			double biggist=0.0;


			if(area<=maxArea&&area>=minArea)
			{	
				tempcon.push_back(contours[i]);
				if(area>biggist)
				{
					biggist=area;
					square=&contours[i];					
				}
			}
			else
			{
				continue;						
			}

		}

		if(square!=NULL)
		{				
			vector<Point> approxCurve;		   
			approxPolyDP(mat2vec(*square),approxCurve,15,true);

			if(approxCurve.size()<=6&&approxCurve.size()>=4)
			{
				flagsqu=1;
				break;
			}
		}
		else
		{			
			ROI.x=(int)(ROI.x-0.5*ROI.width);
			ROI.y=(int)(ROI.y-0.5*ROI.height);
			ROI.width=(int)(1.5*ROI.width);
			ROI.height=(int)(1.5*ROI.height);//SHOULD HAVE BEEN MORE RESONABLE

			if(ROI.x<0)
			{
				ROI.x=0;
			}
			if(ROI.y<0)
			{
				ROI.y=0;
			}
			if(ROI.y-ROI.height<0)
			{
				ROI.height=ROI.y;
			}
			if(ROI.x-ROI.width<0)
			{
				ROI.width=ROI.x;
			}
		}


		square=NULL;
		contours.clear();			
		repeatflag+=1;

	}		

	if(flagsqu==0)
	{
		//AfxMessageBox(_T("target missing"));	
		no++;
		count4sq++;
		error_tp.push_back(gray);
		if(count4sq==5)
		{
			int p=0,f=0;
			vector<Mat>::iterator t=error_tp.begin();
			while(t!=error_tp.end())
			{
				if(categorize(*t))
				{
					p++;
				}
				else
				{
					f++;
				}
				t++;
			}
			if(p<f)
			{
				AfxMessageBox(_T("target lost"));
				error_tp.clear();
				count4sq=-1;
				
			}
			else
			{
				count4sq=0;
				error_tp.clear();
				
			}
		}
		return;


	}
	/*******************************extract ellipses********************************/
	double areaplus=0.0;	
	vector<double> areas;
	for(itA=contours.begin();itA!=contours.end();itA++)
	{		
		Point2f center;
		float R;
		minEnclosingCircle(*itA,center,R);		
		double area=contourArea(*itA);
		if(circlelike(area,R))//COME HERE
		{
			//AfxMessageBox(_T("SHADIAO12"));
			tempcon.push_back(*itA);
			areas.push_back(area);
			areaplus+=area;
			//itA=contours.erase(itA);
			//itA-1;
		}		
	}

	contours.clear();
	contours.insert(contours.end(),tempcon.begin(),tempcon.end());

	tempcon.clear();

	int csz=contours.size();
	if(csz<numelli)//this judge is for debug
	{		
		no++;
		return;
	}
	if(csz>numelli)//this judge is for debug
	{	
		double areaave=areaplus/csz;
		vec4cal(areaave,areas);
		AfxMessageBox(_T("GOOD"));
		vector<pair<int,double>> contpairs;
		pair<int,double> tempp;
		for(int i=0;i<csz;i++)
		{
			tempp.first=i;
			tempp.second=areas[i];
			contpairs.push_back(tempp);
		}
		sort(contpairs.begin(),contpairs.end(),compar1);
		vector<Mat> CONTOURSTEMP;
		for(int i=0;i<numelli;i++)
		{
			CONTOURSTEMP.push_back(contours[contpairs[i].first]);
		}
		CONTOURSTEMP.swap(contours);

	}
	/************************************&centers*********************************/	
	centers.clear();
	Mat INI1(gray,ROI);
	for(itA=contours.begin();itA!=contours.end();itA++)
	{		
		AfxMessageBox(_T("SHADIAO22"));
		Point ct=getcenter(INI1,ROI.tl(),(*itA));	
		centers.push_back(ct);
	}
	int sz=centers.size();
	if(sz!=numelli)
	{
		no++;
		//AfxMessageBox(_T("center group with wrong number"));
		return;
	}
	/*************************************pose-handle****************************/
	int rl=relats_bt_centers(centers);
	AfxMessageBox(_T("SHADIAO23"));
	if(rl==1)
	{
		Mat_<float> temp;
		temp=solve_pnp(centers);
		AfxMessageBox(_T("SHADIAO24"));
		pos_res.push_back(temp);
		ok++;
		return;
	}
	else
	{
		no++;
		return;
	}	

}	
	
/******************************************basic IMGPROC**********************************************************************/
Mat datatrans::performThreshold(Mat res)
{
	Mat result;
	GaussianBlur(res,result,Size(3,3),0,0);
	cv::adaptiveThreshold(res,result,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY_INV,7,7);
	return result;
}
/*---------------------------------------------------------------------------------------------*/
Point2f datatrans::getcenter(Mat src/*LITTLE GRAY*/,Point2f ORG,Mat& contour)
{	
	//fit an ellipse	
	RotatedRect roRect;	
	roRect=fitEllipse(contour);  
	//make mask
	Mat mask = Mat::zeros(src.size(), CV_8UC1);
	ellipse(mask, roRect, cvScalar(255));
	Point2f seed;
	seed.x = roRect.center.x;
	seed.y = roRect.center.y;
	floodFill(mask, seed, 255, NULL, cvScalarAll(0), cvScalarAll(0), CV_FLOODFILL_FIXED_RANGE);
	//get the area
	Mat ROI;
	src.copyTo(ROI, mask);

	int nr=ROI.rows;
	int nc=ROI.cols;
	Point2f tempsum(0.f,0.f);
	int sum=0,count=0;
	for(int i=0;i<nr;i++)
	{
		const uchar* inData=ROI.ptr<uchar>(i);
		
		for(int j=0;j<nc;j++)
		{
			if(inData[j]>0)
			{
				sum+=inData[j];
				tempsum.x+=inData[j]*i;
				tempsum.y+=inData[j]*j;
				count++;
			}
			
		}
	}
	Point2f center=Point2f(tempsum.x/(sum*count),tempsum.y/(sum*count));

	return transorg(ORG.x,ORG.y,center);

}
/******************************svm*******************************************/
bool datatrans::categorize(Mat& gray)
{
	/*cvtColor(frame, gray, CV_BGR2GRAY);*/
	vector<KeyPoint> kp;
	Mat test;
	featureDetector->detect(gray, kp);
	bowDescriptorExtractor->compute(gray, kp, test);

	int flag=0;
	float best_score=777;
	string predicted_category;
	for(int i=0;i<categories;i++) 
	{
		string category = category_names[i];
		float prediction = svms[category]->predict(test, true);

		if(prediction >best_score) 
		{
			best_score = prediction;
			predicted_category = category;
			return true;
		}
	}
	
	return false;
	
}
/******************************************Math calculations****************************************************/
Point2f datatrans::vec(Point2f a,Point2f b)
{
	Point2f v;
	v.x=a.x-b.x;
	v.y=a.y-b.y;
	return v;
}

/*--------------------------------------------------------*/
float datatrans::sinval(Point2f v1,Point2f v2)
{
	float sinv,cosv2;
	
	cosv2=cosval(v1,v2);
	sinv=sqrt(1-cosv2);

	return sinv;
		
}
/*------------------------------------------------------------*/
void datatrans::vec4cal(double a,vector<double> cool)
{
	vector<double>::iterator it=cool.begin();
	while(it!=cool.end())
	{
		(*it)+=a;
		it++;
	}	
	
}
/*------------------------------------------------------------*/
float datatrans:: cosval(Point2f v1,Point2f v2)
{
	float mo1,mo2;
	mo1=mol(v1);
	mo2=mol(v2);

	return(((v1.x)*(v2.x)+(v2.y)*(v1.y))/(mo1*mo2));

}
/*------------------------------------------------------------*/
bool datatrans::circlelike(double polyarea,float R)
{
	if(R>=Tr)
	{
		double cool=polyarea/R/R/3.1416;
		if(cool>circleli)
		{
			return true;
		}
	}	
	
	return false;
	
}
/*-------------------------------------------------------------*/
float datatrans::mol(Point2f vec)
{
	float mo=sqrt(float((vec.x)*(vec.x)+(vec.y)*(vec.y)));
	return mo;
}
/*-------------------------------------------------------------*/
Point2f datatrans::transorg(int x0,int y0,Point2f prt)
{
	prt.x+=x0;
	prt.x+=y0;
	return prt;
}

/*--------------------------------------------------------------*/
int datatrans:: relats_bt_centers(vector<Point2f>& cts)
{
	
	vector<Point2f> temp;
	vector<vecmsg> vmsg;
	vector<vecmsg> vmsg_1;//x&y
	vector<vecmsg> vmsg_2;
	vecmsg tp;
	
	temp=cts;
	
	//get pairs of mol&vec between center points
	for(int i=0;i<numelli;i++)
	{
		for(int j=i+1;j<numelli;j++)
		{
			Point2f vec=(temp[i].x-temp[j].x,temp[i].y-temp[j].y);
			float mo=mol(vec);
			tp.pn1=i;
			tp.pn2=j;
		    tp.vec=vec;
			tp.mol=mo;
			vmsg.push_back(tp);
		}
	}
	//classify into 3 groups,find x&y axis	
	int sz=vmsg.size();
	
	vector<pair<int,float>> tag;
	pair<int,float> temptag;
	for(int i=0;i<sz;i++)
	{
		temptag.first=i;
		temptag.second=vmsg[i].mol;
		tag.push_back(temptag);
		for(int j=i+1;j<sz;j++)
		{
			double k=sinval(vmsg[i].vec,vmsg[j].vec);			
			if(k<0.08)
			{
				temptag.first=j;
				temptag.second=vmsg[j].mol;
				tag.push_back(temptag);				
			}
		}
		if(tag.size()==3)
		{			
			sort(tag.begin(),tag.end(),comp);
			if(int(0.5+tag[0].second/tag[1].second)==2)
			{
				vmsg_1.clear();
				vmsg_1.push_back(vmsg[tag[0].first]);
				vmsg_1.push_back(vmsg[tag[1].first]);
				vmsg_1.push_back(vmsg[tag[2].first]);
				tag.clear();
			}
			else if(int(0.5+tag[0].second/tag[2].second)==4)
			{
				vmsg_2.clear();
				vmsg_2.push_back(vmsg[tag[0].first]);
				vmsg_2.push_back(vmsg[tag[1].first]);
				vmsg_2.push_back(vmsg[tag[2].first]);
				tag.clear();
			}
		}
		if(vmsg_1.size()==3&&vmsg_2.size()==3)
		{
			vmsg.clear();
			break;
		}
		else
		{
			return 0;
		}
	}
	
	//find the origin point tags	
	int flagtaging=0;
	vector<Point2f> SORTED;
	Point2f a,b,c,d,e;
	bool b1,b2,b3,b4;
	int btag;
	
	if(flagtaging<5)
	{
		//y
		if(flagtaging<3)
		{
			b1=(vmsg_2[0].pn1==vmsg_2[1].pn1);
			b2=(vmsg_2[0].pn2==vmsg_2[1].pn1);
			b3=(vmsg_2[0].pn1==vmsg_2[1].pn2);
			b4=(vmsg_2[0].pn2==vmsg_2[1].pn2);
			if(b1)
			{
				c=centers[vmsg_2[0].pn1];
				a=centers[vmsg_2[0].pn2];
				b=centers[vmsg_2[1].pn2];
				btag=vmsg_2[1].pn2;
				flagtaging+=3;
			}
			else if(b2)
			{
				c=centers[vmsg_2[0].pn2];
				a=centers[vmsg_2[0].pn1];
				b=centers[vmsg_2[1].pn2];
				btag=vmsg_2[1].pn2;
				flagtaging+=3;
			}
			else if(b3)
			{
				c=centers[vmsg_2[0].pn1];
				a=centers[vmsg_2[0].pn2];
				b=centers[vmsg_2[1].pn1];
				flagtaging+=3;
				btag=vmsg_2[1].pn1;
			}

			else if(b4)
			{
				c=centers[vmsg_2[0].pn2];
				a=centers[vmsg_2[0].pn1];
				b=centers[vmsg_2[1].pn1];
				flagtaging+=3;
				btag=vmsg_2[1].pn1;
			}

		}
		
		//x
		
		b1=(vmsg_1[1].pn1==btag);
		b2=(vmsg_1[1].pn2==btag);
		b3=(vmsg_1[2].pn1==btag);
		b4=(vmsg_1[2].pn2==btag);

		
		if(btag!=NULL)
		{
			if(vmsg_1[0].pn1==btag)
			{
				e=centers[vmsg_1[0].pn2];
				flagtaging+=1;
				if(b1)
				{
					d=centers[vmsg_1[1].pn2];
					flagtaging+=1;
				}
				if(b2)
				{
					d=centers[vmsg_1[1].pn1];
					flagtaging+=1;
				}
				if(b3)
				{
					d=centers[vmsg_1[2].pn2];
					flagtaging+=1;
				}
				if(b4)
				{
					d=centers[vmsg_1[2].pn1];
					flagtaging+=1;
				}	
			}
			else if(vmsg_1[0].pn2==btag)
			{
				e=centers[vmsg_1[0].pn1];				
				flagtaging+=1;
				if(b1)
				{
					d=centers[vmsg_1[1].pn2];
					flagtaging+=1;
				}
				if(b2)
				{
					d=centers[vmsg_1[1].pn1];
					flagtaging+=1;
				}
				if(b3)
				{
					d=centers[vmsg_1[2].pn2];
					flagtaging+=1;
				}
				if(b4)
				{
					d=centers[vmsg_1[2].pn1];
					flagtaging+=1;
				}	
			}
		}
		if(flagtaging==5)
		{
			SORTED.push_back(a);
			SORTED.push_back(b);
			SORTED.push_back(c);
			SORTED.push_back(d);
			SORTED.push_back(e);
		}
		else
		{
			return -1;
		}
	}
	SORTED.swap(centers);
	return 1;
}
/*********************************mat2vector*****************/
vector<Point> datatrans::mat2vec(Mat& t1f)  
{  
	Mat m;
	t1f.copyTo(m);
	vector<Point> p;  
	p = Mat_<Point>(m);  

	return p;
}  
/*********************************************Î»×ËÍÆËã********************************************************/
Mat_<float> datatrans::solve_pnp(vector<Point2f> recog_cts2)//5pts
{    	
	 /*if(!OBj.empty()&&!recog_cts2.empty())
	 {*/
		 Mat_<float> trans(4,4);
		 //struct pos temp;
		 Mat Rvec;
		 Mat_<float> Tvec;
		 Mat raux,taux;
		 Mat inliers;
		 
		 solvePnPRansac(OBj,recog_cts2,cam,disc,raux,taux);

		
		  raux.convertTo(Rvec,CV_32F);
		  taux.convertTo(Tvec,CV_32F);//cool
		 

		  Mat_<float> r(3,3);
		  Rodrigues(Rvec,r);

		  inv(r);
		  Tvec=(-1)*(r)*Tvec;

		  for(int i=0;i<3;i++)
		  {
			  for(int j=0;j<3;j++)
			  {
				  trans(i,j)=r(i,j);
			  }
			  trans(i,3)=Tvec(i);
		  }

		  trans(3,0)=0;
		  trans(3,1)=0;
		  trans(3,2)=0;
		  trans(3,3)=1;

		  return trans;
	 
}
		  

		
/*
		 //eigen deal preparations
		  Eigen::Matrix3d rotMat;
		  Eigen::Matrix<float,3,1> t;

          cv2eigen(r,rotMat);
		  cv2eigen(Tvec,t);
		  
		  Eigen::Isometry3d T=Eigen::Isometry3d::Identity();
		  Eigen::AngleAxisd angle(rotMat);
		  Eigen::Matrix<float,4,4>trans=Eigen::Matrix<float,4,4>::Zero();
		  		  
		  t=(-1)*(angle.inverse().matrix())*t;
		  T=angle.inverse();
		  T(0,3)=t(0);
		  T(1,3)=t(1);
		  T(2,3)=t(2);
		  
		  for(int i=0;i<3;i++)
		  {
			  for(int j=0;j<4;j++)
			  {
				  trans(i,j)=T(i,j);
			  }
		  }
		 
		  trans(3,0)=0;
		  trans(3,1)=0;
		  trans(3,2)=0;
		  trans(3,3)=1;

	      cv::Mat_<float> tp(4,4);
		  cv::eigen2cv(trans,tp);
	
		  return tp;*/
	


void datatrans::inv(Mat_<float> a)                            
{	
	int n=a.rows;
	int *is,*js,i,j,k;
	float d,p;	
	float* pd1;
	float* pd2;
	is = new int[n];
	js = new int[n];

	for(k=0;k<n;k++)
	{		
		d = 0.0;
		for(i=k;i<n;i++)
			pd1=a.ptr<float>(i); 
			for(j=k;j<n;j++)
			{
				p = fabs(pd1[j]);
				if(p>d)
				{
					d = p;
					is[k] = i;
					js[k] = j;
				}
			}
			if(d + 1.f == 1.f)
			{
				delete[] is,js;
				AfxMessageBox(_T("qiyi"));
				return;
			}
			if(is[k]!=k)     
				pd1=a.ptr<float>(k); 
			    pd2=a.ptr<float>(is[k]); 
				for(j=0;j<n;j++)
				{
					p = pd1[j];
					pd1[j]=pd2[j];
					pd2[j]= p;
				}
				if(js[k]!=k)
					pd1=a.ptr<float>(k);
				    pd2=a.ptr<float>(js[k]);
					for(i=0;i<n;i++)
					{
						p =pd1[j];
						pd1[j] =pd2[j];
						pd2[j]= p;
					}
					pd1[k]=1.0/pd1[k];

					pd1=a.ptr<float>(k);
					pd2=a.ptr<float>(i);
					for(j=0;j<n;j++)
						if(j!=k) pd1[j]= pd1[j]*pd1[k];
					for(i=0;i<n;i++)					
						if(i!=k)
							for(j=0;j<n;j++)
								if(j!=k) pd2[j]= pd2[j]- pd2[k]*pd1[j];
					for(i=0;i<n;i++)
						if(i!=k) pd2[k]=-pd2[k]*pd1[k];
	}
	for(k=n-1;k>=0;k--)
	{
		if(js[k]!=k)
			pd1=a.ptr<float>(k);
		    pd2=a.ptr<float>(js[k]);
			for(j=0;j<n;j++)
			{
				p =pd1[j];pd1[j]=pd2[j];pd2[j]= p;
			}
			if(is[k]!=k)
				pd2=a.ptr<float>(i);
				for(i=0;i<n;i++)
				{
					p = pd2[k];pd2[k]= pd2[is[k]];pd2[is[k]]= p;
				}
	}
	delete[] is,js;
	delete[] pd1,pd2;
	return;
}
/*******************************************************************/

bool compar1(pair<int,double>& a,pair<int,double>& b)
{
	return(a.second>b.second);
}
	
		
	

