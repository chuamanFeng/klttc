#include "StdAfx.h"
#include "objectFind.h"


objectFind::objectFind(int cluster)
{	
	clusters = cluster;
	built=false;
	featureDetector = (new SurfFeatureDetector());
	descriptorExtractor = (new SurfDescriptorExtractor());
	bowtrainer = (new BOWKMeansTrainer(clusters));
	descriptorMatcher = (new FlannBasedMatcher());
	bowDescriptorExtractor = (new BOWImgDescriptorExtractor(descriptorExtractor,descriptorMatcher));
	
	tempfile=path("C:/Users/user/Documents/Visual Studio 2010/Projects/KLT/KLT/template11/");
	train_file=path("C:/Users/user/Documents/Visual Studio 2010/Projects/KLT/KLT/train_data/");
	vocab=path("C:/Users/user/Documents/Visual Studio 2010/Projects/KLT/KLT/vocabulary/");
	
	
	for(directory_iterator i(tempfile),end_iter; i != end_iter; i++) 
	{		
		string filename =tempfile.string() +i->path().filename().string();				
		string category = remove_extension(i->path().filename().string());
		
		Mat im = imread(filename), templ_im;
		objects[category] = im;		
		cvtColor(im,templ_im,CV_BGR2GRAY);
		
		templates[category]=templ_im;
		
		
	}
	AfxMessageBox(_T("INITIALIZED"));
	bool k=make_train_set();
	if(k)
	{
		make_pos_neg();
		build_vocab();
		train_classifiers();
		built=true;
		AfxMessageBox(_T("svm prepare OK"));
	}
	
}
/*---------------------------------------------------------------------------*/
/*
bool objectFind::categorize(Mat& gray)
{
	//cvtColor(frame, gray, CV_BGR2GRAY);
	vector<KeyPoint> kp;
	Mat test;
	featureDetector->detect(gray, kp);
	bowDescriptorExtractor -> compute(gray, kp, test);
	
	int flag=0;
	float best_score=777;
	string predicted_category;
	for(int i=0;i<categories;i++) 
	{
		string category = category_names[i];
		float prediction = svms[category].predict(test, true);

		if(prediction >best_score) 
		{
			best_score = prediction;
			predicted_category = category;
			flag=1;
		}
	}
	if(flag==0)
	{
		return false;
	}
	else
	{
		return true;
	}
}
*/
/*---------------------------------------------------------------------------*/
bool objectFind::make_train_set() 
{
	string category;
	
	for(recursive_directory_iterator i(train_file), end_iter; i != end_iter; i++) 
	{
		if(i.level() == 0)
		{		
			category = (i -> path()).filename().string();
			category_names.push_back(category);
		}
		
		else 
		{
			string filename = train_file.string() + category + string("/") +(i -> path()).filename().string();
			
			pair<string, Mat> p(category, imread(filename, CV_LOAD_IMAGE_GRAYSCALE));
			train_set.insert(p);
		}
	}

	categories = category_names.size();
	if(categories==0)
	{
		//AfxMessageBox(_T("no data"));
		return false;
	}
	else
	{
		return true;
	}
	
}
/*----------------------------------------------------------------------*/
void objectFind::make_pos_neg() 
{
	for(multimap<string, Mat>::iterator i = train_set.begin(); i != train_set.end(); i++)
	{
		
		string category = (*i).first;		
		Mat im = (*i).second, feat;
		
		vector<KeyPoint> kp;
		featureDetector -> detect(im, kp);
		bowDescriptorExtractor -> compute(im, kp, feat);
		
		Mat pos, neg;
		for(int cat_index = 0; cat_index < categories; cat_index++) 
		{
			string check_category = category_names[cat_index];
			
			if(check_category.compare(category) == 0)
			{
				positive_data[check_category].push_back(feat);
			}

			else
			{
				negative_data[check_category].push_back(feat);
			}
		}
	}

	for(int i = 0; i < categories; i++) 
	{
		string category = category_names[i];
		//cout << "CATEGORY" << category << ":" << positive_data[category].rows << " POS"<< negative_data[category].rows << "NEG" << endl;
	}
}
/*-----------------------------------------------------------------------*/
void objectFind::build_vocab()
{
	Mat vocab_descriptors;
	
	for(map<string, Mat>::iterator i = templates.begin(); i != templates.end(); i++) 
	{
		vector<KeyPoint> kp; Mat templ = (*i).second, desc;
		featureDetector -> detect(templ, kp);
		descriptorExtractor -> compute(templ, kp, desc);
		vocab_descriptors.push_back(desc);
	}
	
	bowtrainer -> add(vocab_descriptors);

	vocabu = bowtrainer->cluster();
	
	FileStorage fs(vocab.string()+ "VOCAB.xml", FileStorage::WRITE);
	fs << "VOCAB" << vocabu;
	fs.release();

	AfxMessageBox(_T("VOCAB OK"));
}
/*--------------------------------------------------------------------------*/
void objectFind::train_classifiers()
{
	bowDescriptorExtractor -> setVocabulary(vocabu);
	make_pos_neg();

	for(int i = 0; i < categories; i++) 
	{
		string category = category_names[i];
		
		Mat train_data = positive_data[category], train_labels = Mat::ones(train_data.rows, 1, CV_32S);
		
		train_data.push_back(negative_data[category]);
		Mat m = Mat::zeros(negative_data[category].rows, 1, CV_32S);
		train_labels.push_back(m);
		if(train_labels.empty())
		{
			AfxMessageBox(_T("shadiao666"));
		}
		if(train_labels.empty())
		{
			AfxMessageBox(_T("shadiao676"));
		}
		svms[category]->train(train_data, train_labels);
	
		string svm_filename = string("C:/Users/user/Documents/Visual Studio 2010/Projects/KLT/KLT/33/") + category + string("/SVM.xml");
		svms[category]->save(svm_filename.c_str());
		
		
	}
	AfxMessageBox(_T("SVM OK"));
	
}
/*----------------------------------------------------------------------------------*/

string objectFind::remove_extension(string full) 
{
	int last_idx = full.find_last_of(".");
	string name = full.substr(0, last_idx);
	return name;
}

/*-----------------------------------------------------------------------------------*/
objectFind::~objectFind(void)
{
}
